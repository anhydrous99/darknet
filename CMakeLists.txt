cmake_minimum_required(VERSION 3.12.4)

project(darknet)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(USE_OpenCV "Enable OpenCV" OFF)
option(USE_OpenMP "Enable OpenMP" ON)
option(BUILD_STATIC "Build static library" ON)
option(BUILD_SHARED "Build shared library" ON)

include(CheckLanguage)
check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
    option(USE_CUDA "Enable CUDA" ON)
    option(USE_CUDNN "Enable CUDNN" ON)
    if (USE_CUDA)
        enable_language(CUDA)
        add_compile_definitions(GPU)
        if (USE_CUDNN)
            add_compile_definitions(CUDNN)
        endif ()
    endif ()
endif ()

if (USE_OpenCV)
    find_package(OpenCV)
    add_compile_definitions(OPENCV)
endif ()

if (USE_OpenMP)
    find_package(OpenMP)
endif ()

find_package(Threads)

set(SOURCES src/gemm.c src/utils.c src/cuda.c src/deconvolutional_layer.c src/convolutional_layer.c
        src/list.c src/image.c src/activations.c src/im2col.c src/col2im.c src/blas.c src/crop_layer.c
        src/dropout_layer.c src/maxpool_layer.c src/softmax_layer.c src/data.c src/matrix.c
        src/network.c src/connected_layer.c src/cost_layer.c src/parser.c src/option_list.c
        src/detection_layer.c src/route_layer.c src/upsample_layer.c src/box.c
        src/normalization_layer.c src/avgpool_layer.c src/layer.c src/local_layer.c
        src/shortcut_layer.c src/logistic_layer.c src/activation_layer.c src/rnn_layer.c
        src/gru_layer.c src/crnn_layer.c src/demo.c src/batchnorm_layer.c src/region_layer.c
        src/reorg_layer.c src/tree.c src/lstm_layer.c src/l2norm_layer.c src/yolo_layer.c
        src/iseg_layer.c src/image_opencv.cpp)

set(SOURCES_EXEC examples/captcha.c examples/lsd.c examples/super.c examples/art.c examples/tag.c
        examples/cifar.c examples/go.c examples/rnn.c examples/segmenter.c examples/regressor.c
        examples/classifier.c examples/coco.c examples/yolo.c examples/detector.c examples/nightmare.c
        examples/instance-segmenter.c examples/darknet.c)

if (USE_CUDA)
    set(SOURCES ${SOURCES} src/convolutional_kernels.cu src/deconvolutional_kernels.cu src/activation_kernels.cu
            src/im2col_kernels.cu src/col2im_kernels.cu src/blas_kernels.cu src/crop_layer_kernels.cu
            src/dropout_layer_kernels.cu src/maxpool_layer_kernels.cu src/avgpool_layer_kernels.cu)
    include_directories(${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})

    if (USE_CUDNN)
        find_library(CUDNN_LIB NAMES cudnn PATHS ${CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES})
    endif ()
    find_library(CUBLAS_LIB NAMES cublas PATHS ${CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES})
    find_library(CURAND_LIB NAME curand PATHS ${CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES})
    set(CUDA_LIBS ${CUBLAS_LIB} ${CURAND_LIB} ${CUDNN_LIB})
endif ()

include_directories(include src ${OpenCV_INCLUDE_DIRS})

add_library(obj_darknet OBJECT ${SOURCES})

add_executable(darknet $<TARGET_OBJECTS:obj_darknet> ${SOURCES_EXEC})
target_link_libraries(darknet ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBRARIES})

if (USE_CUDA)
    set_target_properties(obj_darknet PROPERTIES
            CUDA_SEPERABLE_COMPILATION ON
            POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(darknet ${CUDA_LIBS})
endif ()

if (BUILD_STATIC)
    add_library(static_darknet STATIC $<TARGET_OBJECTS:obj_darknet>)
    set_target_properties(static_darknet PROPERTIES OUTPUT_NAME "darknet")
endif ()

if (BUILD_SHARED)
    add_library(shared_darknet SHARED $<TARGET_OBJECTS:obj_darknet>)
    set_target_properties(shared_darknet PROPERTIES OUTPUT_NAME "darknet")
endif ()
