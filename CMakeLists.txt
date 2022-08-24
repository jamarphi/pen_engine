cmake_minimum_required(VERSION 3.0)

#Initializes the project
project(PEN_ENGINE)

#Sets the absolute path for asset management
configure_file(abs_asset_dir_config.h.in abs_asset_dir_config.h)
include_directories(${CMAKE_BINARY_DIR})

#Sets the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/")

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Android")
#For PC builds
if(NOT APPLE)
find_package(OpenGL REQUIRED)
endif()
endif()

message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "OPENGL_FOUND: ${OPENGL_FOUND}")
message(STATUS "OPENGL_glx_LIBRARY: ${OPENGL_glx_LIBRARY}")
message(STATUS "OPENGL_INCLUDE_DIR: ${OPENGL_INCLUDE_DIR}")
message(STATUS "OPENGL_gl_LIBRARY: ${OPENGL_gl_LIBRARY}")
message(STATUS "OPENGL_opengl_LIBRARY: ${OPENGL_opengl_LIBRARY}")
message(STATUS "OPENGL_GLX_INCLUDE_DIR: ${OPENGL_GLX_INCLUDE_DIR}")
message(STATUS "OPENGL_glu_LIBRARY: ${OPENGL_glu_LIBRARY}")
message(STATUS "OpenGL_GLX_FOUND: ${OpenGL_GLX_FOUND}")

set(NON_LINUX OFF)

if(WIN32)
#glfw gets added via the local lib
set(NON_LINUX ON)
add_library(glfw STATIC IMPORTED)

#This is for setting the glfw static lib
set_property(TARGET glfw PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-win/x64/glfw3.lib")
set_property(TARGET glfw PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/glfw")

#Sets the linker language
set_target_properties(glfw PROPERTIES LINKER_LANGUAGE CXX)
endif()

if(APPLE)
set(NON_LINUX ON)
endif()

if(NOT NON_LINUX)
#glfw gets added in as a cmake module
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
        set(glfw3_ROOT "/usr/lib/x86_64-linux-gnu")
        find_package(glfw3 3.3 REQUIRED)
    endif()
endif()

#Project sources for the executable
file(GLOB_RECURSE PROJECT_LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/*.h" "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/dependencies/*.h" "${CMAKE_SOURCE_DIR}/dependencies/*.cpp"
    "${CMAKE_SOURCE_DIR}/ext/ai/*.h" "${CMAKE_SOURCE_DIR}/ext/ai/*.cpp"
    "${CMAKE_SOURCE_DIR}/ext/misc/*.h" "${CMAKE_SOURCE_DIR}/ext/misc/*.cpp"
    "${CMAKE_SOURCE_DIR}/ext/platforms/mac_ios/*.h" "${CMAKE_SOURCE_DIR}/ext/platforms/mac_ios/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c")

#Creates the .so library for Android
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
add_subdirectory(${CMAKE_SOURCE_DIR}/ext/platforms/android)
endif()

#Creates an executable
if(NOT APPLE)
add_executable(${PROJECT_NAME} ${PROJECT_LIB_SOURCES} "${CMAKE_SOURCE_DIR}/../app.cpp")
else()
add_executable(${PROJECT_NAME} ${PROJECT_LIB_SOURCES} "${CMAKE_SOURCE_DIR}/ext/platforms/mac_ios/mac_ios_manager.mm")
endif()

#Sets the C++ version now that the project is created
if(APPLE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
else()
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
endif()

#The library directories for linkage
if(WIN32)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${OPENGL_LIBRARIES})
endif()

if(APPLE)
    if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    #For Mac builds
    find_library(FOUNDATION Foundation REQUIRED)
    find_library(QUARTZ QuartzCore REQUIRED)
    find_library(METAL Metal REQUIRED)
    find_library(METALKIT MetalKit REQUIRED)
    find_library(COREBLUETOOTH CoreBluetooth REQUIRED)
    find_library(AVFAUDIO AVFAudio REQUIRED)
    add_library(sqlite3 SHARED IMPORTED)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    find_library(APPKIT UIKit REQUIRED)
    find_library(COREMOTION CoreMotion REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${FOUNDATION} ${QUARTZ} ${METAL} ${METALKIT}
        ${APPKIT} ${COREBLUETOOTH} ${AVFAUDIO} ${COREMOTION} ${sqlite3})
    else()
    find_library(APPKIT AppKit REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${FOUNDATION} ${QUARTZ} ${METAL} ${METALKIT}
        ${APPKIT} ${COREBLUETOOTH} ${AVFAUDIO} ${sqlite3})
    endif()
    
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        target_link_libraries(${PROJECT_NAME} GLESv2 EGL)
    endif()
endif()

if(NOT NON_LINUX)
    if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    #For Linux builds
    target_link_libraries(glfw INTERFACE rt)
    target_link_libraries(glfw INTERFACE m)
    target_link_libraries(glfw INTERFACE dl)
    target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${OPENGL_LIBRARIES})
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    target_link_libraries(${PROJECT_NAME} GLESv2 EGL)
    endif()
endif()
