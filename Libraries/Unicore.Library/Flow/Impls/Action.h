#pragma once

#include <functional>
#include "../Interfaces.h"
#include "../Bases/SourceBase.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {

template<typename ...TIns>
class Action : public virtual NonCopyable
{
public:
    Action(const SourcePtr <TIns> &... sources, const std::function<void(TIns...)> &func) :
        _action([=]() { func(sources->Value()...); })
    {
        for (const auto &source : std::vector<ChangeablePtr>{sources...})
            source->AddSubscription(OnChangeCallback);
    };

    inline void Exec() const
    {
        _action();
    }

private:
    const std::function<void()> _action;

    const CallbackPtr OnChangeCallback = Callback([this]() {
        _action();
    });
};

}}}}
