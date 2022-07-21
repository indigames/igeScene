cmake_minimum_required(VERSION 3.10.2)

if (NOT DEFINED APP_STYLE)
    set(APP_STYLE STATIC)
endif()

if(${APP_STYLE} MATCHES "SHARED")
    set(TARGET_NAME igeScene)
    if (NOT DEFINED PYTHON_VERSION)
        set(PYTHON_VERSION 3.9)
    endif()
else()
    set(TARGET_NAME igeScene-static)
endif()

project(${TARGET_NAME})

OPTION(USE_PREBUILT_LIBS "Use prebuilt libraries" ON) # Enabled by default

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Modules and install path (required by conan)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

# [IGE]: tweaks for platforms
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
    add_compile_options(/MP)
elseif(IOS)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-g0 -Ofast)
    endif()
elseif(EMSCRIPTEN)
    add_compile_options(-m32 -pthread -s USE_PTHREADS=1)
endif()
# [/IGE]

if(${APP_STYLE} MATCHES "SHARED")
    find_package (Python3 ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Development)
else()
    include(FindPython)
    set (Python3_INCLUDE_DIRS ${Python_INCLUDE_DIRS})
    set (Python3_LIB_DIRS ${Python_LIB_DIRS})
    set (Python3_LIBRARIES ${Python_LIBRARIES})
endif()

include(igeCore)
include(igeVmath)
include(igeBullet)
include(igeEffekseer)
include(igeNavigation)
include(igeSound)

include(Findtaskflow)
include(Findstb)
include(Findjson)


file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

set(INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${taskflow_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "${igeBullet_INCLUDE_DIRS}"
    "${igeCore_INCLUDE_DIRS}"
    "${pyxCore_INCLUDE_DIRS}"
    "${igeVmath_INCLUDE_DIRS}"
    "${igeSound_INCLUDE_DIRS}"
    "${igeEffekseer_INCLUDE_DIRS}"
    "${igeNavigation_INCLUDE_DIRS}"
    "${stb_INCLUDE_DIRS}"
    "${json_INCLUDE_DIRS}"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC})
add_library(${TARGET_NAME} ${APP_STYLE} ${SRC})
target_include_directories(${TARGET_NAME} PUBLIC ${INCLUDES})
target_compile_definitions(${TARGET_NAME} PUBLIC ${DEFINES} ${pyxCore_DEFINES})

target_link_directories(${TARGET_NAME} PUBLIC
    ${Python3_LIB_DIRS}
    ${igeCore_LIB_DIRS}
    ${pyxCore_LIB_DIRS}
    ${igeVmath_LIB_DIRS}
    ${igeBullet_LIB_DIRS}
    ${igeEffekseer_LIB_DIRS}
    ${igeNavigation_LIB_DIRS}
    ${igeSound_LIB_DIRS}
    ${stb_LIB_DIRS}
)

target_link_libraries(${TARGET_NAME}
    ${Python3_LIBRARIES}
    ${igeCore_LIBRARIES}
    ${pyxCore_LIBRARIES}
    ${igeVmath_LIBRARIES}
    ${igeBullet_LIBRARIES}
    ${igeEffekseer_LIBRARIES}
    ${igeNavigation_LIBRARIES}
    ${igeSound_LIBRARIES}
    ${stb_LIBRARIES}
)

if(IOS OR ANDROID)
    target_compile_definitions(${TARGET_NAME} PRIVATE __EFFEKSEER_RENDERER_GLES3__)
endif()

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

# Install Targets
install(TARGETS
    ${TARGET_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

file(GLOB_RECURSE PUBLIC_INCLUDE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*")
foreach(HEADER ${PUBLIC_INCLUDE_FILES})
    file(RELATIVE_PATH HEADER_RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${HEADER}")
    get_filename_component(DIR ${HEADER_RELATIVE} DIRECTORY)
    install(FILES ${HEADER} DESTINATION include/${DIR})
endforeach()