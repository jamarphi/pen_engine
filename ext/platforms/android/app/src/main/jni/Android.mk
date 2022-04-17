#This path goes one level outside of pen_engine
LOCAL_PATH := $(call my-dir)/../../../../../../../../
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/*.cpp) $(wildcard $(LOCAL_PATH)/**/*.cpp)
LOCAL_MODULE := "app"
LOCAL_MODULE_FILENAME := "PEN_ENGINE_ANDROID_LIB"
LOCAL_C_INCLUDES := $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/**/*.c)
LOCAL_LDLIBS := -lGLESv2 -lEGL -landroid
include $(BUILD_SHARED_LIBRARY)
