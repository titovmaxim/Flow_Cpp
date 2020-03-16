#pragma once

#include <mutex>
#include "../Bases/MutableBase.h"
#include "../../Timer.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {

// Reduce OnChange pressure to be fired not frequently than an interval (in seconds).
// OnChange will fire in another thread!
// Reacts on first OnChange immediately, reducing following avalanche.

template<typename T>
class Throttle : public SourceBase<T>
{
public:
    explicit Throttle(double period, SourcePtr <T> source) :
        _timer(0.0, period, [this]() { OnTimer(); }),
        _source(source)
    {
        this->SubscribeTo(_source);
    }

    void OnChange() const override
    {
        const std::lock_guard<std::mutex> lock(_mutex);
        _changeDetected = true;
        _timer.Start(); // Ignored if timer is already running
    }

    T Value() const override
    {
        return _source->Value();
    }

private:
    const SourcePtr <T> _source;
    mutable Unicore::Library::Timer _timer;
    mutable std::mutex _mutex;
    mutable bool _changeDetected = false;

    void OnTimer()
    {
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            if (!_changeDetected) {
                _timer.Stop();
                return;
            }

            _changeDetected = false;
        }

        this->SourceBase<T>::OnChange();
    }
};

}}}}
