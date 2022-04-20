APP_PROJECT_PATH := path/pen_engine/ext/platforms/android
NDK_PROJECT_PATH := C:/Users/user/AppData/Local/Android/Sdk/ndk/20.0.5594570
APP_ABI := arm64-v8a armeabi-v7a x86 x86_64
APP_MANIFEST := $(APP_PROJECT_PATH)/pen_engine_android/src/AndroidManifest.xml
APP_STL := c++_shared
APP_CPPFLAGS += -DANDROID