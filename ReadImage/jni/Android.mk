LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#OPENCV_CAMERA_MODULES:=off
#OPENCV_INSTALL_MODULES:=off
#OPENCV_LIB_TYPE:=SHARED
OPENCV_LIB_TYPE:=STATIC
include /home/first_volume/usr/opencv_android/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES  := read_image_jni.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS     += -llog -ldl

LOCAL_MODULE     := read_image_jni

include $(BUILD_SHARED_LIBRARY)
