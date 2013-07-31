#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <unistd.h>

#include <logger>

#define LOG_MSG(level, msg) { \
        lgr::Logger logger(level, "test", __FILE__, __PRETTY_FUNCTION__, __LINE__); \
        logger << msg; \
        logger.printMsg(); \
    }

#define LOG_ERROR(msg) LOG_MSG(lgr::LOGLEVEL_ERROR, msg)
#define LOG_WARN(msg)  LOG_MSG(lgr::LOGLEVEL_WARN, msg)
#define LOG_INFO(msg)  LOG_MSG(lgr::LOGLEVEL_INFO, msg)
#define LOG_DEBUG(msg) LOG_MSG(lgr::LOGLEVEL_DEBUG, msg)
#define LOG_EXPR(msg)  LOG_DEBUG(#msg << " = " << (msg))

int
main(int argc, char const* argv[])
{
    LOG_INFO("This is a test of log info");
    LOG_WARN("This is a test of log warn");
    LOG_DEBUG("This is a test of log debug");

    const char* testString = "This is a test of log expr";
    LOG_EXPR(testString);

    int total = 100;
    for(int i = 0; i < total; i++) {
        lgr::ProgressBar progBar(lgr::LOGLEVEL_DEBUG, "test", __FILE__, __PRETTY_FUNCTION__, __LINE__);
        progBar.setProgress(i, total);
        progBar.printMsg();

        usleep(10000);
    }
    printf("\n");

    return EXIT_SUCCESS;
}