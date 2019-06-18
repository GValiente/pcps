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

#ifndef Q3D_OBSERVER_H
#define Q3D_OBSERVER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "Q3DVertex"

class Q3DObserver : public QOpenGLWidget, private QOpenGLFunctions
{

public:

    explicit Q3DObserver(QWidget* parent = nullptr, Qt::WindowFlags f = 0);

    ~Q3DObserver() override;

    bool active() const noexcept;

    bool empty() const noexcept;

    float zNear() const noexcept;

    void setZNear(float zNear);

    float zFar() const noexcept;

    void setZFar(float zFar);

    float fov() const noexcept;

    void setFov(float fov);

    const QMatrix4x4& projection() noexcept;

    const QMatrix4x4& modelView() noexcept;

    void resetCamera();

    const QWidget* overlayWidget() const noexcept;

    QWidget* overlayWidget() noexcept;

    const QColor& backgroundColor() const noexcept;

    void setBackgroundColor(const QColor& backgroundColor);

    float lightWeight() const noexcept;

    void setLightWeight(float lightWeight);

    float pointSize() const noexcept;

    void setPointSize(float pointSize);

    void clearPoints();

    void reservePoints(int points);

    void addPoint(const QVector3D& position, const Q3DColor& color);

    void addPoint(const Q3DVertex& point);

    void clearLines();

    void reserveLines(int lines);

    void addLine(const QVector3D& from, const QVector3D& to, const Q3DColor& color);

    void addLine(const Q3DVertex& from, const Q3DVertex& to);

    bool drawBackFaces() const noexcept;

    void setDrawBackFaces(bool draw);

    void clearTriangles();

    void reserveTriangles(int triangles);

    void addTriangle(const QVector3D& vertex1, const QVector3D& vertex2, const QVector3D& vertex3, const Q3DColor& color);

    void addTriangle(const Q3DVertex& vertex1, const Q3DVertex& vertex2, const Q3DVertex& vertex3);

    void clearTexts();

    void reserveTexts(int texts);

    void addText(const QVector3D& position, const Q3DColor& color, QString text);

    void addText(const Q3DVertex& vertex, QString text);

    void clear();

    void reset();

protected:
    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

    void wheelEvent(QWheelEvent* event) override;

    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

private:
    class Impl;

    QScopedPointer<Impl> _impl;

    void _update();
};

#endif
