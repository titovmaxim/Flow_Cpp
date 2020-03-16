#pragma once

#include "../Bases/MutableBase.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {

template<typename T>
class Const : public SourceBase<T>
{
public:
    explicit Const(const T &initialValue)
        : _value(initialValue)
    {
    };

    T Value() const override
    {
        return _value;
    }

private:
    T _value;
};

}}}}
