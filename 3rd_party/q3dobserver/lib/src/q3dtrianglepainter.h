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

#ifndef Q3D_TRIANGLE_PAINTER_H
#define Q3D_TRIANGLE_PAINTER_H

#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include "q3dprogramvertex.h"

class Q3DVertex;
class QOpenGLShaderProgram;

class Q3DTrianglePainter : private QOpenGLFunctions
{

public:
    void init();

    bool empty() const noexcept;

    void viewport(QVector3D& minPos, QVector3D& maxPos) const noexcept;

    void clear() noexcept;

    void reserve(int triangles);

    void add(const Q3DVertex& vertex1, const Q3DVertex& vertex2, const Q3DVertex& vertex3);

    void paint(bool drawBackFaces, int positionAttribute, int normalAttribute, int colorAttribute,
               QOpenGLShaderProgram& program);

private:
    QVector<Q3DProgramVertex> _frontVertices;
    QVector<Q3DProgramVertex> _backVertices;
    QOpenGLBuffer _frontArrayBuffer;
    QOpenGLBuffer _backArrayBuffer;
    bool _upload = false;

    void _paint(bool frontFaces, int positionAttribute, int normalAttribute, int colorAttribute,
                QOpenGLShaderProgram& program);
};

#endif
