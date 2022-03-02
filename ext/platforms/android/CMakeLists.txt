#Add android mobile lib
file(GLOB_RECURSE PROJECT_MOBILE_LIB_SOURCES "../../../src/*.h" "../../../src/*.cpp"
    "../../../ext/platforms/android/*.h"
        "../../../ext/platforms/android/app/src/cpp/function_mapping.cpp"
        "../../../ext/platforms/android/app/src/cpp/input_mapping.cpp"
        "../../../ext/platforms/android/app/src/cpp/pen_helper.cpp"
    "../../../dependencies/*.h" "../../../dependencies/*.cpp"
    "../../../src/*.c")

add_library(PEN_ENGINE_ANDROID_LIB SHARED ${PROJECT_MOBILE_LIB_SOURCES})
target_link_libraries(PEN_ENGINE_ANDROID_LIB GLESv2 EGL)
