#ifdef __linux__
#pragma once

#include <signal.h>
#include <time.h>
#include "Timer.h"

namespace Unicore { namespace Library {

class Timer : public TimerBase
{
public:
    Timer(double initial, double period, std::function<void()> func) :
        TimerBase(initial, period, func)
    {}

    Timer(double period, std::function<void()> func) :
        Timer(period, period, func)
    {}

    ~Timer() override
    {
        Stop();
    }

protected:
    void StartImpl() override
    {
        sigevent se;
        se.sigev_notify = SIGEV_THREAD;
        se.sigev_value.sival_int = _id;
        se.sigev_notify_function = &Callback;
        se.sigev_notify_attributes = nullptr;

        if (timer_create(CLOCK_MONOTONIC, &se, &_timer) != 0)
            throw std::runtime_error("timer_create error");

        itimerspec ts{ToTimespec(_period), ToTimespec(_initial)};

        if (timer_settime(_timer, 0, &ts, nullptr) != 0)
            throw std::runtime_error("timer_settime error");
    }

    timespec ToTimespec(double duration)
    {
        // +1ns is used because Linux consider 0 as no timer
        auto s = (long) duration;
        auto ns = (long) ((duration - s) * 1e9);
        return timespec{s, ns + 1};
    }

    void StopImpl() override
    {
        itimerspec ts{0, 0, 0, 0};
        if (timer_settime(_timer, 0, &ts, nullptr) != 0)
            throw std::runtime_error("timer_settime error");

        if (timer_delete(_timer) != 0)
            throw std::runtime_error("timer_delete error");

        _timer = timer_t{};
    }

private:
    timer_t _timer;

    static void Callback(sigval_t sigval)
    {
        OnTimer(sigval.sival_int);
    }
};

}}

#endif
