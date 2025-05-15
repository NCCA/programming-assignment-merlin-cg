cmake_minimum_required(VERSION 3.5)
set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})
project(ParticleQtBuild VERSION 0.1 LANGUAGES CXX)
set(TargetName ParticleQt)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(NGL CONFIG REQUIRED)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/ui/)
qt_add_resources(DARK_STYLE_RCC qdarkstyle/dark/darkstyle.qrc)

add_executable(${TargetName})
target_sources(${TargetName} PRIVATE
        src/main.cpp
        src/MainWindow.cpp
        src/NGLScene.cpp
        src/Emitter.cpp
        src/Plane.cpp
        src/NGLScene.cpp
        src/NGLSceneMouseControls.cpp
        include/Emitter.h
        include/MainWindow.h
        include/NGLScene.h
        include/MainWindow.h
        include/WindowParams.h
        include/PerlinNoise.hpp
        include/Plane.h
        ui/MainWindow.ui
        shaders/ParticleFragment.glsl
        shaders/ParticleVertex.glsl
        shaders/ColourFragment.glsl
        shaders/ColourVertex.glsl
        shaders/HeightColourFragment.glsl
        shaders/HeightColourVertex.glsl
        shaders/PhongFragment.glsl
        shaders/PhongVertex.glsl

        ${DARK_STYLE_RCC}
)


target_include_directories(ParticleQt PRIVATE include)
target_link_libraries(ParticleQt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
target_link_libraries(${TargetName} PRIVATE NGL)

add_custom_target(${TargetName}CopyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:${TargetName}>/shaders
)

