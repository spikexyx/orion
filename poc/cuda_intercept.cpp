//
// Created by Ethan Xu on 2024/7/22.
//
#include <cuda_runtime.h>
#include <dlfcn.h>
#include <iostream>
#include <cstdio>

typedef cudaError_t (*orig_cudaLaunchKernel_t)(const void*, dim3, dim3, void**, size_t, cudaStream_t);
typedef cudaError_t (*orig_cudaMemcpy_t)(void* dst, const void* src, size_t count, enum cudaMemcpyKind kind);
typedef cudaError_t (*orig_cudaMalloc_t)(void** devPtr, size_t size);
typedef cudaError_t (*orig_cudaFree_t)(void* devPtr);

orig_cudaLaunchKernel_t orig_cudaLaunchKernel = nullptr;
orig_cudaMemcpy_t orig_cudaMemcpy = nullptr;
orig_cudaMalloc_t orig_cudaMalloc = nullptr;
orig_cudaFree_t orig_cudaFree = nullptr;

cudaError_t cudaMemcpy(void* dst, const void* src, size_t count, enum cudaMemcpyKind kind) {
    // printf("Hello from intercepted cudaMemcpy!\n");

    if(!orig_cudaMemcpy) {
        orig_cudaMemcpy = (orig_cudaMemcpy_t)dlsym(RTLD_NEXT, "cudaMemcpy");
    }

    int num_devices = 0;
    cudaGetDeviceCount(&num_devices);

    cudaError_t err = cudaSuccess;

    if(kind == cudaMemcpyDeviceToHost) {
        err = (*orig_cudaMemcpy)(dst, src, count, kind);
        return err;
    }

    for(int i = 0; i < num_devices; i++) {
        cudaSetDevice(i);
        err = (*orig_cudaMemcpy)(dst, src, count, kind);
    }

    return err;
}

cudaError_t cudaMalloc(void** devPtr, size_t size) {
    // printf("Hello from intercepted cudaMalloc!\n");

    if(!orig_cudaMalloc) {
        orig_cudaMalloc = (orig_cudaMalloc_t)dlsym(RTLD_NEXT, "cudaMalloc");
    }

    int num_devices = 0;
    cudaGetDeviceCount(&num_devices);

    cudaError_t err = cudaSuccess;

    for(int i = 0; i < num_devices; i++) {
        cudaSetDevice(i);
        err = (*orig_cudaMalloc)(devPtr, size);
    }

    return err;
}

cudaError_t cudaFree(void* devPtr) {
    if(!orig_cudaFree) {
        orig_cudaFree = (orig_cudaFree_t)dlsym(RTLD_NEXT, "cudaFree");
    }

    int num_devices = 0;
    cudaGetDeviceCount(&num_devices);

    cudaError_t err = cudaSuccess;

    for(int i = 0; i < num_devices; i++) {
        cudaSetDevice(i);
        err = (*orig_cudaFree)(devPtr);
    }

    return err;
}

// 劫持的 cudaLaunchKernel 实现
cudaError_t cudaLaunchKernel(const void* func, dim3 gridDim, dim3 blockDim, void** args, size_t sharedMem, cudaStream_t stream) {

    // printf("Hello from intercepted cudaLaunchKernel!\n");

    if (!orig_cudaLaunchKernel) {
        orig_cudaLaunchKernel = (orig_cudaLaunchKernel_t)dlsym(RTLD_NEXT, "cudaLaunchKernel");
    }

    static int call_count = 0;
    int num_devices = 0;
    cudaGetDeviceCount(&num_devices);
    // printf("Num of devices is: %d\n", num_devices);
    int device = call_count % num_devices; // Round-Robin
    call_count++;
    cudaSetDevice(device);

    // 立即返回一个成功状态，调度器将处理队列中的内核启动
    return (*orig_cudaLaunchKernel)(func, gridDim, blockDim, args, sharedMem, stream);
}

extern "C" void __attribute__((constructor)) init() {
    printf("CUDA Intercept Library Loaded\n");
}