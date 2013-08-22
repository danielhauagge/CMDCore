//
//  vc_logger.cpp
//
//  Created by Daniel Hauagge on 6/15/12.
//  Copyright (c) 2012 Cornell University. All rights reserved.
//

#include "logger"

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

Logger* Logger::s_inst = NULL;
LogLevel Logger::s_minLogLevelPrint = LOGLEVEL_INFO;
LogLevel Logger::s_minLogLevelError = LOGLEVEL_ERROR;

void
Logger::init()
{
    s_inst = new Logger();
}

void
Logger::deinit()
{
    if(s_inst != NULL) delete s_inst;
}

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

    return m_msg;
}

void
Logger::updateProgressBar(int curr, int total,
                          const std::string& moduleName, const std::string& filename,
                          const std::string& function, int lineNumber)
{
    if(m_progbarDone) return;
    double totalTime = m_timer.timeInSeconds();

    m_ticks++;

    const char spin[] = {'-', '\\', '|', '/'};
    const int BAR_LENGTH = 30;
    const int PERC_WIDTH = 3;

    float complete = double(curr) / total;

    // If we're done, then clear the progress bar
    if(curr >= total - 1) {
        m_progbarDone = true;

        // Clean up progress bar before printing final message
        for(int i = 0; i < m_progBarWidth; i++) std::cout << " ";
        std::cout << "\r";

        std::stringstream msg;
        msg << m_msg.str() << " ("
            << std::setprecision(3) << std::fixed << std::showpoint
            << totalTime << " sec)";

        printFormattedMessage(m_logLevel, moduleName, filename, function, lineNumber, msg.str());
    } else {
        int barFill = round(double(BAR_LENGTH) * complete);

        std::stringstream progBar;

        double eta = (total - curr) * totalTime / curr;

        progBar << spin[m_ticks % sizeof(spin)];

        if(m_prevBarFill != barFill) {

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
        //std::cout << m_progBarWidth << std::endl;

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
