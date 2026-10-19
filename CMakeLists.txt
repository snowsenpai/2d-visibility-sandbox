cmake_minimum_required(VERSION 3.28)
project(visibility-sandbox LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# SFML 3.1.0
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Dear ImGui (compatible 1.91.x version)
FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(ImGui)

# ImGui-SFML bridge
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR ${imgui_SOURCE_DIR})

FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v3.0
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(ImGui-SFML)

add_executable(visibility-sandbox
    src/main.cpp
    src/Vec2.h
    src/Vec2.cpp
    src/DebugUI.h
    src/DebugUI.cpp
    src/DebugState.h
    src/DebugPanels.h
    src/DebugPanels.cpp
    src/MathUtils.h
    src/GeometryTests.h
    src/GeometryTests.cpp
    src/Geometry.h
    src/Geometry.cpp
    src/Components.h
    src/Entity.h
    src/Entity.cpp
    src/EntityManager.h
    src/EntityManager.cpp
    src/Game.h
    src/Game.cpp)

target_compile_features(visibility-sandbox PRIVATE cxx_std_17)
target_link_libraries(visibility-sandbox PRIVATE
    SFML::Graphics
    ImGui-SFML::ImGui-SFML
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT visibility-sandbox)
