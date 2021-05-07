cmake_minimum_required(VERSION 3.15.0)

if (NOT DEFINED APP_STYLE)
    set(APP_STYLE STATIC)
endif()

if(${APP_STYLE} MATCHES "SHARED")
    set(TARGET_NAME igeScene)
    if (NOT DEFINED PYTHON_VERSION)
        set(PYTHON_VERSION 3.7)
    endif()
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
    add_compile_options(/MP /std:c++17)
elseif(APPLE)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-g0 -Ofast)
    endif()
endif()

if(${APP_STYLE} MATCHES "SHARED")
    find_package (Python3 ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Development)
else()
    set (Python3_INCLUDE_DIRS "")
endif()

# [IGE]: try include conan build config
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

    # Do not use python from conan for shared
    if(${APP_STYLE} MATCHES "SHARED")
        set(CONAN_INCLUDE_DIRS_PYTHON "")
        set(CONAN_LIBS_PYTHON "")
        set(CONAN_PKG_LIBS_PYTHON "")
        set(CONAN_LIBS_SDL "SDL2")
    endif()

    conan_basic_setup()

    list(GET CONAN_INCLUDE_DIRS_PYXCORE 0 PYXCORE_INCLUDE_DIRS)
endif()
# [/IGE]

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
set (EXCLUDE_DIR
    "/build"
    "/example"
    "/bin"
    "/misc"
    "/docs"
)
foreach (TMP_PATH ${SRC})
    foreach (TMP_DIR ${EXCLUDE_DIR})
        string (FIND ${TMP_PATH} ${TMP_DIR} EXCLUDE_DIR_FOUND)
        if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
            list (REMOVE_ITEM SRC ${TMP_PATH})
        endif ()
    endforeach(TMP_DIR)
endforeach(TMP_PATH)

set(INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${Python3_INCLUDE_DIRS}"
    "${CONAN_INCLUDE_DIRS}"
    "${PYXCORE_INCLUDE_DIRS}/pyxcore"
    "${PYXCORE_INCLUDE_DIRS}/ThirdParty/GLEW"
)

add_library(${TARGET_NAME} ${APP_STYLE} ${SRC})

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDES})

target_compile_definitions(${TARGET_NAME} PRIVATE ${DEFINES} ${CONAN_DEFINES})

if(${APP_STYLE} MATCHES "STATIC")
    target_compile_definitions(${TARGET_NAME} PRIVATE Py_NO_ENABLE_SHARED)
endif()

if(${APP_STYLE} MATCHES "SHARED")
    target_compile_definitions(${TARGET_NAME} PRIVATE GHC_WITH_EXCEPTIONS)

    set_target_properties(
        ${TARGET_NAME}
        PROPERTIES
            PREFIX ""
            OUTPUT_NAME ${TARGET_NAME}
            LINKER_LANGUAGE C
    )

    if(MSVC)
        set_target_properties(
            ${TARGET_NAME}
            PROPERTIES
            SUFFIX ".pyd"
        )
    endif()

    target_link_libraries(${TARGET_NAME}
      ${CONAN_LIBS}
      ${Python3_LIBRARIES}
      user32
      Gdi32
      Crypt32
      dinput8
      dxguid
      winmm
      imm32
      ole32
      oleaut32
      shell32
      version
      uuid
      Setupapi
    )
endif()

