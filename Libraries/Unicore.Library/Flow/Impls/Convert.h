#pragma once

#include "../Bases/MutableBase.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {

template<typename TIn, typename TOut>
class Convert : public MutableBase<TOut>
{
public:
    Convert(const MutablePtr <TIn> &mutable_, const std::function<TOut(TIn)> &forwardFunc, const std::function<TIn(TOut)> &backwardFunc) :
        _mutable(mutable_),
        _forwardFunc(forwardFunc),
        _backwardFunc(backwardFunc)
    {
        this->SubscribeTo(_mutable);
    };

    TOut Value() const override
    {
        return _forwardFunc(_mutable->Value());
    }

    void SetValue(const TOut &newValue) override
    {
        auto converted = _backwardFunc(newValue);
        _mutable->SetValue(converted);
    };

private:
    const MutablePtr <TIn> _mutable;
    const std::function<TOut(TIn)> _forwardFunc;
    const std::function<TIn(TOut)> _backwardFunc;
};

}}}}
