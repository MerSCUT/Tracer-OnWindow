cmake_minimum_required(VERSION 3.18)
project(Tracer VERSION 1.0.0
        LANGUAGES CXX CUDA)

# Find CUDA
find_package(CUDAToolkit REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CUDA_ARCHITECTURES native)
set(CMAKE_CUDA_STANDARD 17)


# Path Variables
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.cc" "${SRC_DIR}/*.cxx" "${SRC_DIR}/*.cu")


add_executable(${PROJECT_NAME})

if(MSVC)
    target_compile_options(Tracer PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:/utf-8>

        $<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=/utf-8>
    )
endif()

message(STATUS "Src files: ${SRC_FILES}")

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR} ${CUDAToolkit_INCLUDE_DIRS})
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES} "include/tiny_obj_loader.h")
target_link_libraries(${PROJECT_NAME} PRIVATE CUDA::cudart)

# WorkDir Definition
target_compile_definitions(Tracer PRIVATE 
    PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)


