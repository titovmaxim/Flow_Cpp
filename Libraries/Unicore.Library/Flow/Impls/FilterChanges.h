#pragma once

#include <mutex>
#include "../Bases/MutableBase.h"

namespace Unicore { namespace Library { namespace Flow { namespace Impls {

// Warning!!!
// Will fire source.Value() on OnChange thread, which can lead to a) concurrency and b) break some thread sensitive stuff, like OpenGL
template<typename T>
class FilterChanges : public Cache<T>
{
public:
    explicit FilterChanges(SourcePtr <T> source) :
        Cache<T>(source)
    {
    }

    void OnChange() const override
    {
        bool wasCached;
        T oldValue;
        {
            const std::lock_guard<std::mutex> lock(this->mutex);

            wasCached = this->isCached;
            oldValue = this->cachedValue;

            this->changeCount++;
            this->isCached = false;
        }

        // Update this->cachedValue
        this->Value();

        {
            const std::lock_guard<std::mutex> lock(this->mutex);

            if (wasCached && this->isCached && oldValue == this->cachedValue)
                return;
        }

        ChangeableBase::OnChange();
    }
};

}}}}
