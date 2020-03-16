#ifdef _WIN32
#pragma once

#include <windows.h>
#include "Timer.h"

namespace Unicore { namespace Library {

class Timer : public TimerBase
{
public:
    Timer(double initial, double period, const std::function<void()> &func) :
        TimerBase(initial, period, func)
    {}

    Timer(double period, const std::function<void()> &func) :
        TimerBase(period, period, func)
    {}

    ~Timer() override
    {
        Stop();
    }

protected:
    void StartImpl() override
    {
        auto ok = CreateTimerQueueTimer(
            &_handle,
            nullptr,
            (WAITORTIMERCALLBACK) WindowsTimerCallback,
            (void *) (long long) _id,
            (DWORD) (_initial * 1e3 + 0.5),
            (DWORD) (_period * 1e3 + 0.5),
            WT_EXECUTEDEFAULT
        );

        if (ok == 0)
            throw std::runtime_error("Can't create timer");
    }

    void StopImpl() override
    {
        DeleteTimerQueueTimer(nullptr, _handle, NULL); // NULL - does not wait, INVALID_HANDLE_VALUE - wait untill timer is cancelled and callback is really finished
        _handle = nullptr;
    }

private:
    HANDLE _handle = nullptr;

    static void WindowsTimerCallback(void *id)
    {
        OnTimer((int) (long long) id);
    }
};

}}

#endif
