cmake_minimum_required(VERSION 3.4)
project(pcps-tests)

# Define sources:
set(SOURCES
    src/test_util.cpp
    src/test.cpp
    src/example.cpp
)

# Define test data folder:
add_definitions(-DPCPS_TEST_DATA_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/data")

# Add a executable with the above sources:
add_executable(${PROJECT_NAME} ${SOURCES})

# Define include directories:
target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# Link static libraries:
target_link_libraries(${PROJECT_NAME} pcps)

# Include PCL library:
find_package(PCL 1.7 REQUIRED COMPONENTS io)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})
list(REMOVE_ITEM PCL_LIBRARIES "vtkproj4")
target_link_libraries(${PROJECT_NAME} ${PCL_LIBRARIES})

# Include catch-mini library:
add_subdirectory(../3rd_party/catch-mini catch-mini)
target_link_libraries(${PROJECT_NAME} catch-mini)
