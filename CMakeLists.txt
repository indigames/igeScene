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

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(compiler_options)

if(MSVC)
	add_compile_options(/std:c++17)
endif()

if(${APP_STYLE} MATCHES "SHARED")
    find_package (Python3 ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Development)
else()
    include(FindPython)
    set (Python3_INCLUDE_DIRS ${Python_INCLUDE_DIRS})
    set (Python3_LIB_DIRS ${Python_LIB_DIRS})
    set (Python3_LIBRARIES ${Python_LIBRARIES})
endif()

include(Findstb)
include(Findjson)

include(igeCore)
include(igeVmath)
include(igeBullet)
include(igeEffekseer)
include(igeNavigation)
include(igeSound)

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

set(INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
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
            SUFFIX ".pyd"
            OUTPUT_NAME ${TARGET_NAME}
            LINKER_LANGUAGE C
    )

    if(MSVC)
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
    elseif(APPLE)
        target_link_libraries(${TARGET_NAME} z c++abi c++ iconv)
        find_library(AUDIOTOOLBOX AudioToolBox)
        find_library(COREAUDIO CoreAudio)
        find_library(COREBLUETOOTH CoreBluetooth)
        find_library(COREFOUNDATION CoreFoundation)
        find_library(COREVIDEO CoreVideo)
        find_library(COREHAPTICS CoreHaptics)
        find_library(GAMECONTROLLER GameController)
        find_library(OPENGL OpenGL)
        find_library(CARBON Carbon)
        find_library(COCOA Cocoa)
        find_library(IOKIT IOKit)
        find_library(FORCEFEEDBACK ForceFeedback)
        find_library(METAL Metal)
        target_link_libraries(${TARGET_NAME} 
            ${AUDIOTOOLBOX}
            ${COREAUDIO}
            ${COREBLUETOOTH}
            ${COREFOUNDATION}
            ${COREVIDEO}
            ${COREHAPTICS}
            ${GAMECONTROLLER}
            ${OPENGL}
            ${CARBON}
            ${COCOA}
            ${IOKIT}
            ${FORCEFEEDBACK}
            ${METAL}
        )
    endif()
    file(GLOB DLLS "${PVRTEXLIB_LIB_DIRS}/*.d*")
    foreach(dll IN LISTS DLLS)
        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" $<TARGET_FILE_DIR:${TARGET_NAME}>
        )
        if(OSX)
            get_filename_component(DYLIB ${dll} NAME)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_INSTALL_NAME_TOOL} -id @loader_path/${DYLIB} $<TARGET_FILE_DIR:${TARGET_NAME}>/${DYLIB})
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change @executable_path/../lib/${DYLIB} @loader_path/${DYLIB} $<TARGET_FILE:${TARGET_NAME}>)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change @executable_path/${DYLIB} @loader_path/${DYLIB} $<TARGET_FILE:${TARGET_NAME}>)
        endif()
    endforeach()
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