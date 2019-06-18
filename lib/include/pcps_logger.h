/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_LOGGER_H
#define PCPS_LOGGER_H

#include <iostream>

namespace pcps
{

///@cond INTERNAL

#define PCPS_LOG_ERROR std::cerr << '[' << pcps::detail::baseName(__FILE__) << "::" << __LINE__ << "] "

namespace detail
{
    template<unsigned int Size>
    constexpr const char* baseNameImpl(const char (&charArray)[Size], unsigned int index) noexcept
    {
        return index >= Size ?
                    charArray :
                    charArray[Size - index] == '/' ?
                        charArray + Size - index + 1 :
                        baseNameImpl(charArray, index + 1);
    }

    template<unsigned int Size>
    constexpr const char* baseName(const char (&charArray)[Size]) noexcept
    {
        return baseNameImpl(charArray, 2);
    }
}

///@endcond

}

#endif
