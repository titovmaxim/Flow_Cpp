#pragma once

#include "../Interfaces.h"
#include "SourceBase.h"

namespace Unicore { namespace Library { namespace Flow {

template<typename T>
class MutableBase : public SourceBase<T>, public virtual IMutable<T>
{
};

}}}