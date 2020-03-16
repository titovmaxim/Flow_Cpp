#pragma once

#include <memory>

namespace Unicore { namespace Library {

class NonCopyable
{
public:
    NonCopyable() = default;

    virtual ~NonCopyable() = default;

    NonCopyable(const NonCopyable &) = delete;

    NonCopyable &operator=(const NonCopyable &) = delete;
};

}}
