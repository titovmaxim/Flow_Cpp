#pragma once

#include "Interfaces.h"
#include "Impls/Var.h"
#include "Impls/Func.h"
#include "Impls/Convert.h"
#include "Impls/Cache.h"
#include "Impls/FilterChanges.h"
#include "Impls/Throttle.h"
#include "Qt/FlowQmlMap.h"
#include "Qt/QtHelper.h"

namespace Unicore { namespace Library { namespace Flow {

template<typename TIn>
template<typename TOut>
inline SourcePtr<TOut> ISource<TIn>::Func(const std::function<TOut(TIn)> &func)
{
    auto selfPtr = this->SharedPtr<ISource<TIn>>();
    return Flow::Func<TOut, TIn>(selfPtr, func);
}

template<typename TIn>
template<typename TOut>
inline MutablePtr<TOut> IMutable<TIn>::Convert(const std::function<TOut(TIn)> &forwardFunc, const std::function<TIn(TOut)> &backwardFunc)
{
    auto selfPtr = this->template SharedPtr<IMutable<TIn>>();
    return std::make_shared<Impls::Convert<TIn, TOut>>(selfPtr, forwardFunc, backwardFunc);
}

template<typename T>
inline std::function<void()> ISource<T>::Action(LifetimeOwner *owner, const std::function<void(T)> &func)
{
    auto selfPtr = this->SharedPtr<ISource<T>>();
    return Flow::Action<T>(owner, selfPtr, func);
}

template<typename T>
inline std::function<void()> ISource<T>::ActionUI(QtHelper *owner, const std::function<void(T)> &func)
{
    auto selfPtr = this->SharedPtr<ISource<T>>();
    return Flow::ActionUI<T>(owner, selfPtr, func);
}

template<typename T>
inline SourcePtr<T> ISource<T>::Cache()
{
    auto selfPtr = this->SharedPtr<ISource<T>>();
    return std::make_shared<Impls::Cache<T>>(selfPtr);
}

template<typename T>
inline SourcePtr<T> ISource<T>::FilterChanges()
{
    auto selfPtr = this->SharedPtr<ISource<T>>();
    return std::make_shared<Impls::FilterChanges<T>>(selfPtr);
}

template<typename T>
inline SourcePtr<T> ISource<T>::Throttle(double interval)
{
    auto selfPtr = this->SharedPtr<ISource<T>>();
    return std::make_shared<Impls::Throttle<T>>(interval, selfPtr);
}

template<typename T>
QQmlPropertyMap *ISource<T>::QmlMap(LifetimeOwner *owner)
{
    auto selfPtr = this->SharedPtr<ISource<T>>();
    static const auto metadata = T::template Metadata<T>();
    auto ptr = std::make_shared<Impls::FlowQmlMap<T>>(metadata, selfPtr);
    owner->KeepAlive(ptr);
    return ptr.get();
}

template<typename T>
QQmlPropertyMap *ISource<T>::QmlValue(LifetimeOwner *owner)
{
    auto selfPtr = this->SharedPtr<ISource<T>>();
    static const std::vector<Flow::MetadataItem<T>> metadata = {
        MetadataItem<T>("Value", [](const auto &data) { return data; }, [](auto &data, const QVariant &value) { data = value.value<T>(); })
    };
    auto ptr = std::make_shared<Impls::FlowQmlMap<T>>
        (metadata, selfPtr);
    owner->KeepAlive(ptr);
    return ptr.get();
}

}}}
