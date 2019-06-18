Q3DObserver
===========

Q3DObserver is a multi-platform C++11 library based on Qt that eases the creation of 3D viewer widgets.

Q3DObserver provides some of the typical 3D viewer functionalities, such as an orbit camera which can be rotated, scaled and translated using the mouse. Based on the Qt toolkit, it compiles on any architecture (Unix-Linux, Mac, Windows). Q3DObserver does not display 3D scenes in various formats, but it can be the base for the coding of such a viewer.

Q3DObserver is licensed under [Zlib license](LICENSE.txt). OpenGL knowledge is NOT required to use this library.

![screenshot of some 3D shapes rendered with Q3DObserver](screenshot.png?raw=true)

## Tested build configurations (minimum required versions may be lower)

* Ubuntu 16.04 with gcc 4.9.
	* and macOS 10.11 with Xcode 8.2.
	* and Windows 7 with Visual Studio 2017.
* CMake 3.4.
* Qt 5.5.

## Building and running tests on Linux and Mac

```
cd q3dobserver
mkdir build
cd build
cmake -DQ3DO_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release ..
make
./tests/q3dobserver-tests
```

## Main features

* Show 3D figures such as points, lines, triangles and text strings without writing OpenGL code.
* Orbit camera which can be rotated, scaled and translated using the mouse.
* Open source commercial friendly license (Zlib): compatible with open and closed source projects.

## Usage

The following example shows how to draw some 3D figures:

```cpp
// main.cpp:

#include <QApplication>
#include "Q3DObserver"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    // Enable antialiasing:
    QSurfaceFormat format;
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    // Create widget and add 3D figures:
    Q3DObserver observer;
    observer.addPoint(QVector3D(0, 0, 0), QColor(Qt::red));
    observer.addLine(QVector3D(-1, -1, -1), QVector3D(1, 1, 1), QColor(Qt::green));
    observer.addTriangle(QVector3D(0, 1, 0), QVector3D(1, 0, 0), QVector3D(-1, 0, 0), QColor(Qt::blue));
    observer.addText(QVector3D(0, 0.5, 0), QColor(Qt::white), "3D text");

    // Show widget:
    observer.resize(1280, 720);
    observer.show();

    return QApplication::exec();
}
```
