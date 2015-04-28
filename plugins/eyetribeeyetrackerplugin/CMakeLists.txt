find_package(Qt5Network)

add_library(
    eyetribeeyetrackerplugin SHARED
    eyetribeeyetrackerplugin.cpp
    ${plugin_ui})

install(
    TARGETS eyetribeeyetrackerplugin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

target_link_libraries(
    eyetribeeyetrackerplugin
    Qt5::Network
    Qt5::Widgets)
