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
find_package(OpenGL REQUIRED)
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

if(WIN32)
#glfw gets added via the local lib
add_library(glfw STATIC IMPORTED)

#This is for setting the glfw static lib
set_property(TARGET glfw PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-vc2019/x64/glfw3.lib")
set_property(TARGET glfw PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/glfw")

#Sets the linker language
set_target_properties(glfw PROPERTIES LINKER_LANGUAGE CXX)

else()
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
    "${CMAKE_SOURCE_DIR}/src/*.c")

#Creates the .so library for Android
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
add_subdirectory(${CMAKE_SOURCE_DIR}/ext/platforms/android)
endif()

#Creates an executable
add_executable(${PROJECT_NAME} ${PROJECT_LIB_SOURCES} "../app.cpp")

#Sets the C++ version now that the project is created
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

#The library directories for linkage
if(WIN32)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${OPENGL_LIBRARIES})
else()
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
