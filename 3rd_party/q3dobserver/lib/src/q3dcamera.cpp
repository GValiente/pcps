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

#include "q3dcamera.h"

#include <cmath>
#include <QMatrix4x4>

QMatrix4x4 Q3DCamera::modelView() const noexcept
{
    QMatrix4x4 matrix;
    matrix.translate(translation);
    matrix.scale(scale);
    matrix.rotate(rotation.x(), 1, 0, 0);
    matrix.rotate(rotation.y(), 0, 1, 0);
    matrix.rotate(rotation.z(), 0, 0, 1);
    matrix.translate(-center);
    return matrix;
}

void Q3DCamera::setupViewport(const QVector3D& minPos, const QVector3D& maxPos, float margin) noexcept
{
    center = (minPos + maxPos) * 0.5;

    QVector3D posDelta = maxPos - minPos;
    float norm = std::sqrt((posDelta.x() * posDelta.x()) + (posDelta.y() * posDelta.y()) + (posDelta.z() * posDelta.z()));
    float newScale;

    if(norm > Q3DConstants::epsilon)
    {
        newScale = 1 / norm;
        newScale *= 1 - margin;
    }
    else
    {
        newScale = 1 - margin;
    }

    scale = QVector3D(newScale, newScale, newScale);
}
