LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


OPENCV_LIB_TYPE:=STATIC
include /home/first_volume/usr/opencv_android/sdk/native/jni/OpenCV.mk

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS     += -llog -ldl

LOCAL_SRC_FILES  := just_scratch.cpp
LOCAL_MODULE     := just_scratch

include $(BUILD_SHARED_LIBRARY)
