#pragma once

namespace Unicore { namespace Library { namespace Flow {

using CallbackPtr = std::shared_ptr<std::function<void()>>;


class IChangeable;

using ChangeablePtr = std::shared_ptr<IChangeable>;


template<typename T>
class ISource;

template<typename T>
using SourcePtr = std::shared_ptr<ISource<T>>;


template<typename T>
class IMutable;

template<typename T>
using MutablePtr = std::shared_ptr<IMutable<T>>;


template<typename T>
class IConnector;

template<typename T>
using ConnectorPtr = std::shared_ptr<IConnector<T>>;

}}}
