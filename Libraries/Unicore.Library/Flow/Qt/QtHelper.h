#pragma once

#include <functional>

namespace Unicore { namespace Library { namespace Flow {

class QtHelper : public QObject, public LifetimeOwner
{
Q_OBJECT

public:
    inline void RunOnUIThread(const std::function<void()> &action)
    {
        emit _runOnUiThread(action);
    }

private:
    Q_INVOKABLE void _runOnUiThread(const std::function<void()> &action)
    {
        action();
    }
};

}}}
