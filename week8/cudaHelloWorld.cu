
#include <stdio.h>

// This function runs on the GPU (device)
__global__ void helloFromGPU() {
    printf("Hello from GPU!\n");
}

int main() {
    // Launch the kernel with 1 block of 1 thread
    helloFromGPU<<<1, 1>>>();

    // Wait for GPU to finish before exiting
    cudaDeviceSynchronize();

    printf("Hello from CPU!\n");
    return 0;
}
