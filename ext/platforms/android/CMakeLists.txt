#Add android mobile lib
file(GLOB_RECURSE PROJECT_MOBILE_LIB_SOURCES "../../../src/*.h" "../../../src/*.cpp"
    "../../../ext/ai/*.h"
    "../../../ext/ai/*.cpp"
    "../../../ext/platforms/android/*.h"
    "../../../ext/platforms/android/*.cpp"
    "../../../dependencies/*.h" "../../../dependencies/*.cpp"
    "../../../src/*.c")

add_library(pen_android SHARED ${PROJECT_MOBILE_LIB_SOURCES})
target_link_libraries(pen_android GLESv2 EGL android)
