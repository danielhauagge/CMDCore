//
//  vc_logger.cpp
//
//  Created by Daniel Hauagge on 6/15/12.
//  Copyright (c) 2012 Cornell University. All rights reserved.
//

#include "CMDCore/logger"

CMDC_NAMESPACE_BEGIN

static
void
printFormattedMessage(LogLevel loglevel,
                      const std::string& moduleName,
                      const std::string& filename,
                      const std::string& function,
                      int lineNumber,
                      const std::string& message)
{
    const char* level = (loglevel == LOGLEVEL_INFO)  ? "INFO  " :
                        (loglevel == LOGLEVEL_WARN)  ? "WARN  " :
                        (loglevel == LOGLEVEL_DEBUG) ? "DEBUG " :
                        "ERROR ";

    char datetime[128];
    time_t theTime = std::time(NULL);
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S ", std::localtime(&theTime));

    std::cout << "[" << moduleName << "] " << datetime << level << " " << message << std::endl;
}

static
int
round(double x)
{
    return (int) (x + 0.5);
}

//Logger* Logger::s_inst = NULL;
LogLevel Logger::s_minLogLevelPrint = LOGLEVEL_WARN;
LogLevel Logger::s_minLogLevelError = LOGLEVEL_ERROR;

// void
// Logger::init(LogLevel minPrint, LogLevel minError)
// {
//     // if(s_inst == NULL) s_inst = new Logger();
//     s_minLogLevelPrint = minPrint;
//     s_minLogLevelError = minError;
// }

void
Logger::setLogLevels(LogLevel minPrint, LogLevel minError)
{
    s_minLogLevelPrint = minPrint;
    assert(minError <= LOGLEVEL_NOPRINTING);
    s_minLogLevelError = minError;
}

// void
// Logger::deinit()
// {
//     // if(s_inst != NULL) delete s_inst;
//     // s_inst = NULL;
// }

Logger::Logger():
    m_ticks(0)
{

}

std::ostream&
Logger::startProgressBar(LogLevel logLevel)
{
    m_logLevel = logLevel;
    m_progBarWidth = 0;
    m_progbarDone = false;
    m_msg.str("");
    m_timer.start();
    m_prevBarFill = -1;
    m_timePrevUpdate = -100;

    return m_msg;
}

void
Logger::updateProgressBar(int curr, int total,
                          const std::string& moduleName, const std::string& filename,
                          const std::string& function, int lineNumber)
{
    if(m_logLevel < s_minLogLevelPrint) return;

    if(m_progbarDone) return;
    double totalTime = m_timer.timeInSeconds();

    const char spin[] = {'-', '\\', '|', '/'};
    const int BAR_LENGTH = 30;
    const int PERC_WIDTH = 3;

    float complete = double(curr) / total;

    // If we're done, then clear the progress bar, print the final message + time to complete
    if(curr >= total - 1) {
        m_progbarDone = true;

        // Erase progress bar before printing final message
        for(int i = 0; i < m_progBarWidth; i++) std::cout << " ";
        std::cout << "\r";

        std::stringstream msg;
        msg << m_msg.str() << " ("
            << std::setprecision(3) << std::fixed << std::showpoint
            << totalTime << " sec)";

        printFormattedMessage(m_logLevel, moduleName, filename, function, lineNumber, msg.str());
    } else {
        int barFill = round(double(BAR_LENGTH) * complete);

        double timeBetweenCalls = totalTime - m_timePrevUpdate;
        if(timeBetweenCalls < 0.2) return;
        m_timePrevUpdate = totalTime;
        m_ticks++;


        std::stringstream progBar;
        progBar << spin[m_ticks % sizeof(spin)];

        if(m_prevBarFill != barFill) {
            double eta = (total - curr) * totalTime / curr;

            progBar << "[";
            for(int i = 0; i < barFill; i++) progBar << "=";
            for(int i = barFill; i < BAR_LENGTH; i++) progBar << " ";
            progBar << "]"
                    << " " << std::setw(PERC_WIDTH) << round(complete * 100.0) << "% "
                    << " " << m_msg.str()
                    << " ("
                    << std::setprecision(3) << std::fixed << std::showpoint
                    << totalTime << " sec";

            if(curr == 0) progBar << ")";
            else {
                progBar << ", ETA: "
                        << std::setprecision(3) << std::fixed << std::showpoint
                        << eta << " sec)";
            }

            int padding = (m_progBarWidth - progBar.str().size() - 1);
            for(int i = 0; i < padding; i++) progBar << " ";
            m_progBarWidth = progBar.str().size() + 1;
        }

        progBar << "\r";
        std::cout << progBar.str() << std::flush;

        m_prevBarFill = barFill;
    }
}

void
Logger::printMessage(LogLevel loglevel,
                     const std::string& moduleName,
                     const std::string& filename,
                     const std::string& function,
                     int lineNumber) const
{
    if(loglevel >= s_minLogLevelPrint) {
        printFormattedMessage(loglevel, moduleName, filename, function, lineNumber, m_msg.str());
    }

    if(loglevel >= s_minLogLevelError) throw std::runtime_error(m_msg.str());
}

CMDC_NAMESPACE_END
