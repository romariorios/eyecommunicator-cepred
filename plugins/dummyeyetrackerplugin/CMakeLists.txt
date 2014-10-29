set(CMAKE_CXX_FLAGS "-fPIE ${CMAKE_CXX_FLAGS}")

qt5_wrap_ui(
    plugin_ui
    dummytrackingcalibrationwidget.ui)

add_library(
    dummyeyetrackerplugin
    dummyeyetrackerplugin.cpp
    dummytrackingcalibrationwidget.cpp
    ${plugin_ui})
