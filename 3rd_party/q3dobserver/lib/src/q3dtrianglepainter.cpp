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

#include "q3dtrianglepainter.h"

#include <QOpenGLShaderProgram>
#include "Q3DVertex"

void Q3DTrianglePainter::init()
{
    initializeOpenGLFunctions();
    _frontArrayBuffer.create();
    _backArrayBuffer.create();
}

bool Q3DTrianglePainter::empty() const noexcept
{
    return _frontVertices.empty();
}

void Q3DTrianglePainter::viewport(QVector3D& minPos, QVector3D& maxPos) const noexcept
{
    QVector3D min = minPos;
    QVector3D max = maxPos;

    for(const auto& vertex : _frontVertices)
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

void Q3DTrianglePainter::clear() noexcept
{
    _frontVertices.clear();
    _backVertices.clear();
}

void Q3DTrianglePainter::reserve(int triangles)
{
    _frontVertices.reserve(triangles * 2);
    _backVertices.reserve(triangles * 2);
}

void Q3DTrianglePainter::add(const Q3DVertex& vertex1, const Q3DVertex& vertex2, const Q3DVertex& vertex3)
{
    auto normal = QVector3D::normal(vertex1.position(), vertex2.position(), vertex3.position());
    _frontVertices.append(Q3DProgramVertex{ vertex1.position(), normal, vertex1.color() });
    _frontVertices.append(Q3DProgramVertex{ vertex2.position(), normal, vertex2.color() });
    _frontVertices.append(Q3DProgramVertex{ vertex3.position(), normal, vertex3.color() });
    _backVertices.append(Q3DProgramVertex{ vertex1.position(), -normal, vertex1.color() });
    _backVertices.append(Q3DProgramVertex{ vertex3.position(), -normal, vertex3.color() });
    _backVertices.append(Q3DProgramVertex{ vertex2.position(), -normal, vertex2.color() });
    _upload = true;
}

void Q3DTrianglePainter::paint(bool drawBackFaces, int positionAttribute, int normalAttribute, int colorAttribute,
                               QOpenGLShaderProgram& program)
{
    if(_frontVertices.empty())
    {
        return;
    }

    if(drawBackFaces)
    {
        _paint(false, positionAttribute, normalAttribute, colorAttribute, program);
    }

    _paint(true, positionAttribute, normalAttribute, colorAttribute, program);
    _upload = false;
}

void Q3DTrianglePainter::_paint(bool frontFaces, int positionAttribute, int normalAttribute, int colorAttribute,
                                QOpenGLShaderProgram& program)
{
    QOpenGLBuffer& arrayBuffer = frontFaces ? _frontArrayBuffer : _backArrayBuffer;

    arrayBuffer.bind();

    if(_upload)
    {
        if(frontFaces)
        {
            arrayBuffer.allocate(_frontVertices.data(), _frontVertices.size() * int(sizeof(Q3DProgramVertex)));
        }
        else
        {
            arrayBuffer.allocate(_backVertices.data(), _backVertices.size() * int(sizeof(Q3DProgramVertex)));
        }
    }

    program.enableAttributeArray(positionAttribute);
    program.setAttributeBuffer(positionAttribute, GL_FLOAT, 0, 3, sizeof(Q3DProgramVertex));

    program.enableAttributeArray(normalAttribute);
    program.setAttributeBuffer(normalAttribute, GL_FLOAT, sizeof(QVector3D), 3, sizeof(Q3DProgramVertex));

    program.enableAttributeArray(colorAttribute);
    program.setAttributeBuffer(colorAttribute, GL_FLOAT, sizeof(QVector3D) * 2, 4, sizeof(Q3DProgramVertex));

    glDrawArrays(GL_TRIANGLES, 0, arrayBuffer.size());
    arrayBuffer.release();
}
