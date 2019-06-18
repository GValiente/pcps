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

#ifndef Q3D_CAMERA_H
#define Q3D_CAMERA_H

#include <QVector3D>
#include "q3dconstants.h"

class Q3DCamera
{

public:
    QVector3D translation = QVector3D(0, 0, -1);
    QVector3D rotation = QVector3D(0, 0, 0);
    QVector3D scale = QVector3D(1, 1, 1);
    QVector3D center = QVector3D(0, 0, 0);

    QMatrix4x4 modelView() const noexcept;

    void setupViewport(const QVector3D& minPos, const QVector3D& maxPos,
                       float margin = Q3DConstants::defaultCameraMargin) noexcept;
};

#endif
