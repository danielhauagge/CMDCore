#include "cmdcore"
#include "logger"

CMDC_NAMESPACE_BEGIN

void
init()
{
    cmdc::Logger::init();
}

void
deinit()
{
    cmdc::Logger::deinit();
}

CMDC_NAMESPACE_END