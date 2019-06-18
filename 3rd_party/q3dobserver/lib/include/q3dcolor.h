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

#ifndef Q3D_COLOR_H
#define Q3D_COLOR_H

#include <QVector4D>

class Q3DColor
{

public:
    Q3DColor() noexcept;

    Q3DColor(float r, float g, float b, float a = 1) noexcept;

    Q3DColor(const QVector4D& vector) noexcept;

    Q3DColor(const QColor& color) noexcept;

    const QVector4D& vector() const noexcept;

    void setVector(const QVector4D& vector) noexcept;

    QColor color() const noexcept;

    void setColor(const QColor& color) noexcept;

    bool hasAlpha() const noexcept;

private:
    QVector4D _vector;
};

#endif
