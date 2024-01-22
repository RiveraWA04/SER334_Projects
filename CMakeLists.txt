cmake_minimum_required(VERSION 3.7)
project(BMPImageProcessor)

set(CMAKE_C_STANDARD 99)


include_directories(src cmake-build-debug)


add_executable(BMPImageProcessor
        BMPHandler.c
        Image.c
        RiveraRosalesImageProcessor.c
)

