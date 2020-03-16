#pragma once

#include <mutex>
#include "../Bases/MutableBase.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {

template<typename T>
class Cache : public SourceBase<T>
{
public:
    explicit Cache(const SourcePtr <T> &source) :
        _source(source)
    {
        this->SubscribeTo(_source);
    }

    void OnChange() const override
    {
        {
            const std::lock_guard<std::mutex> lock(mutex);
            changeCount++;
            isCached = false;
            // Do not "zero" cachedValue here, because its destructor is better to be called from .Value thread
        }

        ChangeableBase::OnChange();
    }

    T Value() const override
    {
        int lastChangeCount;

        {
            const std::lock_guard<std::mutex> lock(mutex);

            if (isCached)
                return cachedValue;

            lastChangeCount = changeCount;
        }

        // Warning!!!
        // In case of multi-thread use of Cache->Value it is possible that _source->Value will be fired
        // several times per one OnChange iа the requests will go faster than the result will actually be evaluated
        auto value = _source->Value();

        {
            const std::lock_guard<std::mutex> lock(mutex);

            // If no OnChange events during _source->Value()
            if (lastChangeCount == changeCount) {
                // cachedValue destructor will be called here, in a proper thread
                cachedValue = value;
                isCached = true;
            }

            return value;
        }
    }

protected:
    mutable int changeCount = 0;
    mutable std::mutex mutex;
    mutable T cachedValue;
    mutable bool isCached = false;

private:
    const SourcePtr <T> _source;
};

}}}}
