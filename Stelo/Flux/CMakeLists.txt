cmake_minimum_required(VERSION 3.16) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Flux STATIC
    Widget.cpp
    DataStruct.cpp
    Platform/InputData.cpp
)

set(ValidGraphics "")
if(PLATFORM_BACKEND STREQUAL "WINDOWS")
    
elseif(PLATFORM_BACKEND STREQUAL "LINUX")
    
elseif(PLATFORM_BACKEND STREQUAL "MACOS")
    add_library(Widgets_MacOS STATIC 
        Platform/MacOS/App.mm
        Platform/MacOS/ImplNativeFunc.mm
    )
    find_library(APPKIT_FRAMEWORK AppKit)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    find_library(CORE_HAPTICS CoreHaptics)
    find_library(GAME_CONTROLLER GameController)

    target_link_libraries(Widgets_MacOS PRIVATE
        "-framework IOKit"
        "-framework AppKit"
        "-framework QuartzCore"
        "-framework CoreHaptics"
        "-framework GameController"
        "-framework MetalPerformanceShaders"
        "-framework Metal"
        "-framework MetalKit" 
    )

    target_link_libraries(Flux PUBLIC Widgets_MacOS)
endif()

target_include_directories(Flux PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(Stelo::Flux ALIAS Flux)
