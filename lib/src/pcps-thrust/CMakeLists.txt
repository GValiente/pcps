cmake_minimum_required(VERSION 3.4)
project(pcps-thrust)

# Define sources:
set(SOURCES
    src/pcps_thrust_cached_allocator.cu
    src/pcps_thrust_cloud.cu
    src/pcps_thrust_organizer.cu
    src/pcps_thrust_normal_extractor.cu
    src/pcps_thrust_normal_splitter.cu
)

# Add a CUDA library with the above sources:
CUDA_ADD_LIBRARY(${PROJECT_NAME} ${SOURCES})

# Define CUDA include directories:
CUDA_INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/include")
