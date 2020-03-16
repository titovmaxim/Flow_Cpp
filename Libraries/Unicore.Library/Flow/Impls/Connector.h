#pragma once

#include <functional>
#include "../Interfaces.h"
#include "../Bases/SourceBase.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {


template<typename T>
class Connector : public SourceBase<T>, public virtual IConnector<T>
{
public:
    explicit Connector(const T &valueWhenDisconnected) :
        _valueWhenDisconnected(valueWhenDisconnected)
    {};

    T Value() const override
    {
        const std::lock_guard<std::mutex> lock(_mutex);
        return _source == nullptr
               ? _valueWhenDisconnected
               : _source->Value();
    }

    bool IsConnected() const override
    {
        const std::lock_guard<std::mutex> lock(_mutex);
        return _source != nullptr;
    }

    void Disconnect() override
    {
        ConnectTo(nullptr);
    }

    void ConnectTo(SourcePtr <T> source) override
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        if (_source == source)
            return;

        // Unsubscribe automatically from previous source (in destructor of _temporalSubscription)
        _temporalSubscription = nullptr;

        _source = source;

        if (source != nullptr) {
            _temporalSubscription = Flow::Callback([this]() {
                this->OnChange();
            });
            _source->AddSubscription(_temporalSubscription);
        }

        this->OnChange();
    }

private:
    const T _valueWhenDisconnected;
    mutable std::mutex _mutex;
    mutable SourcePtr <T> _source = nullptr;
    CallbackPtr _temporalSubscription;
};

}}}}
