#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <unistd.h>

#include <CMDCore/cmdcore>
#include <CMDCore/logger>
#include <CMDCore/timer>
#include <CMDCore/autotimer>

#define LOG_ERROR(msg) LOG_MSG(cmdc::LOGLEVEL_ERROR, msg)
#define LOG_WARN(msg)  LOG_MSG(cmdc::LOGLEVEL_WARN, msg)
#define LOG_INFO(msg)  LOG_MSG(cmdc::LOGLEVEL_INFO, msg)
#define LOG_DEBUG(msg) LOG_MSG(cmdc::LOGLEVEL_DEBUG, msg)
#define LOG_EXPR(msg)  LOG_DEBUG(#msg << " = " << (msg))

#define LOG_MSG(level, msg) { \
        cmdc::Logger::getInstance()->setMessage() << msg; \
        cmdc::Logger::getInstance()->printMessage(level, "test", __FILE__, __PRETTY_FUNCTION__, __LINE__); \
    }
#define PROGBAR_START(msg) cmdc::Logger::getInstance()->startProgressBar(cmdc::LOGLEVEL_INFO) << msg
#define PROGBAR_UPDATE(curr, total) cmdc::Logger::getInstance()->updateProgressBar(curr, total, \
        "test", __FILE__, __PRETTY_FUNCTION__, __LINE__)

#define TIMER(msg) cmdc::AutoTimer __timer(msg, cmdc::LOGLEVEL_INFO, "test", __FILE__, __PRETTY_FUNCTION__, __LINE__)

int
main(int argc, char const* argv[])
{
    using namespace cmdc;
    //cmdc::init();
    cmdc::Logger::setLogLevels(cmdc::LOGLEVEL_INFO);

    TIMER("Total time");

    LOG_INFO("Working as expected");
    LOG_WARN("Hey, things aren't working exactly as planned!");

    int total = 100;
    PROGBAR_START("A progress bar message");
    for(int i = 0; i < total; i++) {
        PROGBAR_UPDATE(i, total);
        usleep(30000);
    }

    LOG_INFO("After the progress bar");

    LOG_INFO("Shoudl not print debug message next");
    LOG_DEBUG("This should not print");
    LOG_INFO("Changing log level to debug, should see a debug message next");
    cmdc::Logger::setLogLevels(cmdc::LOGLEVEL_DEBUG);
    LOG_DEBUG("This should print");

    // cmdc::deinit();
    return EXIT_SUCCESS;
}
