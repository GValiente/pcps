/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_thrust_cached_allocator.h"

#include <iostream>
#include "thrust/system/cuda/memory.h"

namespace pcps_thrust
{

namespace
{
    constexpr int Size = 8192;
}

CachedAllocator::value_type* CachedAllocator::allocate(std::ptrdiff_t numBytes)
{
    if(std::size_t(numBytes) > Size)
    {
        std::cerr << "CachedAllocator::allocate(): invalid numBytes: " << numBytes << std::endl;
        return nullptr;
    }

    std::size_t numFreeBlocks = _freeBlocks.size();
    value_type* result = nullptr;

    if(numFreeBlocks > 0)
    {
        result = _freeBlocks[numFreeBlocks - 1];
        _freeBlocks.pop_back();
    }
    else
    {
        result = thrust::cuda::malloc<value_type>(Size).get();

        if(! result)
        {
            std::cerr << "CachedAllocator::allocate(): thrust::cuda::malloc call failed" << std::endl;
            return nullptr;
        }
    }

    return result;
}

void CachedAllocator::deallocate(value_type* pointer, std::size_t) noexcept
{
    _freeBlocks.push_back(pointer);
}

void CachedAllocator::clear() noexcept
{
    for(value_type* pointer : _freeBlocks)
    {
        thrust::cuda::free(thrust::cuda::pointer<value_type>(pointer));
    }

    _freeBlocks.clear();
}

}
