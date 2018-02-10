cmake_minimum_required(VERSION 3.10)

project(fzy VERSION 0.0.1)

include(CTest)

find_package(Qt5 REQUIRED COMPONENTS Qml Quick QuickControls2)

if (BUILD_TESTING)
  find_package(Qt5 REQUIRED COMPONENTS Test)
endif()

add_library(fzy STATIC fzy.cpp fzy.h)
target_compile_features(fzy PUBLIC cxx_std_17)

add_executable(fzy-qml
  FzyModel.cpp
  FzyModel.h
  fzy-qml.cpp
  qml/qml.qrc
)

target_link_libraries(fzy-qml
  PRIVATE
    fzy
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
)

target_compile_features(fzy-qml PUBLIC cxx_std_17)

set_target_properties(fzy-qml PROPERTIES
  AUTOMOC ON
  AUTORCC ON
)

if (BUILD_TESTING)
  add_executable(tst-fzy tst-fzy.cpp)
  target_compile_features(tst-fzy PUBLIC cxx_std_17)
  set_property(TARGET tst-fzy PROPERTY AUTOMOC ON)
  target_link_libraries(tst-fzy PRIVATE fzy Qt5::Test)
  add_test(tst-fzy tst-fzy)
endif()
