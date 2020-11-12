cmake_minimum_required(VERSION 3.4.1)

if (NOT DEFINED APP_STYLE)
    set(APP_STYLE STATIC)
endif()

if(${APP_STYLE} MATCHES "SHARED")
    set(TARGET_NAME igeScene)
else()
    set(TARGET_NAME igeScene-static)
endif()

project(${TARGET_NAME})

set (CMAKE_CXX_STANDARD 17)

if(DEFINED ANDROID_PLATFORM)
    set (ANDROID 1)
    if(ANDROID_ABI STREQUAL "armeabi-v7a")
        add_compile_options(-marm -march=armv7-a -mtune=cortex-a15 -mfpu=neon-fp16 -mfloat-abi=softfp)
    elseif(ANDROID_ABI STREQUAL "x86")
        add_compile_options(-march=atom -m32 -msse -msse2 -msse3 -mssse3 -mfpmath=sse -mstackrealign)
    elseif(ANDROID_ABI STREQUAL "arm64-v8a")
        add_compile_options(-march=armv8-a+simd+sve -mtune=cortex-a53 -D__ARM_NEON -D__ARM_NEON__)
    elseif(ANDROID_ABI STREQUAL "x86_64")
        add_compile_options(-march=atom -m64 -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mfpmath=sse)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-g0 -Ofast)
    else(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-Og)
    endif()
elseif(MSVC)
    if(${APP_STYLE} MATCHES "SHARED")
        set(CMAKE_CXX_FLAGS "/MD ${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /Zc:inline")
        set(CMAKE_CXX_FLAGS_DEBUG "/MD /JMC /ZI /Od")
    endif()
    add_compile_options(/MP)
elseif(APPLE)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-g0 -Ofast)
    endif()
endif()

set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

set (PYTHON_INCLUDE_DIRS ${LIBS_DIR}/cpython/Include)
list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/Python/Include)
if (MSVC)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/cpython/PC)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/Python/PC)
elseif (OSX)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/cpython/Mac)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/Python/Mac)
elseif (ANDROID)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/cpython/Android)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/Python/Android)
elseif (IOS)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/cpython/IOS)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/Python/IOS)
endif ()

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PYTHON_INCLUDE_DIRS}
    ${LIBS_DIR}/pyxCore/pyxcore
    ${LIBS_DIR}/pyxCore/include/pyxcore
    ${LIBS_DIR}/pyxCore/ThirdParty/GLEW
    ${LIBS_DIR}/pyxCore/include/ThirdParty/GLEW
    ${LIBS_DIR}/pyxCore/ThirdParty/edge/include
    ${LIBS_DIR}/pyxCore/include/ThirdParty/edge/include
    ${LIBS_DIR}/pyxCore/ThirdParty/spdlog/include
    ${LIBS_DIR}/pyxCore/include/ThirdParty/spdlog/include
    ${LIBS_DIR}/tracy
    ${LIBS_DIR}/tracy/include
    ${LIBS_DIR}/json
    ${LIBS_DIR}/json/include
    ${LIBS_DIR}/igeVmath
    ${LIBS_DIR}/igeVmath/include
    ${LIBS_DIR}/igeCore
    ${LIBS_DIR}/igeCore/include
    ${LIBS_DIR}/igeScene
    ${LIBS_DIR}/igeScene/include
    ${LIBS_DIR}/igeSound
    ${LIBS_DIR}/igeSound/include
    ${LIBS_DIR}/igeSound/ThirdParty/soloud/include
    ${LIBS_DIR}/igeSound/include/ThirdParty/soloud/include
    ${LIBS_DIR}/igeEffekSeer/include/ThirdParty/Effekseer/Dev/Cpp
    ${LIBS_DIR}/igeEffekSeer/include/ThirdParty/Effekseer/Dev/Cpp/Effekseer
    ${LIBS_DIR}/igeEffekSeer/include/ThirdParty/Effekseer/Dev/Cpp/EffekseerRendererCommon
    ${LIBS_DIR}/imgui
    ${LIBS_DIR}/imgui/include
    ${LIBS_DIR}/SDL
    ${LIBS_DIR}/SDL/include
    ${LIBS_DIR}/stb
    ${LIBS_DIR}/stb/include
    ${LIBS_DIR}/igeBullet
    ${LIBS_DIR}/igeBullet/include
    ${LIBS_DIR}/igeBullet/bullet
    ${LIBS_DIR}/igeBullet/include/bullet
)

if(APPLE)
    if(IOS)
        set(INCLUDES ${INCLUDES} "${CMAKE_CURRENT_SOURCE_DIR}/platform/ios")
        file(GLOB EXTRA_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/*.mm"
        )
        set(SRC ${SRC} ${EXTRA_SOURCES})
    elseif(OSX)
        set(INCLUDES ${INCLUDES} "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos")
        file(GLOB EXTRA_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/*.mm"
            "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/*.cpp"
        )
        set(SRC ${SRC} ${EXTRA_SOURCES})
    endif()
elseif(ANDROID)
    set(INCLUDES ${INCLUDES} "${CMAKE_CURRENT_SOURCE_DIR}/platform/android")
    file(GLOB EXTRA_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/android/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/android/*.cpp"
    )
    set(SRC ${SRC} ${EXTRA_SOURCES})
elseif(MSVC)
    set(INCLUDES ${INCLUDES} "${CMAKE_CURRENT_SOURCE_DIR}/platform/win32")
    file(GLOB EXTRA_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/win32/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/win32/*.cpp"
    )
    set(SRC ${SRC} ${EXTRA_SOURCES})
endif()

add_library(${TARGET_NAME} ${APP_STYLE} ${SRC})

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDES})

# Python37
set (DEFINITIONS Py_NO_ENABLE_SHARED USE_IGE)
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(DEFINITIONS ${DEFINITIONS} _AMD64_)
else()
    set(DEFINITIONS ${DEFINITIONS} _X86_)
endif()
target_compile_definitions(${TARGET_NAME} PUBLIC ${DEFINITIONS})

if(${APP_STYLE} MATCHES "SHARED")
    set_target_properties(
        ${TARGET_NAME}
        PROPERTIES
            PREFIX ""
            OUTPUT_NAME ${TARGET_NAME}
            LINKER_LANGUAGE C
    )
    if(WIN32)
        set_target_properties(
            ${TARGET_NAME}
            PROPERTIES
            SUFFIX ".pyd"
        )
    endif()
    target_link_libraries(${TARGET_NAME} ${PYTHON_LIBRARIES})
endif()

