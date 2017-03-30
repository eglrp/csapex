/// HEADER
#include <csapex/core/exception_handler.h>

/// PROJECT
#include <csapex/core/csapex_core.h>

/// SYSTEM
#include <iostream>

using namespace csapex;

ExceptionHandler::ExceptionHandler(bool fatal_exceptions)
    : fatal_exceptions_(fatal_exceptions)
{

}

ExceptionHandler::~ExceptionHandler()
{
    // do nothing
}

void ExceptionHandler::pause()
{
    assertion_failed();
}


bool ExceptionHandler::handleException(std::exception_ptr eptr)
{
    try {
        if (eptr) {
            std::rethrow_exception(eptr);
        }

    } catch(const std::exception& e) {
        if(fatal_exceptions_) {
            std::cerr << "caught an exception in --fatal-exceptions mode: Abort!" << std::endl;
            std::abort();
        }
        std::cerr << "Uncaught exception:" << e.what() << std::endl;
        return false;

    } catch(const csapex::Failure& assertion) {
        handleAssertionFailure(assertion);
    } catch(const std::string& s) {
        std::cerr << "Uncaught exception (string) exception: " << s << std::endl;
    } catch(...) {
        std::cerr << "Uncaught exception of unknown type and origin!" << std::endl;
        std::abort();
    }

    return true;
}

void ExceptionHandler::handleAssertionFailure(const Failure &assertion)
{
    pause();

    assertion.printStackTrace();
    std::abort();
}
