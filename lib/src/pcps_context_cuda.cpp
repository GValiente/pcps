/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_context.h"

#include "pcps_thrust_cached_allocator.h"

namespace pcps
{

namespace
{
    class DefaultContext : public Context
    {

    public:
        pcps_thrust::CachedAllocator defaultAllocator;

        DefaultContext() noexcept
        {
            allocator = &defaultAllocator;
        }
    };
}

std::unique_ptr<Context> Context::buildDefault()
{
    return std::unique_ptr<Context>(new DefaultContext());
}

Context::Context(pcps_thrust::CachedAllocator& _allocator) noexcept :
    allocator(&_allocator)
{
}

}
