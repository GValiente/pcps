/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_BOOST_COMPUTE_H
#define PCPS_BOOST_COMPUTE_H

#if defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"
    #pragma GCC diagnostic ignored "-Wfloat-equal"
    #pragma GCC diagnostic ignored "-Wctor-dtor-privacy"
    #pragma GCC diagnostic ignored "-Wignored-attributes"
    #pragma GCC diagnostic ignored "-Wmissing-declarations"
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#define BOOST_COMPUTE_THREAD_SAFE
#define BOOST_COMPUTE_HAVE_THREAD_LOCAL

#include "boost/compute/command_queue.hpp"
#include "boost/compute/container/mapped_view.hpp"
#include "boost/compute/iterator/counting_iterator.hpp"
#include "boost/compute/algorithm/transform.hpp"
#include "boost/compute/algorithm/transform_reduce.hpp"

// Allow closure pointers:
// https://groups.google.com/forum/#!topic/boost-compute/5D2A_vJ83B8
// https://stackoverflow.com/questions/54636860/boostcompute-passing-pointer-to-a-closure

namespace boost
{
    namespace compute
    {
        template<>
        inline const char* type_name<mapped_view<int>>()
        {
            return "__global int *";
        }

        template<>
        inline const char* type_name<mapped_view<float>>()
        {
            return "__global float *";
        }

        template<>
        inline const char* type_name<mapped_view<float2_>>()
        {
            return "__global float2 *";
        }

        template<>
        inline const char* type_name<mapped_view<float4_>>()
        {
            return "__global float4 *";
        }

        namespace detail
        {
            inline meta_kernel& operator<<(meta_kernel& kernel, const mapped_view<int>& mappedView)
            {
                kernel << kernel.var<int*>(kernel.get_buffer_identifier<int>(mappedView.get_buffer()));
                return kernel;
            }

            inline meta_kernel& operator<<(meta_kernel& kernel, const mapped_view<float>& mappedView)
            {
                kernel << kernel.var<float*>(kernel.get_buffer_identifier<float>(mappedView.get_buffer()));
                return kernel;
            }

            inline meta_kernel& operator<<(meta_kernel& kernel, const mapped_view<float2_>& mappedView)
            {
                kernel << kernel.var<float2_*>(kernel.get_buffer_identifier<float2_>(mappedView.get_buffer()));
                return kernel;
            }

            inline meta_kernel& operator<<(meta_kernel& kernel, const mapped_view<float4_>& mappedView)
            {
                kernel << kernel.var<float4_*>(kernel.get_buffer_identifier<float4_>(mappedView.get_buffer()));
                return kernel;
            }
        }
    }
}

#if defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic pop
#endif

#endif
