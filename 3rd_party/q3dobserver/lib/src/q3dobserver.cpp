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

#include "q3dobserver.h"

#include <cmath>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include "q3dcamera.h"
#include "q3dpointpainter.h"
#include "q3dlinepainter.h"
#include "q3dtrianglepainter.h"
#include "q3dtextwidget.h"
#include "q3dlogger.h"

class Q3DObserver::Impl
{

public:
    QOpenGLShaderProgram program;
    QScopedPointer<Q3DPointPainter> opaquePointPainter;
    QScopedPointer<Q3DPointPainter> transparentPointPainter;
    QScopedPointer<Q3DLinePainter> opaqueLinePainter;
    QScopedPointer<Q3DLinePainter> transparentLinePainter;
    QScopedPointer<Q3DTrianglePainter> opaqueTrianglePainter;
    QScopedPointer<Q3DTrianglePainter> transparentTrianglePainter;
    Q3DTextWidget* textWidget = nullptr;
    QMatrix4x4 projection;
    QMatrix4x4 modelView;
    Qt::MouseButton lastMouseButton = Qt::NoButton;
    QVector2D lastMousePos;
    Q3DCamera camera;
    QColor backgroundColor;
    float zNear = Q3DConstants::defaultZNear;
    float zFar = Q3DConstants::defaultZFar;
    float fov = Q3DConstants::defaultFov;
    float lightWeight = Q3DConstants::defaultLightWeight;
    float pointSize = Q3DConstants::defaultPointSize;
    float aspectRatio = 1;
    bool drawBackFaces = Q3DConstants::defaultDrawBackFaces;
    bool setupProjection = true;
    bool setupViewport = true;
    bool setupModelView = true;
    bool setupBackgroundColor = true;
    bool active = false;

    Impl()
    {
        opaquePointPainter.reset(new Q3DPointPainter());
        transparentPointPainter.reset(new Q3DPointPainter());
        opaqueLinePainter.reset(new Q3DLinePainter());
        transparentLinePainter.reset(new Q3DLinePainter());
        opaqueTrianglePainter.reset(new Q3DTrianglePainter());
        transparentTrianglePainter.reset(new Q3DTrianglePainter());
    }

    bool init()
    {
        constexpr char vertexShader[] = R"glsl(
                uniform mat4 u_mvpMatrix;
                uniform float u_pointSize;

                attribute vec3 a_position;
                attribute vec3 a_normal;
                attribute vec4 a_color;

                varying vec3 v_position;
                varying vec3 v_normal;
                varying vec4 v_color;

                void main()
                {
                    gl_Position = u_mvpMatrix * vec4(a_position, 1);
                    gl_PointSize = u_pointSize;

                    v_position = a_position;
                    v_normal = a_normal;
                    v_color = a_color;
                }
                )glsl";

        constexpr char fragmentShader[] = R"glsl(
                uniform vec3 u_lightPosition;
                uniform float u_lightWeight;

                varying vec3 v_position;
                varying vec3 v_normal;
                varying vec4 v_color;

                void main()
                {
                    if(v_normal == vec3(0, 0, 0))
                    {
                        gl_FragColor = v_color;
                    }
                    else
                    {
                        vec3 lightDirection = normalize(u_lightPosition - v_position);
                        float diffuseValue = max(dot(v_normal, lightDirection), 0.0);
                        vec4 diffuseColor = vec4(diffuseValue, diffuseValue, diffuseValue, 1);
                        gl_FragColor = (diffuseColor * u_lightWeight) + (v_color * (1.0 - u_lightWeight));
                    }
                }
                )glsl";

        if(! program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
        {
            Q3D_LOG_ERROR << "Vertex shader build failed: " << program.log().toStdString();
            return false;
        }

        if(! program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
        {
            Q3D_LOG_ERROR << "Fragment shader build failed: " << program.log().toStdString();
            return false;
        }

        program.bindAttributeLocation("a_position", 0);
        program.bindAttributeLocation("a_normal", 2);
        program.bindAttributeLocation("a_color", 4);

        if(! program.link())
        {
            Q3D_LOG_ERROR << "Shader program link failed: " << program.log().toStdString();
            return false;
        }

        if(! program.bind())
        {
            Q3D_LOG_ERROR << "Shader program bind failed: " << program.log().toStdString();
            return false;
        }

        opaquePointPainter->init();
        transparentPointPainter->init();
        opaqueLinePainter->init();
        transparentLinePainter->init();
        opaqueTrianglePainter->init();
        transparentTrianglePainter->init();
        active = true;
        return true;
    }
};

Q3DObserver::Q3DObserver(QWidget* parent, Qt::WindowFlags f) :
    QOpenGLWidget(parent, f),
    _impl(new Impl())
{
    _impl->textWidget = new Q3DTextWidget(*this);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_impl->textWidget);
    setMouseTracking(true);
}

Q3DObserver::~Q3DObserver()
{
    makeCurrent();
    _impl.reset();
    doneCurrent();
}

bool Q3DObserver::active() const noexcept
{
    return _impl->active;
}

bool Q3DObserver::empty() const noexcept
{
    return _impl->opaquePointPainter->empty() && _impl->transparentPointPainter->empty() &&
            _impl->opaqueLinePainter->empty() && _impl->transparentLinePainter->empty() &&
            _impl->opaqueTrianglePainter->empty() && _impl->transparentTrianglePainter->empty() &&
            _impl->textWidget->empty();
}

float Q3DObserver::zNear() const noexcept
{
    return _impl->zNear;
}

void Q3DObserver::setZNear(float zNear)
{
    _impl->zNear = zNear;
    _impl->setupProjection = true;
    _update();
}

float Q3DObserver::zFar() const noexcept
{
    return _impl->zFar;
}

void Q3DObserver::setZFar(float zFar)
{
    _impl->zFar = zFar;
    _impl->setupProjection = true;
    _update();
}

float Q3DObserver::fov() const noexcept
{
    return _impl->fov;
}

void Q3DObserver::setFov(float fov)
{
    _impl->fov = fov;
    _impl->setupProjection = true;
    _update();
}

const QMatrix4x4& Q3DObserver::projection() noexcept
{
    if(_impl->setupProjection)
    {
        _impl->projection.setToIdentity();
        _impl->projection.perspective(_impl->fov, _impl->aspectRatio, _impl->zNear, _impl->zFar);
        _impl->setupProjection = false;
    }

    return _impl->projection;
}

const QMatrix4x4& Q3DObserver::modelView() noexcept
{
    if(_impl->setupViewport)
    {
        float min = -std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::max();
        QVector3D minPos(max, max, max);
        QVector3D maxPos(min, min, min);
        _impl->opaquePointPainter->viewport(minPos, maxPos);
        _impl->transparentPointPainter->viewport(minPos, maxPos);
        _impl->opaqueLinePainter->viewport(minPos, maxPos);
        _impl->transparentLinePainter->viewport(minPos, maxPos);
        _impl->opaqueTrianglePainter->viewport(minPos, maxPos);
        _impl->transparentTrianglePainter->viewport(minPos, maxPos);
        _impl->textWidget->viewport(minPos, maxPos);
        _impl->camera.setupViewport(minPos, maxPos);
        _impl->setupModelView = true;
        _impl->setupViewport = false;
    }

    if(_impl->setupModelView)
    {
        _impl->modelView = _impl->camera.modelView();
        _impl->setupModelView = false;
    }

    return _impl->modelView;
}

void Q3DObserver::resetCamera()
{
    _impl->camera = Q3DCamera();
    _impl->setupViewport = true;
    _update();
}

const QWidget* Q3DObserver::overlayWidget() const noexcept
{
    return _impl->textWidget;
}

QWidget* Q3DObserver::overlayWidget() noexcept
{
    return _impl->textWidget;
}

const QColor& Q3DObserver::backgroundColor() const noexcept
{
    return _impl->backgroundColor;
}

void Q3DObserver::setBackgroundColor(const QColor& backgroundColor)
{
    _impl->backgroundColor = backgroundColor;
    _impl->setupBackgroundColor = true;
    _update();
}

float Q3DObserver::lightWeight() const noexcept
{
    return _impl->lightWeight;
}

void Q3DObserver::setLightWeight(float lightWeight)
{
    _impl->lightWeight = lightWeight;
    _update();
}

float Q3DObserver::pointSize() const noexcept
{
    return _impl->pointSize;
}

void Q3DObserver::setPointSize(float pointSize)
{
    _impl->pointSize = pointSize;
    _update();
}

void Q3DObserver::clearPoints()
{
    _impl->opaquePointPainter->clear();
    _impl->transparentPointPainter->clear();
    _update();
}

void Q3DObserver::reservePoints(int points)
{
    _impl->opaquePointPainter->reserve(points);
    _impl->transparentPointPainter->reserve(points);
}

void Q3DObserver::addPoint(const QVector3D& position, const Q3DColor& color)
{
    addPoint(Q3DVertex(position, color));
}

void Q3DObserver::addPoint(const Q3DVertex& point)
{
    if(point.color().hasAlpha())
    {
        _impl->transparentPointPainter->add(point);
    }
    else
    {
        _impl->opaquePointPainter->add(point);
    }

    _update();
}

void Q3DObserver::clearLines()
{
    _impl->opaqueLinePainter->clear();
    _impl->transparentLinePainter->clear();
    _update();
}

void Q3DObserver::reserveLines(int lines)
{
    _impl->opaqueLinePainter->reserve(lines);
    _impl->transparentLinePainter->reserve(lines);
}

void Q3DObserver::addLine(const QVector3D& from, const QVector3D& to, const Q3DColor& color)
{
    if(color.hasAlpha())
    {
        _impl->transparentLinePainter->add(Q3DVertex(from, color), Q3DVertex(to, color));
    }
    else
    {
        _impl->opaqueLinePainter->add(Q3DVertex(from, color), Q3DVertex(to, color));
    }

    _update();
}

void Q3DObserver::addLine(const Q3DVertex& from, const Q3DVertex& to)
{
    if(from.color().hasAlpha() || to.color().hasAlpha())
    {
        _impl->transparentLinePainter->add(from, to);
    }
    else
    {
        _impl->opaqueLinePainter->add(from, to);
    }

    _update();
}

bool Q3DObserver::drawBackFaces() const noexcept
{
    return _impl->drawBackFaces;
}

void Q3DObserver::setDrawBackFaces(bool draw)
{
    _impl->drawBackFaces = draw;
    _update();
}

void Q3DObserver::clearTriangles()
{
    _impl->opaqueTrianglePainter->clear();
    _impl->transparentTrianglePainter->clear();
    _update();
}

void Q3DObserver::reserveTriangles(int triangles)
{
    _impl->opaqueTrianglePainter->reserve(triangles);
    _impl->transparentTrianglePainter->reserve(triangles);
}

void Q3DObserver::addTriangle(const QVector3D& vertex1, const QVector3D& vertex2, const QVector3D& vertex3,
                            const Q3DColor& color)
{
    if(color.hasAlpha())
    {
        _impl->transparentTrianglePainter->add(
                    Q3DVertex(vertex1, color), Q3DVertex(vertex2, color), Q3DVertex(vertex3, color));
    }
    else
    {
        _impl->opaqueTrianglePainter->add(
                    Q3DVertex(vertex1, color), Q3DVertex(vertex2, color), Q3DVertex(vertex3, color));
    }

    _update();
}

void Q3DObserver::addTriangle(const Q3DVertex& vertex1, const Q3DVertex& vertex2, const Q3DVertex& vertex3)
{
    if(vertex1.color().hasAlpha() || vertex2.color().hasAlpha() || vertex3.color().hasAlpha())
    {
        _impl->transparentTrianglePainter->add(vertex1, vertex2, vertex3);
    }
    else
    {
        _impl->opaqueTrianglePainter->add(vertex1, vertex2, vertex3);
    }

    _update();
}

void Q3DObserver::clearTexts()
{
    _impl->textWidget->clear();
    _update();
}

void Q3DObserver::reserveTexts(int texts)
{
    _impl->textWidget->reserve(texts);
}

void Q3DObserver::addText(const QVector3D& position, const Q3DColor& color, QString text)
{
    addText(Q3DVertex(position, color), std::move(text));
}

void Q3DObserver::addText(const Q3DVertex& vertex, QString text)
{
    _impl->textWidget->add(vertex, std::move(text));
    _update();
}

void Q3DObserver::clear()
{
    clearPoints();
    clearLines();
    clearTriangles();
    clearTexts();
}

void Q3DObserver::reset()
{
    clear();
    setZNear(Q3DConstants::defaultZNear);
    setZFar(Q3DConstants::defaultZFar);
    setFov(Q3DConstants::defaultFov);
    setBackgroundColor(QColor());
    setLightWeight(Q3DConstants::defaultLightWeight);
    setPointSize(Q3DConstants::defaultPointSize);
    setDrawBackFaces(Q3DConstants::defaultDrawBackFaces);
    resetCamera();
}

void Q3DObserver::mousePressEvent(QMouseEvent* event)
{
    _impl->lastMousePos = QVector2D(event->x(), event->y());
    _impl->lastMouseButton = event->button();
}

void Q3DObserver::mouseMoveEvent(QMouseEvent* event)
{
    auto xDelta = float(event->x() - _impl->lastMousePos.x()) / width();
    auto yDelta = float(event->y() - _impl->lastMousePos.y()) / height();
    _impl->lastMousePos = QVector2D(event->x(), event->y());

    enum class Action
    {
        ROTATE, TRANSLATE
    };

    Action action;

    if(_impl->lastMouseButton == Qt::LeftButton)
    {
        action = Action::ROTATE;
    }
    else if(_impl->lastMouseButton == Qt::MiddleButton)
    {
        action = Action::TRANSLATE;
    }
    else
    {
        return;
    }

    switch(action)
    {

    case Action::ROTATE:
        _impl->camera.rotation += QVector3D(180 * yDelta, 0, 180 * xDelta);
        break;

    case Action::TRANSLATE:
        {
            auto factor = -_impl->camera.translation.z() * Q3DConstants::translationFactor;
            _impl->camera.translation += QVector3D(xDelta * factor, -yDelta * factor, 0);
        }
        break;
    }

    _impl->setupModelView = true;
    _update();
}

void Q3DObserver::mouseReleaseEvent(QMouseEvent*)
{
    _impl->lastMousePos = QVector2D();
    _impl->lastMouseButton = Qt::NoButton;
}

void Q3DObserver::wheelEvent(QWheelEvent* event)
{
    auto& translation = _impl->camera.translation;
    auto zDelta = std::pow(Q3DConstants::scaleFactor, float(event->delta()));
    translation.setZ(translation.z() * zDelta);
    _impl->setupModelView = true;
    _update();
}

void Q3DObserver::initializeGL()
{
    initializeOpenGLFunctions();

    if(! _impl->init())
    {
        Q3D_LOG_ERROR << "Impl init failed";
        return;
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void Q3DObserver::resizeGL(int w, int h)
{
    _impl->aspectRatio = float(w) / std::max(h, 1);
    _impl->setupProjection = true;
}

void Q3DObserver::paintGL()
{
    if(active())
    {
        if(_impl->setupBackgroundColor)
        {
            const QColor& bc = _impl->backgroundColor;
            glClearColor(float(bc.redF()), float(bc.greenF()), float(bc.blueF()), 1);
            _impl->setupBackgroundColor = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(! empty())
        {
            const auto& projectionMatrix = projection();
            const auto& modelViewMatrix = modelView();
            auto& program = _impl->program;
            program.setUniformValue("u_mvpMatrix", projectionMatrix * modelViewMatrix);
            program.setUniformValue("u_pointSize", _impl->pointSize);

            auto viewport = rect();
            QVector3D cameraPosition(viewport.width() * 0.5f, viewport.height() * 0.5f, _impl->zNear);
            cameraPosition = cameraPosition.unproject(modelViewMatrix, projectionMatrix, viewport);
            program.setUniformValue("u_lightPosition", cameraPosition);
            program.setUniformValue("u_lightWeight", _impl->lightWeight);

            int positionAttribute = program.attributeLocation("a_position");
            int normalAttribute = program.attributeLocation("a_normal");
            int colorAttribute = program.attributeLocation("a_color");
            bool drawBackFaces = _impl->drawBackFaces;
            glEnable(GL_DEPTH_TEST);
            _impl->opaquePointPainter->paint(positionAttribute, normalAttribute, colorAttribute, program);
            _impl->opaqueLinePainter->paint(positionAttribute, normalAttribute, colorAttribute, program);
            _impl->opaqueTrianglePainter->paint(drawBackFaces, positionAttribute, normalAttribute,
                                                colorAttribute, program);

            glDisable(GL_DEPTH_TEST);
            _impl->transparentPointPainter->paint(positionAttribute, normalAttribute, colorAttribute, program);
            _impl->transparentLinePainter->paint(positionAttribute, normalAttribute, colorAttribute, program);
            _impl->transparentTrianglePainter->paint(drawBackFaces, positionAttribute, normalAttribute,
                                                     colorAttribute, program);
        }
    }
}

void Q3DObserver::_update()
{
    update();
    _impl->textWidget->update();
}
