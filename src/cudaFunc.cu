#include<cudaHeader.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h> 
#include "stat_render/core/common.h"

__global__ void runCudaMathKernel_GPU(float* data, int size) {
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < size) {
        data[i] = data[i] * 2.0f; // 示例逻辑：翻倍
    }
}

void runCudaMathKernel(float* data, int size) {
    int threadsPerBlock = 256;
    int blocksPerGrid = (size + threadsPerBlock - 1) / threadsPerBlock;

    // 在这里启动核函数
    //runCudaMathKernel_GPU <<<blocksPerGrid, threadsPerBlock >>> (data, size);
    runCudaMathKernel_GPU KERNEL_ARGS2(blocksPerGrid, threadsPerBlock) (data, size);
}