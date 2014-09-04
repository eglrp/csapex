/// HEADER
#include <csapex.h>

/// PROJECT
#include <csapex/core/csapex_core.h>
#include <csapex/core/settings.h>
#include <csapex/view/csapex_window.h>
#include <csapex/view/box.h>
#include <csapex/model/node_factory.h>
#include <csapex/model/node_worker.h>
#include <csapex/model/graph_worker.h>
#include <csapex/core/graphio.h>
#include <csapex/utility/thread.h>
#include <csapex/utility/error_handling.h>
#include <csapex/core/drag_io.h>
#include <csapex/view/designer.h>
#include <csapex/view/designer_view.h>
#include <csapex/model/node_factory.h>
#include <utils_param/parameter_factory.h>
#include <csapex/view/widget_controller.h>
#include <csapex/view/designer_scene.h>

/// SYSTEM
#include <boost/program_options.hpp>
#include <QtGui>
#include <execinfo.h>
#include <stdlib.h>

namespace po = boost::program_options;

using namespace csapex;


CsApexApp::CsApexApp(int& argc, char** argv, bool headless)
    : QApplication(argc, argv, !headless)
{}

bool CsApexApp::notify(QObject* receiver, QEvent* event) {
    try {
        return QApplication::notify(receiver, event);

    } catch(const std::exception& e) {
        ErrorState* er = dynamic_cast<ErrorState*> (receiver);
        NodeWorker* bw = dynamic_cast<NodeWorker*> (receiver);

        if(er) {
            er->setError(true, e.what());
        } else if(bw) {
            bw->triggerError(true, e.what());
        } else {
            std::cerr << "Uncatched exception:" << e.what() << std::endl;
        }

        return false;

    } catch(const std::string& s) {
        std::cerr << "Uncatched exception (string) exception: " << s << std::endl;
    } catch(...) {
        std::cerr << "Uncatched exception of unknown type and origin!" << std::endl;
        std::abort();
    }

    return true;
}

Main::Main(CsApexApp& a)
    : app(a), splash(NULL)
{
    csapex::thread::set_name("cs::APEX main");
}

Main::~Main()
{
    delete splash;
}

int Main::run()
{
    csapex::error_handling::init();

    int result = app.exec();

    return result;
}

int Main::main(bool headless, const std::string& config, const std::string& path_to_bin, const std::vector<std::string>& additional_args)
{
    Settings settings;
    settings.setCurrentConfig(config);

    if(!settings.knows("path_to_bin")) {
        settings.add(param::ParameterFactory::declareFileInputPath("path_to_bin", path_to_bin));
    } else {
        settings.set("path_to_bin", path_to_bin);
    }

    if(!settings.knows("headless")) {
        settings.add(param::ParameterFactory::declareBool("headless", headless));
    } else {
        settings.set("headless", headless);
    }

    if(!settings.knows("additional_args")) {
        settings.add(param::ParameterFactory::declareValue< std::vector<std::string> >("additional_args", additional_args));
    } else {
        settings.set("additional_args", additional_args);
    }


    NodeFactory::Ptr node_factory(new NodeFactory(settings));

    Graph::Ptr graph(new Graph);
    GraphWorker::Ptr graph_worker(new GraphWorker(&settings, graph.get()));
    WidgetControllerPtr widget_control (new WidgetController(settings, graph, node_factory.get()));

    CommandDispatcher dispatcher(settings, graph_worker, widget_control);

    CsApexCore core(settings, graph_worker, node_factory.get(), &dispatcher);

    if(!headless) {
        app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

        /*
         * There seems to be a bug in Qt4:
         *  A race condition in QApplication sometimes causes a deadlock on startup when using the GTK theme!
         *  Workaround: Specify as a program argument: '-style Plastique' for the Plastique theme or other non-GTK based theme.
         */
        QPixmap pm(":/apex_splash.png");
        splash = new QSplashScreen (pm);
        splash->show();

        app.processEvents();

        DragIO drag_io(graph.get(), &dispatcher, widget_control);
        DesignerScene* scene = new DesignerScene(graph, &dispatcher, widget_control);
        DesignerView* view = new DesignerView(scene, graph, &dispatcher, widget_control, drag_io);
        Designer* designer = new Designer(settings, graph, &dispatcher, widget_control, view, scene);

        widget_control->setDesigner(designer);

        CsApexWindow w(core, &dispatcher, widget_control, graph_worker, designer);
        QObject::connect(&w, SIGNAL(statusChanged(QString)), this, SLOT(showMessage(QString)));

        csapex::error_handling::stop_request().connect(boost::bind(&CsApexWindow::close, &w));

        core.init(&drag_io);
        w.start();
        core.startup();

        w.show();
        splash->finish(&w);

        int res = run();

        return res;

    } else {
        core.init(NULL);
        csapex::error_handling::stop_request().connect(boost::bind(&csapex::error_handling::kill));
        core.startup();
        return run();
    }
}

void Main::showMessage(const QString& msg)
{
    if(splash->isVisible()) {
        splash->showMessage(msg, Qt::AlignBottom | Qt::AlignRight, Qt::black);
    }
    app.processEvents();
}


int main(int argc, char** argv)
{
    std::string path_to_bin(argv[0]);

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "show help message")
            ("dump", "show variables")
            ("headless", "run without gui")
            ("input", "config file to load")
            ("ros-name", "(ros parameter (provided by launch files))")
            ("ros-log", "(ros parameter (provided by launch files))")
            ;

    po::positional_options_description p;
    p.add("input", 1);
    p.add("ros-name", 1);
    p.add("ros-log", 1);

    // first check for --headless parameter
    // this has to be done before the qapp can be created, which
    // has to be done before parameters can be read.
    bool headless = false;
    for(int i = 1; i < argc; ++i) {
        if(std::string(argv[i]) == "--headless") {
            headless = true;
            break;
        }
    }

    if(!headless) {
        // if headless not requested, check if there is a display
        // if not, we enforce headless mode
        if(!getenv("DISPLAY")) {
            headless = true;
            std::cout << "warning: enforcing headless mode because there is no display detected" << std::endl;
        }
    }

    // filters all qt parameters from argv
    CsApexApp app(argc, argv, headless);

    // now check for remaining parameters
    po::variables_map vm;
    std::vector<std::string> additional_args;

    try {
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).positional(p).run();

        po::store(parsed, vm);

        po::notify(vm);

        additional_args = po::collect_unrecognized(parsed.options, po::include_positional);

    } catch(const std::exception& e) {
        std::cerr << "cannot parse parameters: " << e.what() << std::endl;
        return 4;
    }


    // display help?
    if(vm.count("help")) {
        std::cerr << desc << std::endl;
        return 1;
    }
    if(vm.count("dump")) {
        std::cout << "to be passed on:\n";
        for(std::size_t i = 0; i < additional_args.size(); ++i) {
            std::cout << additional_args[i] << "\n";
        }
        std::cout << std::flush;
        return 0;
    }

    // which file to use?
    std::string input;
    if (vm.count("input")) {
        input = vm["input"].as<std::string>();
    } else {
        input = Settings::default_config;
    }

    // start the app
    Main m(app);
    return m.main(headless, input, path_to_bin, additional_args);
}

