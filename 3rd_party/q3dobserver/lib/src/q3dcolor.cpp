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

#include "q3dcolor.h"

#include <cmath>
#include <QColor>
#include "q3dconstants.h"

namespace
{
    QVector4D vectorFromColor(const QColor& color) noexcept
    {
        return { float(color.redF()), float(color.greenF()), float(color.blueF()), float(color.alphaF()) };
    }
}

Q3DColor::Q3DColor() noexcept :
    _vector(0, 0, 0, 1)
{
}

Q3DColor::Q3DColor(float r, float g, float b, float a) noexcept :
    _vector(r, g, b, a)
{
}

Q3DColor::Q3DColor(const QVector4D& vector) noexcept :
    _vector(vector)
{
}

Q3DColor::Q3DColor(const QColor& color) noexcept :
    _vector(vectorFromColor(color))
{
}

const QVector4D& Q3DColor::vector() const noexcept
{
    return _vector;
}

void Q3DColor::setVector(const QVector4D& vector) noexcept
{
    _vector = vector;
}

QColor Q3DColor::color() const noexcept
{
    return QColor::fromRgbF(double(_vector[0]), double(_vector[1]), double(_vector[2]), double(_vector[3]));
}

void Q3DColor::setColor(const QColor& color) noexcept
{
    _vector = vectorFromColor(color);
}

bool Q3DColor::hasAlpha() const noexcept
{
    return std::abs(_vector[3] - 1) > Q3DConstants::epsilon;
}
