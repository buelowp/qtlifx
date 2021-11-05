cmake_minimum_required(VERSION 3.0)
project (qtlifx)


FILE (GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
FILE (GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

set (CMAKE_CXX_STANDARD 17)
set (THREADS_PREFER_PTHREAD_FLAG ON)
set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package (Qt5Network CONFIG REQUIRED)

# Tell CMake to create the helloworld executable
add_executable (${PROJECT_NAME} ${SOURCES})

# Use the Widgets module from Qt 5.
target_link_libraries (${PROJECT_NAME} Qt5::Network)

# Install the executable
install (TARGETS qtlifx DESTINATION bin)
