/*
 * Copyright (c) 2018 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef Q3D_LOGGER_H
#define Q3D_LOGGER_H

#include <iostream>

namespace Q3DLogger
{

#define Q3D_LOG_ERROR std::cerr << '[' << Q3DLogger::detail::baseName(__FILE__) << "::" << __LINE__ << "] "

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

}

#endif
