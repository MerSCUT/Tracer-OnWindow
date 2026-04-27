cmake_minimum_required(VERSION 3.18)
project(Tracer VERSION 1.0.0
        LANGUAGES CXX CUDA)

# Find CUDA
find_package(CUDAToolkit REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CUDA_ARCHITECTURES native)

# Path Variables
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.cc" "${SRC_DIR}/*.cxx" "${SRC_DIR}/*.cu")


add_executable(${PROJECT_NAME})

message(STATUS "Src files: ${SRC_FILES}")

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR} ${CUDAToolkit_INCLUDE_DIRS})
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE CUDA::cudart)

# WorkDir Definition
target_compile_definitions(Tracer PRIVATE 
    PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)

