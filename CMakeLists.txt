cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(OpticsCompensation_s CXX)

find_package(OpenCL REQUIRED)

set(LUA_INCLUDE_DIR "/" CACHE PATH "Lua include dir")
set(LUA_LIBRARY_DIR "/" CACHE PATH "Lua include dir")

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE src/optics_compensation_s.cc)
target_sources(${PROJECT_NAME} PRIVATE src/cl_manager.cc)
target_sources(${PROJECT_NAME} PRIVATE src/cl_kernel.cc)
target_sources(${PROJECT_NAME} PRIVATE src/cpu_kernel.cc)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE AUL_Utils/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_directories(${PROJECT_NAME} PRIVATE ${LUA_LIBRARY_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCL_LIBRARIES} ${OpenCV_LIBS} lua51)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /source-charset:utf-8
        $<IF:$<CONFIG:Debug>,
            /MTd,
            /MT /Ox
        >
        /openmp:experimental
        /EHa
        /MP
        /wd4018
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        /DELAYLOAD:OpenCL.dll
        /ignore:4099
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -stdlib=libc++ -m32)
    target_link_options(${PROJECT_NAME} PRIVATE -m32)
endif()

# Disable DLL name prefix("lib")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

# Install DLL and script
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /)
install(FILES script/${PROJECT_NAME}.anm DESTINATION /)