cmake_minimum_required(VERSION 3.4)
project(q3dobserver-tests)

# Define sources:
set(SOURCES
    tests.cpp
)

# Add a executable with the above sources:
add_executable(${PROJECT_NAME} ${SOURCES})

# Link static libraries:
target_link_libraries(${PROJECT_NAME} q3dobserver)

# Include catch-mini library:
add_subdirectory(3rd_party/catch-mini)
target_link_libraries(${PROJECT_NAME} catch-mini)
