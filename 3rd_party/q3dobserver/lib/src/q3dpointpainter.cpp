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

#include "q3dpointpainter.h"

#include <QOpenGLShaderProgram>
#include "Q3DVertex"

void Q3DPointPainter::init()
{
    initializeOpenGLFunctions();
    _arrayBuffer.create();
}

bool Q3DPointPainter::empty() const noexcept
{
    return _vertices.empty();
}

void Q3DPointPainter::viewport(QVector3D& minPos, QVector3D& maxPos) const noexcept
{
    QVector3D min = minPos;
    QVector3D max = maxPos;

    for(const auto& vertex : _vertices)
    {
        const auto& pos = vertex.position;
        min[0] = std::min(min[0], pos[0]);
        min[1] = std::min(min[1], pos[1]);
        min[2] = std::min(min[2], pos[2]);
        max[0] = std::max(max[0], pos[0]);
        max[1] = std::max(max[1], pos[1]);
        max[2] = std::max(max[2], pos[2]);
    }

    minPos = min;
    maxPos = max;
}

void Q3DPointPainter::clear() noexcept
{
    _vertices.clear();
}

void Q3DPointPainter::reserve(int points)
{
    _vertices.reserve(points);
}

void Q3DPointPainter::add(const Q3DVertex& point)
{
    _vertices.append(Q3DProgramVertex{ point.position(), QVector3D(), point.color() });
    _upload = true;
}

void Q3DPointPainter::paint(int positionAttribute, int normalAttribute, int colorAttribute,
                            QOpenGLShaderProgram& program)
{
    if(_vertices.empty())
    {
        return;
    }

    _arrayBuffer.bind();

    if(_upload)
    {
        _arrayBuffer.allocate(_vertices.data(), _vertices.size() * int(sizeof(Q3DProgramVertex)));
        _upload = false;
    }

    program.enableAttributeArray(positionAttribute);
    program.setAttributeBuffer(positionAttribute, GL_FLOAT, 0, 3, sizeof(Q3DProgramVertex));

    program.enableAttributeArray(normalAttribute);
    program.setAttributeBuffer(normalAttribute, GL_FLOAT, sizeof(QVector3D), 3, sizeof(Q3DProgramVertex));

    program.enableAttributeArray(colorAttribute);
    program.setAttributeBuffer(colorAttribute, GL_FLOAT, sizeof(QVector3D) * 2, 4, sizeof(Q3DProgramVertex));

    glDrawArrays(GL_POINTS, 0, _arrayBuffer.size());
    _arrayBuffer.release();
}
