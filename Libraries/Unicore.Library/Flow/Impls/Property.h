#pragma once

#include "../Bases/MutableBase.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {

template<typename T>
class Property : public MutableBase<T>
{
public:
    Property(const std::function<T()> &get, const std::function<bool(T)> &set)
        : _get(get),
          _set(set)
    {
    };

    T Value() const override
    {
        return _get();
    }

    void SetValue(const T &newValue) override
    {
        if (_set(newValue))
            this->OnChange();
    };

private:
    const std::function<T()> _get;
    const std::function<bool(T)> _set;
};

}}}}
