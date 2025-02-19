cmake_minimum_required(VERSION 3.22)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

project(unk_engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

set(ASSETS_FOLDER ${CMAKE_SOURCE_DIR}/assets)

# ios and web
if ((APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin") OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")    # Disable shared builds on platforms where it does not make sense to use them
    set(SDL_SHARED OFF)
else()
    set(SDL_SHARED ON)
endif()

if(MSVC)
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)				# parallelize each target, unless Ninja is the generator
    endif()
endif()


file(GLOB_RECURSE SOURCES "src/*.cpp")

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

set(SDLTTF_VENDORED ON) # build own deps.
add_subdirectory(vendor/SDL_ttf EXCLUDE_FROM_ALL)

set(SDLMIXER_VENDORED ON) # build own deps.
set(SDLMIXER_MIDI_NATIVE OFF)  # disabled
set(SDLMIXER_GME OFF)
set(SDLMIXER_WAVPACK OFF)     
set(SDLMIXER_MOD OFF)
set(SDLMIXER_OPUS OFF)
set(SDLMIXER_MP3 ON)
set(SDLMIXER_FLAC ON)
add_subdirectory(vendor/SDL_mixer EXCLUDE_FROM_ALL)

set(SDLIMAGE_VENDORED ON) # build own deps.
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_JPEG OFF)
set(SDLIMAGE_BMP OFF)
set(SDLIMAGE_WEBP OFF)
set(SDLIMAGE_PNG ON)
set(SDLIMAGE_GIF ON)
set(SDLIMAGE_SVG ON)
add_subdirectory(vendor/SDL_image EXCLUDE_FROM_ALL)



if(EMSCRIPTEN)
	set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-narrowing")
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
    target_link_libraries(${PROJECT_NAME} PUBLIC android log GLESv2 m)

endif()



# ios
if(APPLE AND NOT BUILD_SHARED_LIBS)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES})
    find_library(IO_LIB ImageIO REQUIRED)
    find_library(CS_LIB CoreServices REQUIRED)
    find_library(CT_LIB CoreText REQUIRED)
    find_library(CG_LIB CoreGraphics REQUIRED)
    find_library(CF_LIB CoreFoundation REQUIRED)
    find_library(OPENGLES OpenGLES REQUIRED)
    find_library(UIKIT UIKit)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CF_LIB} ${CT_LIB} ${IO_LIB} ${CS_LIB} ${CG_LIB} ${OPENGLES} ${UIKIT})

    set(LAUNCH_SCREEN "${CMAKE_SOURCE_DIR}/templates/ios/LaunchScreen.storyboard")
    set(ICON_NAME "SAppLogo.icns")
    set(ICON_PATH "${CMAKE_SOURCE_DIR}/templates/ios/${ICON_NAME}")
    set(RESOURCES ${LAUNCH_SCREEN} "${CMAKE_SOURCE_DIR}/assets")

    set_target_properties(${PROJECT_NAME} PROPERTIES 
    MACOSX_BUNDLE TRUE
    RESOURCE "${RESOURCES}"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/templates/ios/Info.plist.in"
    XCODE_IPHONEOS_DEPLOYMENT_TARGET 12.0
    XCODE_GENERATE_SCHEME TRUE
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES YES
    XCODE_ATTRIBUTE_CODE_SIGN_STYLE “Automatic”
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.unkengine.app.project"
	XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.unkengine.app.project"
	XCODE_ATTRIBUTE_CURRENTYEAR "${CURRENTYEAR}")

    if(CMAKE_OSX_SYSROOT MATCHES "iphonesimulator")
        message(STATUS "SIMULATOR BUILD")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO"
            XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "iphonesimulator"
            XCODE_ATTRIBUTE_ARCHS "x86_64 arm64")
    else()
        message(STATUS "REAL DEVICE BUILD")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO"
            XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "iphoneos"
            XCODE_ATTRIBUTE_ARCHS "arm64")
    endif()

endif()

# macosx
if(APPLE AND BUILD_SHARED_LIBS)
    message(STATUS "BUILD MACOS")
    set(RESOURCES "${CMAKE_SOURCE_DIR}/assets")

    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${RESOURCES} )

    set_target_properties(${PROJECT_NAME} PROPERTIES 
    MACOSX_BUNDLE TRUE
    RESOURCE "${RESOURCES}")
    target_link_libraries(${PROJECT_NAME} PRIVATE objc stdc++)
 

endif()


if(MSVC)
    add_executable(${PROJECT_NAME} ${SOURCES})

    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

endif()



if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_executable(${PROJECT_NAME} ${SOURCES})

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-narrowing")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")

    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)


endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/public)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_mixer::SDL3_mixer SDL3_image::SDL3_image)

target_compile_definitions(${PROJECT_NAME} PUBLIC SDL_MAIN_USE_CALLBACKS)
