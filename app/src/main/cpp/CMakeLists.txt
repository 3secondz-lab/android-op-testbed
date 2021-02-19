cmake_minimum_required(VERSION 3.10.2)

project("op_testbed")

# opencv
include_directories(${OpenCV_DIR}/jni/include)
add_library(lib_opencv SHARED IMPORTED)
set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION
        ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_java4.so)

add_library(native-lib SHARED
        native-lib.cpp
        opencv.cpp)

find_library(
        log-lib
        log )

target_link_libraries(native-lib
        lib_opencv
        ${log-lib})