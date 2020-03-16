#pragma once

#include <memory>
#include <functional>
#include "Interfaces.h"
#include "Impls/Const.h"
#include "Impls/Var.h"
#include "Impls/Func.h"
#include "Impls/Property.h"
#include "Impls/Connector.h"
#include "Impls/Action.h"

namespace Unicore { namespace Library { namespace Flow {

template<typename T>
inline SourcePtr<T> Const(const T &initialValue)
{
    return std::make_shared<Impls::Const<T>>(initialValue);
}

template<typename T>
inline MutablePtr<T> Var(const T &initialValue)
{
    return std::make_shared<Impls::Var<T>>(initialValue);
}

template<typename T>
inline MutablePtr<T> Property(const std::function<T()> &get, const std::function<bool(T)> &set)
{
    return std::make_shared<Impls::Property<T>>(get, set);
}

template<typename T>
inline MutablePtr<T> Property(const std::function<T()> &get, const std::function<void(T)> &set)
{
    return std::make_shared<Impls::Property<T>>(get, [=](T value) {
        set(value);
        return true;
    });
}

template<typename T>
inline ConnectorPtr<T> Connector(const T &valueWhenDisconnected)
{
    return std::make_shared<Impls::Connector<T>>(valueWhenDisconnected);
}

template<typename T, typename ...TIns>
inline SourcePtr<T> Func(const SourcePtr<TIns> &... sources, const std::function<T(TIns...)> &func)
{
    return std::make_shared<Impls::Func<T, TIns...>>(sources..., func);
}

template<typename ...TIns>
inline std::function<void()> Action(LifetimeOwner *owner, const SourcePtr<TIns> &... sources, const std::function<void(TIns...)> &func)
{
    auto action = std::make_shared<Impls::Action<TIns...>>(sources..., func);
    owner->KeepAlive(action);
    return std::function<void()>([action]() { action->Exec(); });
}

template<typename ...TIns>
inline std::function<void()> ActionUI(QtHelper *owner, const SourcePtr<TIns> &... sources, const std::function<void(TIns...)> &func)
{
    std::function<void(TIns...)> funcUI([owner, func](TIns... values) {
        owner->RunOnUIThread([=]() {
            func(values...);
        });
    });

    return Flow::Action<TIns...>(owner, sources..., funcUI);
}

}}}
