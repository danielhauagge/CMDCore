//
//  vc_logger.cpp
//
//  Created by Daniel Hauagge on 6/15/12.
//  Copyright (c) 2012 Cornell University. All rights reserved.
//

#include "logger"

LOGGER_NAMESPACE_BEGIN

Logger::Logger(LogLevel loglevel, const char* moduleName, const char* filename, const char* function, int lineNumber):
    _loglevel(loglevel),
    _moduleName(moduleName),
    _filename(filename),
    _function(function),
    _lineNumber(lineNumber)
{
}

void
Logger::printMsg()
{
    if(_loglevel >= getLogLevel()) {
        std::string level = (_loglevel == LOGLEVEL_INFO)  ? "INFO  " :
                            (_loglevel == LOGLEVEL_WARN)  ? "WARN  " :
                            (_loglevel == LOGLEVEL_DEBUG) ? "DEBUG " :
                            "ERROR ";

        char datetime[128];
        time_t theTime = std::time(NULL);
        strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S ", std::localtime(&theTime));

        std::cout << "[" << _moduleName << "] " << datetime << level << " " << this->str() << std::endl;
    }

    if(_loglevel == LOGLEVEL_ERROR) throw std::runtime_error(this->str());
}

// TODO set this based on compilation flags
static LogLevel _minLogLevel = LOGLEVEL_DEBUG;

LogLevel
Logger::getLogLevel()
{
    return _minLogLevel;
}

void
Logger::setLogLevel(LogLevel loglevel)
{
    _minLogLevel = loglevel;
}

ProgressBar::ProgressBar(LogLevel loglevel, const char* moduleName, const char* filename, const char* function, int lineNumber):
    _loglevel(loglevel),
    _moduleName(moduleName),
    _filename(filename),
    _function(function),
    _lineNumber(lineNumber),
    _currentProgress(0),
    _totalProgress(0)
{
}

void
ProgressBar::setProgress(int curr, int total)
{
    _currentProgress = curr;
    _totalProgress = total;
}

static
int
round(double x)
{
    return (int) (x + 0.5);
}

void
ProgressBar::printMsg()
{
    const char* spin = "-\\|/";

    const int BAR_LENGTH = 30;
    int percent = round(double(BAR_LENGTH) * double(_currentProgress) / _totalProgress);

    std::stringstream msg;

    msg << "[";
    for(int i = 0; i < percent; i++) msg << "=";
    for(int i = percent; i < BAR_LENGTH; i++) msg << " ";
    msg << "]\r";
    printf(msg.str().c_str());
    fflush(stdout);
}

LOGGER_NAMESPACE_END
