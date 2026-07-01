cmake_minimum_required(VERSION 3.10)
project(SteloEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options("-O3")

add_subdirectory(Stelo)
add_executable(SteloEngine Examples/Test.cpp)

target_compile_definitions(SteloEngine PRIVATE
    STELO_DESIGN_PHYSICS_SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Shaders/DesignPhysicsTriangle.metal"
)

target_link_libraries(SteloEngine PRIVATE Stelo::Engine)
