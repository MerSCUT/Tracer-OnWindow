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
# 针对 Tracer 目标精确设置编码，分离 C++ 和 CUDA 的编译参数

if(MSVC)
    target_compile_options(Tracer PRIVATE
        # 对于普通的 .cpp 文件，直接传递 /utf-8 给 MSVC
        $<$<COMPILE_LANGUAGE:CXX>:/utf-8>
        
        # 对于 .cu 文件，通过 -Xcompiler 包装，让 nvcc 安全地转交给后端的 MSVC
        $<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=/utf-8>
    )
endif()

message(STATUS "Src files: ${SRC_FILES}")

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR} ${CUDAToolkit_INCLUDE_DIRS})
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE CUDA::cudart)

# WorkDir Definition
target_compile_definitions(Tracer PRIVATE 
    PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)


