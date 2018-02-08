cmake_minimum_required(VERSION 3.4.1)



find_library(
              log-lib
              log )

add_library(lib_chromaprint SHARED IMPORTED)
set_target_properties(lib_chromaprint PROPERTIES IMPORTED_LOCATION
${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libchromaprint.so)

add_library(lib_avfilter STATIC IMPORTED)
set_target_properties(lib_avfilter PROPERTIES IMPORTED_LOCATION
${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavfilter.a)

add_library(lib_avcodec STATIC IMPORTED)
set_target_properties(lib_avcodec PROPERTIES IMPORTED_LOCATION
${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavcodec.a)

add_library(lib_avformat STATIC IMPORTED)
set_target_properties(lib_avformat PROPERTIES IMPORTED_LOCATION
${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavformat.a)

add_library(lib_avutil STATIC IMPORTED)
set_target_properties(lib_avutil PROPERTIES IMPORTED_LOCATION
${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavutil.a)

add_library(lib_postproc STATIC IMPORTED)
set_target_properties(lib_postproc PROPERTIES IMPORTED_LOCATION
${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libpostproc.a)

add_library(lib_swresample STATIC IMPORTED)
set_target_properties(lib_swresample PROPERTIES IMPORTED_LOCATION
${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libswresample.a)

add_library(
             fpcalc
             SHARED
             src/main/cpp/fpcalc.cpp )

target_include_directories(fpcalc PRIVATE ${CMAKE_SOURCE_DIR}/libs/include)

target_link_libraries( fpcalc
                       log
                       lib_avcodec
                       lib_avfilter
                       lib_avformat
                       lib_avutil
                       lib_postproc
                       lib_swresample
                       lib_chromaprint)