#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <QQmlPropertyMap>
#include "../Utils.h"
#include "Types.h"
#include "LifetimeOwner.h"
#include "Qt/QtHelper.h"

namespace Unicore { namespace Library { namespace Flow {

inline CallbackPtr Callback(const std::function<void()> &func)
{
    return std::make_shared<std::function<void()>>(func);
}


class IChangeable : public NonCopyable, public std::enable_shared_from_this<IChangeable>
{
public:
    virtual void AddSubscription(CallbackPtr callback) = 0;

    virtual void SubscribeTo(const ChangeablePtr &source) const = 0;

    virtual void SubscribeTo(const std::vector<ChangeablePtr> &sources) const = 0;

    virtual void OnChange() const = 0;

protected:
    const CallbackPtr OnChangeCallback = Callback([this]() {
        OnChange();
    });

    template<class TSelf>
    std::shared_ptr<TSelf> SharedPtr()
    {
        return std::dynamic_pointer_cast<TSelf>(shared_from_this());
    }
};


template<typename T>
class ISource : public virtual IChangeable
{
public:
    virtual T Value() const = 0;

    // Chaining

    template<typename TOut>
    SourcePtr<TOut> Func(const std::function<TOut(T)> &func);

    std::function<void()> Action(LifetimeOwner *owner, const std::function<void(T)> &func);

    std::function<void()> ActionUI(QtHelper *owner, const std::function<void(T)> &func);

    SourcePtr<T> Cache();

    SourcePtr<T> FilterChanges();

    SourcePtr<T> Throttle(double interval);

    QQmlPropertyMap *QmlMap(LifetimeOwner *owner);

    QQmlPropertyMap *QmlValue(LifetimeOwner *owner);
};


template<typename T>
class IMutable : public virtual ISource<T>
{
public:
    virtual void SetValue(const T &newValue) = 0;

    // Chaining

    template<typename TOut>
    MutablePtr<TOut> Convert(const std::function<TOut(T)> &forwardFunc, const std::function<T(TOut)> &backwardFunc);
};


template<typename T>
class IConnector : public virtual ISource<T>
{
public:
    virtual bool IsConnected() const = 0;

    virtual void Disconnect() = 0;

    virtual void ConnectTo(SourcePtr<T> source) = 0;
};


// A __KEY__ represent a name if a field, or, in the case of property, name of xxx __KEY() and Set__KEY__(xxx) methods.
// Possible __TYPE__ values are to what QVariant van be casted through QVariant.value<__TYPE__>
#define FLOW_QML(...) template<typename T> static std::vector<Unicore::Library::Flow::MetadataItem<T>> Metadata() { static const std::vector<Unicore::Library::Flow::MetadataItem<T>> metadata = { __VA_ARGS__ }; return metadata; }
#define FLOW_QML_FIELD(__KEY__, __TYPE__) Unicore::Library::Flow::MetadataItem<T>(#__KEY__, [](const auto &data) { return data.__KEY__; }, [](auto &data, const QVariant &value) { data.__KEY__ = value.value<__TYPE__>(); })
#define FLOW_QML_PROPERTY(__KEY__, __TYPE__) Unicore::Library::Flow::MetadataItem<T>(#__KEY__, [](const auto &data) { return data.__KEY__(); }, [](auto &data, const QVariant &value) { data.Set##__KEY__(value.value<__TYPE__>()); })


template<typename T>
struct MetadataItem
{
    const std::basic_string<char> Key;
    const QString KeyQ;
    const std::function<QVariant(const T &)> Get;
    const std::function<void(T &, const QVariant &)> Set;

    MetadataItem(const char *key, const std::function<QVariant(const T &)> &get, const std::function<void(T &, const QVariant &)> &set) :
        Key(key),
        KeyQ(key),
        Get(get),
        Set(set)
    {
    }
};

}}}
