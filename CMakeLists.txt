cmake_minimum_required(VERSION 3.20)
project(filters CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(image_processor
    main.cpp
    bmp.cpp
    image.cpp
    factory.cpp
    controller.cpp
    ./all_filters/crop_filter.cpp
    ./all_filters/negative_filter.cpp
    ./all_filters/pixel_filter.cpp
    ./all_filters/grayscale_filter.cpp
    ./all_filters/edge_filter.cpp
    ./all_filters/matrix_filter.cpp
    ./all_filters/cizoomblur_filter.cpp
    ./all_filters/gaussian_filter.cpp
    ./all_filters/sharpening_filter.cpp
)
target_compile_options(image_processor PRIVATE -Wall -Wextra -Wpedantic)
