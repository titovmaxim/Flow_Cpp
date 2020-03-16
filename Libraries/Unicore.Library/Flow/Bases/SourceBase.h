#pragma once

#include "../Interfaces.h"
#include "ChangeableBase.h"

namespace Unicore { namespace Library { namespace Flow {

template<typename T>
class SourceBase : public ChangeableBase, public virtual ISource<T>
{
};

}}}