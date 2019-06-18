/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_THRUST_CACHED_ALLOCATOR_H
#define PCPS_THRUST_CACHED_ALLOCATOR_H

#include <vector>

namespace pcps_thrust
{

class CachedAllocator
{

public:
    using value_type = char;

    ~CachedAllocator() noexcept
    {
        clear();
    }

    value_type* allocate(std::ptrdiff_t numBytes);

    void deallocate(value_type* pointer, std::size_t) noexcept;

    void clear() noexcept;

protected:
    std::vector<value_type*> _freeBlocks;
};

}

#endif
