cmake_minimum_required(VERSION 3.10)
project (qtlifx DESCRIPTION "LIFX control library for Linux")

FILE(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public/*.h")

option(LIFX_BUILD_STATIC "Build as a static object (default OFF)" OFF)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_compile_options(-Wall -Wextra -Wpedantic)

# Find the QtWidgets library
find_package(Qt5Network CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)

set(PROJ_VERSION_MAJOR 1)
set(PROJ_VERSION_MINOR 0)
set(PROJ_VERSION_PATCH 0)
set(PROJ_VERSION ${PROJ_VERSION_MAJOR}.${PROJ_VERSION_MINOR}.${PROJ_VERSION_PATCH} )

if(LIFX_BUILD_STATIC)
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
else()
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${PUBLIC})
    set_target_properties (${PROJECT_NAME} PROPERTIES VERSION ${PROJ_VERSION} SOVERSION ${PROJ_VERSION_MAJOR} )
endif()

# Use the Widgets module from Qt 5.
target_link_libraries(${PROJECT_NAME} Qt5::Network Qt5::Gui)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/private")

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${PUBLIC} DESTINATION include)

