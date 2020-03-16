#pragma once

#include <functional>
#include "../Interfaces.h"
#include "../Bases/SourceBase.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {

template<typename TOut, typename ...TIns>
class Func : public SourceBase<TOut>
{
public:
    Func(const SourcePtr <TIns> &... sources, const std::function<TOut(TIns...)> &func) :
        _func([=]() { return func(sources->Value()...); })
    {
        this->SubscribeTo(std::vector<ChangeablePtr>{sources...});
    };

    TOut Value() const override
    {
        return _func();
    }

private:
    const std::function<TOut()> _func;
};

}}}}
