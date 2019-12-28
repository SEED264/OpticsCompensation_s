cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_COMPILER clang++)
project(OpticsCompensation_s CXX)

find_package(OpenCL REQUIRED)

set(LUA_INCLUDE_DIR "/" CACHE PATH "Lua include dir")
set(LUA_LIBRARY_DIR "/" CACHE PATH "Lua include dir")

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE src/optics_compensation_s.cc)

target_include_directories(${PROJECT_NAME} PRIVATE AUL_Utils/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})

target_link_directories(${PROJECT_NAME} PRIVATE ${LUA_LIBRARY_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCL_LIBRARIES} lua51)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /source-charset:utf-8 /Ox)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -stdlib=libc++ -m32)
    target_link_options(${PROJECT_NAME} PRIVATE -m32)
endif()

# Disable DLL name prefix("lib")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

# Install DLL and script
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /)
install(FILES script/${PROJECT_NAME}.anm DESTINATION /)