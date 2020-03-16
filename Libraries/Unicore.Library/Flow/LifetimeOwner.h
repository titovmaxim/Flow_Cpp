#pragma once

#include <mutex>
#include <thread>
#include "Interfaces.h"

namespace Unicore { namespace Library { namespace Flow {

// The inheritors MUST call ReleaseOwned as a first instruction of the destructor!
class LifetimeOwner
{
public:

    void KeepAlive(std::shared_ptr<void> item)
    {
        {
            const std::lock_guard<std::mutex> lock(_mutex);
            _actions.emplace_back(item);
        }
    }

    virtual ~LifetimeOwner()
    {
#ifdef QT_DEBUG
#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable: 4297 )
#elif __linux__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wterminate"
#endif
        if (_actions.size() > 0)
            throw std::invalid_argument("ReleaseOwned is not called in destructor");
#ifdef _WIN32
#pragma warning( pop )
#elif __linux__
#pragma GCC diagnostic pop
#endif
#endif
    };

    // Assure that all weak references will efficiently die (for example are not used in OnChange notification loop)
    void ReleaseOwned()
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        while (_actions.size() > 0) {
            std::weak_ptr<void> weak = _actions.back();
            _actions.pop_back();

            while (!weak.expired())
                std::this_thread::yield();
        }
    }

private:
    std::mutex _mutex;
    std::vector<std::shared_ptr<void>> _actions;
};

}}}
