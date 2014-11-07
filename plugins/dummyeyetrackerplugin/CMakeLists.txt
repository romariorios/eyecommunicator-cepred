qt5_wrap_ui(
    plugin_ui
    dummytrackingcalibrationwidget.ui)

add_library(
    dummyeyetrackerplugin SHARED
    dummyeyetrackerplugin.cpp
    dummytrackingcalibrationwidget.cpp
    ${plugin_ui})

install(
    TARGETS dummyeyetrackerplugin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

target_link_libraries(
    dummyeyetrackerplugin
    Qt5::Widgets)
