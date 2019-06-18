cmake_minimum_required(VERSION 3.4)
project(pcps)

# Define library implementation options:
set(PCPS_IMPLEMENTATION "CPU" CACHE STRING "PCPS implementation")
set_property(CACHE PCPS_IMPLEMENTATION PROPERTY STRINGS CPU CUDA OpenCL)

# Define sources:
set(SOURCES
    src/pcps_point.cpp
    src/pcps_cloud.cpp
    src/pcps_device_cloud.cpp
    src/pcps_organizer.cpp
    src/pcps_normal_extractor.cpp
    src/pcps_normal_region.cpp
    src/pcps_normal_splitter.cpp
    src/pcps_normal_merger.cpp
    src/pcps_plane_segmentator.cpp
)

# Define implementation sources:
if(PCPS_IMPLEMENTATION MATCHES OpenCL)
    add_definitions(-DPCPS_OPENCL)

    set(SOURCES
        ${SOURCES}
        src/pcps_context_opencl.cpp
        src/pcps_device_cloud_opencl.cpp
        src/pcps_organizer_opencl.cpp
        src/pcps_normal_extractor_opencl.cpp
        src/pcps_normal_splitter_opencl.cpp
    )
elseif(PCPS_IMPLEMENTATION MATCHES CUDA)
    add_definitions(-DPCPS_CUDA)

    set(SOURCES
        ${SOURCES}
        src/pcps_context_cuda.cpp
        src/pcps_device_cloud_cuda.cpp
        src/pcps_organizer_cuda.cpp
        src/pcps_normal_extractor_cuda.cpp
        src/pcps_normal_splitter_cuda.cpp
    )
else()
    set(SOURCES
        ${SOURCES}
        src/pcps_context_cpu.cpp
        src/pcps_device_cloud_cpu.cpp
        src/pcps_organizer_cpu.cpp
        src/pcps_normal_extractor_cpu.cpp
        src/pcps_normal_splitter_cpu.cpp
    )
endif()

# Add a library with the above sources:
add_library(${PROJECT_NAME} ${SOURCES})

# Define include directories:
target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# Include implementation libraries:
if(PCPS_IMPLEMENTATION MATCHES OpenCL)
    # Include OpenCL library:
    find_package(OpenCL REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARY})

    # Include boost library:
    set(PCPS_BOOST_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/boost/include" CACHE STRING "boost library path")
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${PCPS_BOOST_PATH}
    )
elseif(PCPS_IMPLEMENTATION MATCHES CUDA)
    # Include CUDA library:
    find_package(CUDA REQUIRED)
    include_directories(${CUDA_INCLUDE_DIRS})
    list(APPEND CUDA_NVCC_FLAGS "-std=c++11")

    # Include thrust library:
    set(PCPS_THRUST_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/thrust/include" CACHE STRING "thrust library path")
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${PCPS_THRUST_PATH}
    )

    # Include pcps-thrust library:
    subdirs(${CMAKE_CURRENT_SOURCE_DIR}/src/pcps-thrust)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/pcps-thrust/include)
    target_link_libraries(${PROJECT_NAME} pcps-thrust)
endif()
