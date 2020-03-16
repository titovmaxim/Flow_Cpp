#pragma once

#include <atomic>
#include <vector>
#include <mutex>
#include <functional>
#include "../Interfaces.h"

namespace Unicore { namespace Library { namespace Flow {

class ChangeableBase : public virtual IChangeable
{
private:
    static const auto WeakCleanupFrequency = 32;

public:
    void AddSubscription(CallbackPtr callback) override
    {
        bool shouldCleanup;
        {
            const std::lock_guard<std::recursive_mutex> lock(_recursiveMutex);
            _weakSubscriptions.emplace_back(callback); // Store a week_ptr
            shouldCleanup = ++_weakCleanupCount >= WeakCleanupFrequency;
        }

        if (shouldCleanup)
            TryCleanup();
    }

    void SubscribeTo(const ChangeablePtr &source) const override
    {
        source->AddSubscription(OnChangeCallback);
    }

    void SubscribeTo(const std::vector<ChangeablePtr> &sources) const override
    {
        for (const auto &source : sources)
            source->AddSubscription(OnChangeCallback);
    }

    void OnChange() const override
    {
        NotifySubscribers();
    }

private:
    mutable std::atomic<int> _weakCleanupCount{0};
    mutable std::recursive_mutex _recursiveMutex;
    mutable std::vector<std::weak_ptr<std::function<void()>>> _weakSubscriptions;

    void NotifySubscribers() const
    {
        auto shouldCleanup = false;
        {
            const std::lock_guard<std::recursive_mutex> lock(_recursiveMutex);
            // N can be changed inside loop if someone calls AddSubscription from OnChange event
            auto N = _weakSubscriptions.size();
            for (auto i = 0; i < N; i++) {
                if (auto callback = _weakSubscriptions[i].lock())
                    (*callback)();
                else
                    shouldCleanup = true;
            }
        }

        if (shouldCleanup)
            TryCleanup();
    }

    void TryCleanup() const
    {
        const std::unique_lock<std::recursive_mutex> lock(_recursiveMutex, std::try_to_lock);
        if (!lock.owns_lock())
            return;

        _weakCleanupCount = 0;

        const auto originalN = _weakSubscriptions.size();
        auto N = originalN;
        auto i = 0U;

        while (i < N) {
            // Dead WeakRef
            if (_weakSubscriptions[i].expired()) {
                N--;
                if (i != N)
                    _weakSubscriptions[i] = _weakSubscriptions[N];
            } else {
                i++;
            }
        }

        if (N < originalN)
            _weakSubscriptions.resize(N);
    }
};

}}}
