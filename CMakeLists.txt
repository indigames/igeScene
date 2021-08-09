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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Modules and install path (required by conan)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

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
    find_package(Python REQUIRED)
    set (Python3_INCLUDE_DIRS ${Python_INCLUDE_DIRS})
    set (Python3_LIBRARIES ${Python_LIBRARIES})
endif()

find_package(taskflow REQUIRED)
find_package(json REQUIRED)
find_package(stb REQUIRED)
find_package(SDL REQUIRED)
find_package(numpy REQUIRED)
find_package(pyxCore REQUIRED)
find_package(igeCore REQUIRED)
find_package(bullet REQUIRED)
find_package(igeSound REQUIRED)
find_package(igeEffekseer REQUIRED)
find_package(igeNavigation REQUIRED)


file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
set (EXCLUDE_DIR
    "/_ige_build"
    "/_ige_release"
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
    "${numpy_INCLUDE_DIRS}"
    "${bullet_INCLUDE_DIRS}"
    "${igeBullet_INCLUDE_DIRS}"
    "${igeVmath_INCLUDE_DIRS}"
    "${taskflow_INCLUDE_DIRS}"
    "${stb_INCLUDE_DIRS}"
    "${json_INCLUDE_DIRS}"
    "${pyxCore_INCLUDE_DIRS}"
    "${pyxCore_INCLUDE_DIRS}/pyxcore"
    "${pyxCore_INCLUDE_DIRS}/ThirdParty/GLEW"
    "${igeCore_INCLUDE_DIRS}"
    "${igeSound_INCLUDE_DIRS}"
    "${igeSound_INCLUDE_DIRS}/ThirdParty/soloud/include"
    "${igeEffekseer_INCLUDE_DIRS}"
    "${igeEffekseer_INCLUDE_DIRS}/ThirdParty/Effekseer/Dev/Cpp"
    "${igeEffekseer_INCLUDE_DIRS}/ThirdParty/Effekseer/Dev/Cpp/Effekseer"
    "${igeNavigation_INCLUDE_DIRS}"
    "${igeNavigation_INCLUDE_DIRS}/3rd-party/recast/Detour/Include"
    "${igeNavigation_INCLUDE_DIRS}/3rd-party/recast/DetourCrowd/Include"
    "${igeNavigation_INCLUDE_DIRS}/3rd-party/recast/DetourTileCache/Include"
    "${igeNavigation_INCLUDE_DIRS}/3rd-party/recast/Recast/Include"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC})
add_library(${TARGET_NAME} ${APP_STYLE} ${SRC})
target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDES})
target_compile_definitions(${TARGET_NAME} PRIVATE ${DEFINES} ${pyxCore_DEFINES})


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
      ${Python3_LIBRARIES}
      ${pyxCore_LIBRARIES}
      ${igeCore_LIBRARIES}
      ${numpy_LIBRARIES}
      ${tracy_LIBRARIES}
      ${bullet_LIBRARIES}
      ${igeBullet_LIBRARIES}
      ${igeVmath_LIBRARIES}
      ${stb_LIBRARIES}
      ${SDL_LIBRARIES}
      ${igeSound_LIBRARIES}
      ${igeEffekseer_LIBRARIES}
      ${igeNavigation_LIBRARIES}
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

file(GLOB_RECURSE PUBLIC_INCLUDE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
foreach(HEADER ${PUBLIC_INCLUDE_FILES})
    file(RELATIVE_PATH HEADER_RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${HEADER}")
    get_filename_component(DIR ${HEADER_RELATIVE} DIRECTORY)
    install(FILES ${HEADER} DESTINATION include/${DIR})
endforeach()