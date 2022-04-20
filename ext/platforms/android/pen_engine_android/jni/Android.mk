#Paths go one directory outside of pen_engine to look for any source files
PC_INT_MAIN := app.cpp
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
recursive_wildcard=$(foreach d,$(wildcard $1*),$(call recursive_wildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
CPP_SRC_FILES := $(call recursive_wildcard, ../../../../../, *.cpp *.c)
CPP_SRC_FILES_FORMATTED := $(CPP_SRC_FILES:$(LOCAL_PATH)/%=%)
CPP_SRC_FILES_FORMATTED := $(addprefix ../, $(CPP_SRC_FILES_FORMATTED))
CPP_SRC_FILES_FORMATTED := $(filter-out ../../../../../../$(PC_INT_MAIN), $(CPP_SRC_FILES_FORMATTED))
CPP_SRC_FILES_FORMATTED := $(filter-out ../../../../../../pen_engine/examples/example.cpp, $(CPP_SRC_FILES_FORMATTED))
CPP_SRC_FILES_FORMATTED := $(filter-out ../../../../../../pen_engine/examples/example_gui.cpp, $(CPP_SRC_FILES_FORMATTED))
CPP_SRC_FILES_FORMATTED := $(filter-out ../../../../../../pen_engine/examples/max_grid_area.cpp, $(CPP_SRC_FILES_FORMATTED))
CPP_SRC_FILES_FORMATTED := $(filter-out ../../../../../../pen_engine/examples/wave_algorithm.cpp, $(CPP_SRC_FILES_FORMATTED))
LOCAL_SRC_FILES := $(CPP_SRC_FILES_FORMATTED)
LOCAL_MODULE := pen_engine_android
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../..
LOCAL_CPPFLAGS += -std=c++14 -frtti -fexceptions
LOCAL_CPP_EXTENSION := .cxx .cpp .cc .c
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_LDLIBS += -lGLESv2 -lEGL -landroid
LOCAL_SHORT_COMMANDS := true
include $(BUILD_SHARED_LIBRARY)