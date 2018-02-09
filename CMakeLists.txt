cmake_minimum_required(VERSION 3.10)

project(fzy VERSION 0.0.1)

include(CTest)

find_package(Qt5 REQUIRED COMPONENTS Qml Quick QuickControls2)

add_executable(fzy
  fzy.cpp
  FzyModel.h
  FzyModel.cpp
  qml/qml.qrc
)

target_link_libraries(fzy
  Qt5::Qml
  Qt5::Quick
  Qt5::QuickControls2
)

target_compile_features(fzy PUBLIC cxx_std_17)

# c.f. https://cmake.org/cmake/help/latest/manual/cmake-qt.7.html
set_property(TARGET fzy PROPERTY AUTOMOC ON)
set_property(TARGET fzy PROPERTY AUTOUIC ON)
set_property(TARGET fzy PROPERTY AUTORCC ON)
target_include_directories(fzy PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
