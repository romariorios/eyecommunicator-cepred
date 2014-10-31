cmake_minimum_required(VERSION 2.8.11)

project(eyecommunicator-cepred)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

find_package(Qt5Widgets REQUIRED)
include_directories(
    ${Qt5Widgets_INCLUDE_DIRS})

qt5_wrap_ui(
    eyecommunicator_ui
    mainwindow.ui)

add_executable(
    eyecommunicator
    main.cpp
    mainwindow.cpp
    ${eyecommunicator_ui})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(plugins)
link_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/plugins/dummyeyetrackerplugin)

target_link_libraries(
    eyecommunicator
    dummyeyetrackerplugin
    Qt5::Widgets)

install(
    TARGETS eyecommunicator RUNTIME
    DESTINATION bin)