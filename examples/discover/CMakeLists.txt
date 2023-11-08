cmake_minimum_required(VERSION 3.10)
project (lifxapp)

FILE (GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE (GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

set (CMAKE_CXX_STANDARD 17)
set (THREADS_PREFER_PTHREAD_FLAG ON)
set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package (Qt5Network CONFIG REQUIRED)
find_package (Qt5Gui CONFIG REQUIRED)
find_package (Qt5Widgets CONFIG REQUIRED)

add_library(qtlifxlib SHARED IMPORTED)

# Provide the full path to the library, so CMake knows where to find it.
set_target_properties(qtlifxlib PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/library/libqtlifx.so)

# Tell CMake to create the helloworld executable
include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/../../library/public")

add_executable (${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Use the Widgets module from Qt 5.
target_link_libraries (${PROJECT_NAME} Qt5::Network Qt5::Gui Qt5::Widgets qtlifxlib)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../library/public")

add_dependencies(lifxapp qtlifx)
