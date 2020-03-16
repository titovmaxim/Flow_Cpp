#pragma once

#include <memory>
#include <vector>
#include <QQmlPropertyMap>
#include "../Interfaces.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {

class FlowQmlMapBase : public QQmlPropertyMap
{
Q_OBJECT

protected:
    const Flow::CallbackPtr EmitOnFlowChange = Flow::Callback([this]() { emit OnFlowChange(); });

    virtual Q_INVOKABLE void OnFlowChange() = 0;
};


template<typename T>
class FlowQmlMap : public FlowQmlMapBase, public Flow::LifetimeOwner
{
public:
    explicit FlowQmlMap(std::vector<Unicore::Library::Flow::MetadataItem<T>> metadata, Flow::SourcePtr<T> source) :
        _metadata(metadata),
        _source(source),
        _mutable(std::dynamic_pointer_cast<Flow::IMutable<T>>(source))
    {
        InitializeProperties();

        _source->AddSubscription(EmitOnFlowChange);
        QObject::connect(this, &FlowQmlMap::valueChanged, this, &FlowQmlMap::OnQmlChange);
    }

    ~FlowQmlMap() override
    {
        this->ReleaseOwned();
    };

private:
    const std::vector<Flow::MetadataItem<T>> _metadata;
    const Flow::SourcePtr<T> _source;
    const Flow::MutablePtr<T> _mutable;
    std::recursive_mutex _mutex;

    void InitializeProperties()
    {
        auto data = _source->Value();
        for (auto m : _metadata)
            this->insert(m.KeyQ, m.Get(data));
    }

    void OnFlowChange() override
    {
        const std::lock_guard<std::recursive_mutex> lock(_mutex);

        auto data = _source->Value();

        for (auto m : _metadata) {
            auto key = m.Key.c_str();

            auto value = m.Get(data);
            auto oldValue = this->property(key);

            if (oldValue != value)
                this->setProperty(key, value);
        }
    }

    void OnQmlChange(const QString &key, const QVariant &value)
    {
        // Hold the value disregarding the wish of Qml to change it on immutable source
        if (_mutable == nullptr) {
            OnFlowChange();
            return;
        }

        const std::lock_guard<std::recursive_mutex> lock(_mutex);

        for (auto m : _metadata) {
            if (key != m.KeyQ)
                continue;

            auto data = _source->Value();
            auto oldValue = m.Get(data);

            if (oldValue != value) {
                m.Set(data, value);
                _mutable->SetValue(data);
            }

            return;
        }

#ifdef QT_DEBUG
        auto keyName = key.toStdString();
        throw std::invalid_argument("Key not found in FlowQmlMap: " + keyName);
#endif
    }
};

}}}}
