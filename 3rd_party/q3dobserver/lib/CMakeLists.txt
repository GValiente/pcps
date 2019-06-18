cmake_minimum_required(VERSION 3.4)
project(q3dobserver)

# Define sources:
set(SOURCES
    src/q3dcolor.cpp
    src/q3dvertex.cpp
    src/q3dobserver.cpp
    src/q3dcamera.cpp
    src/q3dpointpainter.cpp
    src/q3dlinepainter.cpp
    src/q3dtrianglepainter.cpp
    src/q3dtextwidget.cpp
)

# Add a library with the above sources:
add_library(${PROJECT_NAME} ${SOURCES})

# Define include directories:
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Include Qt5 library:
find_package(Qt5Widgets)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Widgets)
