find_package(Qt5Network)

add_library(
    gazepointeyetrackerplugin SHARED
    gazepointeyetrackerplugin.cpp
    ${plugin_ui})

install(
    TARGETS gazepointeyetrackerplugin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

target_link_libraries(
    gazepointeyetrackerplugin
    Qt5::Network
    Qt5::Widgets)
