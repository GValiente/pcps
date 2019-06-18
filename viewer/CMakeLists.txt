cmake_minimum_required(VERSION 3.4)
project(pcps-viewer)

# Define sources:
set(SOURCES
    src/pcps_vw_settings.cpp
    src/pcps_vw_project.cpp
    src/pcps_vw_color_map.cpp
    src/pcps_vw_cloud_widget.cpp
    src/pcps_vw_input_cloud_widget.cpp
    src/pcps_vw_organization_cloud_widget.cpp
    src/pcps_vw_normal_extraction_cloud_widget.cpp
    src/pcps_vw_normal_split_cloud_widget.cpp
    src/pcps_vw_normal_merge_cloud_widget.cpp
    src/pcps_vw_stages_manager_widget.cpp
    src/pcps_vw_options_tree_widget.cpp
    src/pcps_vw_options_widget.cpp
    src/pcps_vw_central_widget.cpp
    src/pcps_vw_main_window.cpp
    src/main.cpp
)

# Enable Qt5 slots:
find_package(Qt5Widgets)
set(CMAKE_AUTOMOC ON)

# Add a executable with the above sources:
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# Include PCL library:
find_package(PCL 1.7 REQUIRED COMPONENTS io)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})
list(REMOVE_ITEM PCL_LIBRARIES "vtkproj4")
target_link_libraries(${PROJECT_NAME} ${PCL_LIBRARIES})

# Include q3dobserver library:
add_subdirectory(../3rd_party/q3dobserver q3dobserver)
target_link_libraries(${PROJECT_NAME} q3dobserver)

# Include pcps library:
target_link_libraries(${PROJECT_NAME} pcps)
