/// HEADER
#include <csapex/view/csapex_view_core_remote.h>


/// COMPONENT
#include <csapex/core/csapex_core.h>
#include <csapex/view/utility/message_renderer_manager.h>
#include <csapex/view/node/node_adapter_factory.h>
#include <csapex/view/designer/drag_io.h>
#include <csapex/model/graph_facade.h>
#include <csapex/scheduling/thread_pool.h>
#include <csapex/command/dispatcher.h>
#include <csapex/io/session.h>

/// SYSTEM
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/version.hpp>
#if (BOOST_VERSION / 100000) >= 1 && (BOOST_VERSION / 100 % 1000) >= 54
namespace bf3 = boost::filesystem;
#else
namespace bf3 = boost::filesystem3;
#endif

using boost::asio::ip::tcp;
using namespace csapex;


CsApexViewCoreRemote::CsApexViewCoreRemote(const std::string &ip, int port)
    : socket(io_service),
      resolver(io_service),
      resolver_iterator(boost::asio::connect(socket, resolver.resolve({ip, std::to_string(port)}))),
      session_(std::make_shared<Session>(std::move(socket))),
      settings_(std::make_shared<SettingsRemote>(session_))
{
}



NodeAdapterFactoryPtr CsApexViewCoreRemote::getNodeAdapterFactory()
{
    return node_adapter_factory_;
}

std::shared_ptr<DragIO> CsApexViewCoreRemote::getDragIO()
{
    return drag_io;
}

/// PROXIES
ExceptionHandler& CsApexViewCoreRemote::getExceptionHandler() const
{
    //return exception_handler_;
}


PluginLocatorPtr CsApexViewCoreRemote::getPluginLocator() const
{
    return nullptr;//core_->getPluginLocator();
}

CommandExecutorPtr CsApexViewCoreRemote::getCommandDispatcher()
{
    return dispatcher_;
}

Settings& CsApexViewCoreRemote::getSettings() const
{
    return *settings_;
}


GraphFacadePtr CsApexViewCoreRemote::getRoot()
{
    return nullptr;//core_->getRoot();
}

ThreadPoolPtr CsApexViewCoreRemote::getThreadPool()
{
    // TODO: replace with proxy
    //apex_assert_hard(//core_->getThreadPool());
    return nullptr;//core_->getThreadPool();
}
NodeFactoryPtr CsApexViewCoreRemote::getNodeFactory() const
{
    // TODO: replace with proxy
    apex_assert_hard(node_factory_);
    return node_factory_;
}
SnippetFactoryPtr CsApexViewCoreRemote::getSnippetFactory() const
{
    // TODO: replace with proxy
    apex_assert_hard(snippet_factory_);
    return snippet_factory_;
}
ProfilerPtr CsApexViewCoreRemote::getProfiler() const
{
    // TODO: replace with proxy
    //apex_assert_hard(//core_->getProfiler() != nullptr);
    return nullptr;//core_->getProfiler();
}

void CsApexViewCoreRemote::sendNotification(const std::string& notification, ErrorState::ErrorLevel error_level)
{
    //core_->sendNotification(notification, error_level);
}



/// RELAYS

void CsApexViewCoreRemote::reset()
{
    //core_->reset();
}


void CsApexViewCoreRemote::load(const std::string& file)
{
    //core_->load(file);
}

void CsApexViewCoreRemote::saveAs(const std::string& file, bool quiet)
{
    //core_->saveAs(file, quiet);
}

bool CsApexViewCoreRemote::isPaused() const
{
    return false; //core_->isPaused();
}

void CsApexViewCoreRemote::setPause(bool paused)
{
    //core_->setPause(paused);
}


bool CsApexViewCoreRemote::isSteppingMode() const
{
    return false; //core_->isSteppingMode();
}

void CsApexViewCoreRemote::setSteppingMode(bool stepping)
{
    //core_->setSteppingMode(stepping);
}

void CsApexViewCoreRemote::step()
{
    //core_->step();
}


void CsApexViewCoreRemote::shutdown()
{
    //core_->shutdown();
}

void CsApexViewCoreRemote::clearBlock()
{
    //core_->getRoot()->clearBlock();
}

void CsApexViewCoreRemote::resetActivity()
{
    //core_->getRoot()->resetActivity();
}
