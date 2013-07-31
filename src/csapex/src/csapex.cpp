/// PROJECT
#include <csapex/csapex_window.h>
#include <csapex/box.h>
#include <csapex/boxed_object.h>

/// SYSTEM
#include <boost/program_options.hpp>
#include <QtGui>
#include <opencv2/opencv.hpp>
#include <QApplication>
#include <signal.h>
#include <X11/Xlib.h>

namespace po = boost::program_options;

using namespace csapex;

void siginthandler(int param)
{
    printf("User pressed Ctrl+C\n");
    exit(1);
}

struct EvaluationApplication : public QApplication {
    EvaluationApplication(int& argc, char** argv)
        : QApplication(argc, argv)
    {}

    virtual bool notify(QObject* receiver, QEvent* event) {
        try {
            return QApplication::notify(receiver, event);

        } catch(const std::exception& e) {
            BoxedObject* bo = dynamic_cast<BoxedObject*> (receiver);
            Box* box = dynamic_cast<Box*> (receiver);
            BoxWorker* bw = dynamic_cast<BoxWorker*> (receiver);

            if(bo) {
                bo->setError(true, e.what());
            } else if(box) {
                box->getContent()->setError(true, e.what());
            } else if(bw) {
                bw->parent()->getContent()->setError(true, e.what());
            } else {
                std::cerr << "Uncatched exception:" << e.what() << std::endl;
            }

            return false;

        } catch(const std::string& s) {
            std::cerr << "Uncatched exception (string) exception: " << s << std::endl;
        } catch(...) {
            std::cerr << "Uncatched exception of unknown type and origin!" << std::endl;
            throw;
        }

        return true;
    }
};

int main(int argc, char** argv)
{
    // initialize XLib, so that OpenGL can safely be used in a multithreaded environment
//    if(XInitThreads() == 0) {
//        std::cout << "cannot start the application, XLib couldn't be initialized" << std::endl;
//    }

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "show help message")
            ("directory",  po::value<std::string>()->default_value("."), "working directory")
            ("second_directory",  po::value<std::string>(), "comparation directory")
            ;

    po::positional_options_description p;
    p.add("directory", 1).add("second_directory", 1);

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

    } catch(po::unknown_option& e) {
        std::cerr << "Error parsing parameters: " << e.what() << "\n";
        std::cerr << desc << std::endl;
        return 2;
    }

    po::notify(vm);

    if(vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    std::string directory = vm["directory"].as<std::string>();
    std::cout << "working directory is " << directory << std::endl;

    EvaluationApplication app(argc, argv);
    EvaluationWindow w;

    if(vm.find("second_directory") != vm.end()) {
        std::string comp_directory = vm["second_directory"].as<std::string>();
        std::cout << "comparation directory is " << comp_directory << std::endl;
    }

    w.start();

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    signal(SIGINT, siginthandler);
    int result = app.exec();

    return result;
}
