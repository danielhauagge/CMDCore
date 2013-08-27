#include "CMDCore/timer"

CMDC_NAMESPACE_BEGIN

// ****************************************************************************
// * Timer                                                                    *
// ****************************************************************************

Timer::Timer()
{
    _timer.start();
}

void
Timer::start()
{
    _timer.start();
}

void
Timer::stop()
{
    _timer.stop();
}

void
Timer::resume()
{
    _timer.resume();
}

bool
Timer::isStopped() const
{
    return _timer.is_stopped();
}

double
Timer::timeInSeconds()
{
    int64_t nanoElapse = timeInNanoSeconds();
    return double(nanoElapse) / 1e9;
}

int64_t
Timer::timeInNanoSeconds()
{
    boost::timer::cpu_times elapsed = _timer.elapsed();
    return int64_t(elapsed.wall);
}

CMDC_NAMESPACE_END