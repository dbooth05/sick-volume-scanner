cmake_minimum_required(VERSION 3.10)
project(DepthScan)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SICK_VISION_ROOT ${CMAKE_SOURCE_DIR}/external/sick_visionary_cpp_shared)
set(SICK_VISION_LIB ${CMAKE_SOURCE_DIR}/external/libsick_visionary_cpp_shared.so)
set(SICK_VISION_INC ${SICK_VISION_ROOT}/src) # header files are here

add_executable(test src/main_test.cpp)

# include and link SICK prebuilts
target_include_directories(test PRIVATE ${SICK_VISION_INC})
target_link_libraries(test PRIVATE ${SICK_VISION_LIB})
target_link_directories(test PRIVATE ${SICK_VISION_ROOT}/build)
