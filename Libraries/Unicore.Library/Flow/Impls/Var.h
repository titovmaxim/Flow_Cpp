#pragma once

#include "../Bases/MutableBase.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {

template<typename T>
class Var : public MutableBase<T>
{
public:
    explicit Var(const T &initialValue)
        : _value(initialValue)
    {
    };

    T Value() const override
    {
        const std::lock_guard<std::mutex> lock(_mutex);
        return _value;
    }

    void SetValue(const T &newValue) override
    {
        {
            const std::lock_guard<std::mutex> lock(_mutex);
            _value = newValue;
        }

        this->OnChange();
    };

private:
    mutable std::mutex _mutex;
    T _value;
};

}}}}
