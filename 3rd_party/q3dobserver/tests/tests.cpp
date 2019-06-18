#include <random>
#include <functional>
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>
#include "Q3DObserver"
#include "catch.hpp"

namespace
{
    void showObserver(const char* title, const std::function<void(Q3DObserver&)>& function)
    {
        int argc = 0;
        char** argv = nullptr;
        QApplication app(argc, argv);

        QSurfaceFormat format;
        format.setSamples(4);
        QSurfaceFormat::setDefaultFormat(format);

        Q3DObserver observer;
        observer.setWindowTitle(title);
        observer.resize(1280, 720);
        observer.show();
        REQUIRE(observer.active());
        function(observer);

        QApplication::exec();
    }

    void drawPoints(int points, int pointSize, Q3DObserver& observer, int seed = 0)
    {
        observer.setPointSize(pointSize);
        REQUIRE(std::abs(observer.pointSize() - pointSize) < 1e-6f);

        std::default_random_engine re(seed);
        std::uniform_real_distribution<float> posDist(-1, 1);
        std::uniform_real_distribution<float> colorDist(0, 1);
        observer.reservePoints(points);

        for(int i = 0; i < points; ++i)
        {
            QVector3D position(posDist(re), posDist(re), posDist(re));

            if(i % 2)
            {
                observer.addPoint(position, Q3DColor(colorDist(re), colorDist(re), colorDist(re)));
            }
            else
            {
                observer.addPoint(position, Q3DColor(colorDist(re), colorDist(re), colorDist(re), colorDist(re)));
            }
        }
    }

    void drawLines(int lines, Q3DObserver& observer, int seed = 0)
    {
        std::default_random_engine re(seed);
        std::uniform_real_distribution<float> posDist(-1, 1);
        std::uniform_real_distribution<float> colorDist(0, 1);
        observer.reserveLines(lines);

        for(int i = 0; i < lines; ++i)
        {
            QVector3D from(posDist(re), posDist(re), posDist(re));
            QVector3D to(posDist(re), posDist(re), posDist(re));

            if(i % 2)
            {
                observer.addLine(from, to, Q3DColor(colorDist(re), colorDist(re), colorDist(re)));
            }
            else
            {
                observer.addLine(from, to, Q3DColor(colorDist(re), colorDist(re), colorDist(re), colorDist(re)));
            }
        }
    }

    void drawTriangles(int triangles, Q3DObserver& observer, int seed = 0)
    {
        std::default_random_engine re(seed);
        std::uniform_real_distribution<float> posDist(-1, 1);
        std::uniform_real_distribution<float> colorDist(0, 1);
        observer.reserveTriangles(triangles);

        for(int i = 0; i < triangles; ++i)
        {
            QVector3D vertex1(posDist(re), posDist(re), posDist(re));
            QVector3D vertex2(posDist(re), posDist(re), posDist(re));
            QVector3D vertex3(posDist(re), posDist(re), posDist(re));

            if(i % 2)
            {
                observer.addTriangle(vertex1, vertex2, vertex3,
                                   Q3DColor(colorDist(re), colorDist(re), colorDist(re)));
            }
            else
            {
                observer.addTriangle(vertex1, vertex2, vertex3,
                                   Q3DColor(colorDist(re), colorDist(re), colorDist(re), colorDist(re)));
            }
        }
    }

    void draw3DTexts(int texts, Q3DObserver& observer, int seed = 0)
    {
        std::default_random_engine re(seed);
        std::uniform_real_distribution<float> posDist(-1, 1);
        std::uniform_real_distribution<float> colorDist(0, 1);
        observer.reserveTexts(texts);

        auto font = observer.font();
        font.setPointSize(14);
        observer.setFont(font);

        for(int i = 0; i < texts; ++i)
        {
            QVector3D vertex(posDist(re), posDist(re), posDist(re));

            if(i % 2)
            {
                observer.addText(vertex, Q3DColor(colorDist(re), colorDist(re), colorDist(re)), "3D");
            }
            else
            {
                observer.addText(vertex, Q3DColor(colorDist(re), colorDist(re), colorDist(re), colorDist(re)), "3D");
            }
        }
    }

    void drawOverlayText(const QString& overlayText, Q3DObserver& observer)
    {
        auto overlay = observer.overlayWidget();
        REQUIRE(overlay);

        auto label = new QLabel(overlayText, overlay);
        auto labelFont = overlay->font();
        labelFont.setPointSize(labelFont.pointSize() + 12);
        label->setFont(labelFont);
        label->setStyleSheet("QLabel { color : white; }");

        auto layout = new QVBoxLayout(overlay);
        layout->addWidget(label, 0, Qt::AlignCenter);
    }
}

TEST_CASE("Empty")
{
    showObserver("Empty", [](Q3DObserver& observer) {
        REQUIRE(observer.empty());
    });
}

TEST_CASE("Background color")
{
    showObserver("Background color", [](Q3DObserver& observer) {
        QColor color(255, 255, 0);
        observer.setBackgroundColor(color);
        REQUIRE(observer.empty());
        REQUIRE(observer.backgroundColor() == color);
    });
}

TEST_CASE("Points")
{
    showObserver("Points", [](Q3DObserver& observer) {
        drawPoints(1000, 10, observer);
        REQUIRE(! observer.empty());
    });
}

TEST_CASE("Lines")
{
    showObserver("Lines", [](Q3DObserver& observer) {
        drawLines(1000, observer);
        REQUIRE(! observer.empty());
    });
}

TEST_CASE("Triangles with back faces")
{
    showObserver("Triangles with back faces", [](Q3DObserver& observer) {
        bool drawBackFaces = true;
        observer.setDrawBackFaces(drawBackFaces);
        REQUIRE(observer.drawBackFaces() == drawBackFaces);

        drawTriangles(10, observer);
        REQUIRE(! observer.empty());
    });
}

TEST_CASE("Triangles without back faces")
{
    showObserver("Triangles without back faces", [](Q3DObserver& observer) {
        bool drawBackFaces = false;
        observer.setDrawBackFaces(drawBackFaces);
        REQUIRE(observer.drawBackFaces() == drawBackFaces);

        drawTriangles(10, observer);
        REQUIRE(! observer.empty());
    });
}

TEST_CASE("3D texts")
{
    showObserver("3D texts", [](Q3DObserver& observer) {
        draw3DTexts(1000, observer);
        REQUIRE(! observer.empty());
    });
}

TEST_CASE("Light weight")
{
    showObserver("Light weight", [](Q3DObserver& observer) {
        float lightWeight = 0.95f;
        observer.setLightWeight(lightWeight);
        REQUIRE(std::abs(observer.lightWeight() - lightWeight) < 1e-6f);

        drawTriangles(10, observer);
    });
}

TEST_CASE("Overlay text")
{
    showObserver("Overlay text", [](Q3DObserver& observer) {
        drawTriangles(10, observer);
        draw3DTexts(10, observer);
        drawOverlayText("Text drawn on overlay", observer);
    });
}

TEST_CASE("Everything")
{
    showObserver("Everything", [](Q3DObserver& observer) {
        observer.setBackgroundColor(QColor(64, 64, 64));
        drawPoints(10, 10, observer, 1);
        drawLines(10, observer, 2);
        drawTriangles(10, observer, 3);
        draw3DTexts(10, observer, 4);
        drawOverlayText("Text drawn on overlay", observer);
    });
}
