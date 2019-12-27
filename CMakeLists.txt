cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_COMPILER clang++)
project(OpticsCompensation_s CXX)

find_package(OpenCL REQUIRED)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE src/optics_compensation_s.cc)
target_include_directories(${PROJECT_NAME} PRIVATE AUL_Utils/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCL_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")