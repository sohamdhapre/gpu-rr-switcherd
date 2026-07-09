cmake_minimum_required(VERSION 3.10)
project(gpu-switcher VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 1)

find_package(PkgConfig REQUIRED)
pkg_check_modules(UDEV REQUIRED libudev)
pkg_check_modules(SDBUS REQUIRED sdbus-c++)

set(SOURCES
    src/main.cpp
    src/powerMonitor.cpp
    src/displayManager.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${UDEV_INCLUDE_DIRS} ${SDBUS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${UDEV_LIBRARIES} ${SDBUS_LIBRARIES})