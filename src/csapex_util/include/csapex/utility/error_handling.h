#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

/// PROJECT
#include <csapex_util_export.h>
#include <csapex/utility/slim_signal.hpp>

/// SYSTEM
#include <csignal>
#if WIN32
#include <windows.h>
#endif

namespace csapex
{
namespace error_handling
{
CSAPEX_UTILS_EXPORT void init();

CSAPEX_UTILS_EXPORT void siginthandler(int);
CSAPEX_UTILS_EXPORT void sigtraphandler(int);
#if WIN32
CSAPEX_UTILS_EXPORT LONG WINAPI sigsegvhandler(EXCEPTION_POINTERS* ExceptionInfo);
#else
CSAPEX_UTILS_EXPORT void sigsegvhandler(int sig_num, siginfo_t* info, void* ucontext);
#endif
CSAPEX_UTILS_EXPORT void stop();
CSAPEX_UTILS_EXPORT void kill();

inline slim_signal::Signal<void()>& stop_request()
{
    static slim_signal::Signal<void()> s;
    return s;
}

}  // namespace error_handling
}  // namespace csapex

#endif  // ERROR_HANDLING_H
