#pragma once

#include <mutex>
#include <map>
#include <functional>
#include <stdexcept>

namespace Unicore { namespace Library {

class TimerBase
{
public:
    TimerBase(double initial, double period, const std::function<void()> &func) :
        _initial(initial),
        _period(period),
        _func(func)
    {
        std::lock_guard<std::recursive_mutex> lock(GlobalMutex);

        // look for first empty Id slot
        for (;;) {
            _id = NextId++;
            auto it = Timers.find(_id);
            if (it == Timers.end()) {
                Timers.insert(it, std::pair<int, TimerBase *>{_id, this});
                return;
            }
        }
    }

    // Block till timer will be cancelled and last callback finished
    virtual ~TimerBase()
    {
        std::lock_guard<std::recursive_mutex> lockStatic(GlobalMutex);
        std::lock_guard<std::mutex> lock(_funcMutex); // Assures that func() will be finised before destructor fires
        Timers.erase(_id);
    }

    void Start()
    {
        if (_isActive)
            return;

        _isActive = true;
        StartImpl();
    }

    void Stop()
    {
        if (!_isActive)
            return;

        _isActive = false;
        StopImpl();
    }

    bool IsActive() const
    {
        return _isActive;
    }

protected:
    static int NextId;
    static std::recursive_mutex GlobalMutex;
    static std::map<int, TimerBase *> Timers;

    const double _initial;
    const double _period;
    const std::function<void()> _func;
    std::mutex _funcMutex;
    int _id;
    volatile bool _isActive = false;

    // GlobalMutex and _funcMutex locks MUST overlap!
    static void OnTimer(int id)
    {
        GlobalMutex.lock();

        auto it = Timers.find(id);
        if (it == Timers.end()) {
            GlobalMutex.unlock();
            return;
        }

        auto that = it->second;
        std::function<void()> func(that->_func);

        std::lock_guard<std::mutex>(that->_funcMutex); // Assures that func() will be finised before destructor fires

        GlobalMutex.unlock();

        func();
    }

    virtual void StartImpl() = 0;

    virtual void StopImpl() = 0;
};

int TimerBase::NextId = 0;
std::recursive_mutex TimerBase::GlobalMutex;
std::map<int, TimerBase *> TimerBase::Timers;

}}

// Include one of platform-dependent implementations
#include "Timer_Linux.h"
#include "Timer_Windows.h"
