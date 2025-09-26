cmake_minimum_required(VERSION 3.16)
project(CoursePlannerQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Quick Core Gui Qml REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    CourseGraph.cpp
    CourseGraph.h
    CourseGraph.cpp
)

# Thêm resource QML
qt_add_resources(QML_RESOURCES qml.qrc)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
)

target_sources(${PROJECT_NAME} PRIVATE ${QML_RESOURCES})
