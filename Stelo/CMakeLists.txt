cmake_minimum_required(VERSION 3.16) 

set(PLATFORM_BACKEND "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(PLATFORM_BACKEND WINDOWS)
elseif(APPLE)
    if(DEFINED CMAKE_OSX_SYSROOT AND (CMAKE_OSX_SYSROOT STREQUAL "iphoneos" OR CMAKE_OSX_SYSROOT STREQUAL "iphonesimulator"))
        set(PLATFORM_BACKEND IOS)
    else()
        set(PLATFORM_BACKEND MACOS)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_BACKEND LINUX)

elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(PLATFORM_BACKEND FREEBSD)

elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(PLATFORM_BACKEND ANDROID)
endif()

add_compile_definitions(${PLATFORM_BACKEND})

message(STATUS "Platform: ${PLATFORM_BACKEND}")

add_subdirectory("Argon")
add_subdirectory("Flux")

add_library(Engine STATIC 
    Math/Matrix.cpp
    Math/Quaternion.cpp
    Math/Vector.cpp

    ECS/Component.cpp
    World/GameObject.cpp

    System/GraphicManager.cpp
)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Engine PUBLIC Stelo::Argon Stelo::Flux)

add_library(Stelo::Engine ALIAS Engine)