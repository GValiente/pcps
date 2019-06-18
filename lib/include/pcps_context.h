/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_CONTEXT_H
#define PCPS_CONTEXT_H

#include <memory>

#ifdef PCPS_OPENCL
    namespace boost
    {
        namespace compute
        {
            class device;
            class context;
            class command_queue;
        }
    }
#endif

#ifdef PCPS_CUDA
    namespace pcps_thrust
    {
        class CachedAllocator;
    }
#endif

namespace pcps
{

/**
 * @brief Manages a set of resources including memory buffers and program objects.
 */
class Context
{

public:
    /**
     * @brief Retrieves a default context.
     */
    static std::unique_ptr<Context> buildDefault();

    #ifdef PCPS_OPENCL
        /**
         * @brief Retrieves a default context with the given device.
         * @param device boost::compute device.
         * @return A default context with the given device.
         */
        static std::unique_ptr<Context> buildDefault(const boost::compute::device& device);

        const boost::compute::context* context = nullptr; /**< boost::compute context. */
        boost::compute::command_queue* queue = nullptr; /**< boost::compute queue. */

        /**
         * @brief Class constructor.
         * @param context boost::compute context.
         * @param queue boost::compute queue.
         */
        Context(const boost::compute::context& context, boost::compute::command_queue& queue) noexcept;
    #endif

    #ifdef PCPS_CUDA
        pcps_thrust::CachedAllocator* allocator = nullptr; /**< pcps_thrust cached allocator. */

        /**
         * @brief Class constructor.
         * @param allocator pcps_thrust cached allocator.
         */
        explicit Context(pcps_thrust::CachedAllocator& allocator) noexcept;
    #endif

    /**
     * @brief Copy construction is not allowed.
     */
    Context(const Context& other) = delete;

    /**
     * @brief Copy assignment is not allowed.
     */
    Context& operator=(const Context& other) = delete;

    /**
     * @brief Move construction is not allowed.
     */
    Context(Context&& other) = delete;

    /**
     * @brief Move assignment is not allowed.
     */
    Context& operator=(Context&& other) = delete;

    /**
     * @brief Class destructor.
     */
    virtual ~Context() = default;

protected:
    ///@cond INTERNAL

    Context() = default;

    ///@endcond
};

}

#endif
