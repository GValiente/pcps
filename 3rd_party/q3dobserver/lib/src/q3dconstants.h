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

#ifndef Q3D_CONSTANTS_H
#define Q3D_CONSTANTS_H

namespace Q3DConstants
{

constexpr float epsilon = 1e-6f;

constexpr float defaultZNear = 0.1f;
constexpr float defaultZFar = 100;
constexpr float defaultFov = 45;
constexpr float defaultLightWeight = 0.5;
constexpr float defaultPointSize = 5;
constexpr float defaultCameraMargin = 0.125;
constexpr bool defaultDrawBackFaces = true;

constexpr float translationFactor = 0.85f;
constexpr float scaleFactor = 0.999f;

}

#endif
