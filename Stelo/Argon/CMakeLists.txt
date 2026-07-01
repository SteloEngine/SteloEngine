cmake_minimum_required(VERSION 3.16)
project(Argon)

add_library(Argon STATIC
    Interface/Device.cpp
    Interface/Invalid.cpp
)

set(ARGON_ENABLE_METAL 0)
set(ARGON_ENABLE_VULKAN 0)
set(ARGON_ENABLE_D3D12 0)
set(ARGON_ENABLE_D3D11 0)
set(ARGON_ENABLE_OPENGL 0)

set(ValidGraphics "")

if(PLATFORM_BACKEND STREQUAL "MACOS")
    list(APPEND ValidGraphics "METAL")
    set(ARGON_ENABLE_METAL 1)

    target_sources(Argon PRIVATE 
        Platform/Metal/Buffer.mm
        Platform/Metal/Cmd.mm
        Platform/Metal/Device.mm
        Platform/Metal/Pipeline.mm
        Platform/Metal/Queue.mm
        Platform/Metal/Shader.mm
        Platform/Metal/SwapChain.mm
        Platform/Metal/Texture.mm
    )

    find_library(APPKIT_FRAMEWORK AppKit)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    find_library(CORE_HAPTICS CoreHaptics)
    find_library(GAME_CONTROLLER GameController)

    target_link_libraries(Argon PRIVATE
        "-framework IOKit"
        "-framework AppKit"
        "-framework QuartzCore"
        "-framework CoreHaptics"
        "-framework GameController"
        "-framework MetalPerformanceShaders"
        "-framework Metal"
        "-framework MetalKit" 
    )

    target_compile_options(Argon PRIVATE
        $<$<COMPILE_LANGUAGE:OBJC>:-fno-objc-arc>
        $<$<COMPILE_LANGUAGE:OBJCXX>:-fno-objc-arc>
    )
    
elseif(PLATFORM_BACKEND STREQUAL "WINDOWS")
    find_library(DX12_LIB d3d12)
    find_library(DX11_LIB d3d11)
    find_library(VK_LIB vulkan-1)

    if(DX12_LIB)
        set(ARGON_ENABLE_D3D12 1)
        list(APPEND ValidGraphics "D3D12")
    endif()
    if(VK_LIB)
        set(ARGON_ENABLE_VULKAN 1)
        list(APPEND ValidGraphics "VULKAN")
    endif()
    if(DX11_LIB)
        set(ARGON_ENABLE_D3D11 1)
        list(APPEND ValidGraphics "D3D11")
    endif()

elseif(PLATFORM_BACKEND STREQUAL "LINUX")
    find_library(VK_LIB vulkan)
    if(VK_LIB)
        set(ARGON_ENABLE_VULKAN 1)
        list(APPEND ValidGraphics "VULKAN")
    endif()
endif()

target_compile_definitions(Argon PUBLIC
    ENABLE_METAL=${ARGON_ENABLE_METAL}
    ENABLE_VULKAN=${ARGON_ENABLE_VULKAN}
    ENABLE_D3D12=${ARGON_ENABLE_D3D12}
    ENABLE_D3D11=${ARGON_ENABLE_D3D11}
    ENABLE_OPENGL=${ARGON_ENABLE_OPENGL}
)

message(STATUS "[Argon] Supported Graphics APIs: ${ValidGraphics}")

target_include_directories(Argon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(Stelo::Argon ALIAS Argon)
