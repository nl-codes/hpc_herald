#include <stdio.h>
#include <cuda_runtime.h>
__global__ void display() {
    printf("Block %d -> tid X = %d and Y = %d and tid Z = %d\n",
        blockIdx.x, threadIdx.x, threadIdx.y, threadIdx.z);
}
int main() {
    dim3 gridSize(1, 1, 1);
    dim3 blockSize(3, 2, 3);
    display << <gridSize, blockSize >> > ();
    cudaDeviceSynchronize();
    return 0;
}
