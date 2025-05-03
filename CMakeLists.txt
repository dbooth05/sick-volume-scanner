cmake_minimum_required(VERSION 3.10)
project(DepthScan)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SICK_VISION_ROOT ${CMAKE_SOURCE_DIR}/external/sick_visionary_cpp_shared)
set(SICK_VISION_INC ${SICK_VISION_ROOT}/src) # header files are here

add_subdirectory(external/sick_visionary_cpp_shared)
add_executable(test src/main_test.cpp)

# include and link SICK prebuilts
target_include_directories(test PRIVATE ${SICK_VISION_INC})
target_link_libraries(test sick_visionary_cpp_shared)
target_link_directories(test PRIVATE ${SICK_VISION_ROOT}/build)
