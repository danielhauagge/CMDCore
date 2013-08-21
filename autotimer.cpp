#include "autotimer"

CLC_NAMESPACE_BEGIN

// ****************************************************************************
// * AutoTimer                                                                *
// ****************************************************************************

AutoTimer::AutoTimer(const std::string& msg,
                     LogLevel logLevel,
                     const std::string& moduleName,
                     const std::string& fileName,
                     const std::string& function,
                     int lineNumber):
    m_msg(msg),
    m_logLevel(logLevel),
    m_moduleName(moduleName),
    m_fileName(fileName),
    m_function(function),
    m_lineNumber(lineNumber)
{
    start();
}

AutoTimer::~AutoTimer()
{
    stop();

    char formatedMsg[512];
    snprintf(formatedMsg, sizeof(formatedMsg), "%s: %.3f sec", m_msg.c_str(), timeInSeconds());
    //LOG_INFO(formatedMsg);
    clc::Logger::getInstance()->setMessage() << formatedMsg;
    clc::Logger::getInstance()->printMessage(m_logLevel, m_moduleName, m_fileName, m_function, m_lineNumber);
}

CLC_NAMESPACE_END