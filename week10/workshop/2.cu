#include <stdio.h>
#include <cuda_runtime.h>
__global__ void display() {
    int threadID = threadIdx.x;
    int blockID = blockIdx.x;
    printf("Block %d -> ThreadId = %d\n", blockID, threadID);
}
int main() {
    dim3 gridSize(1, 1, 1);
    dim3 blockSize(5, 1, 1);
    display << <gridSize, blockSize >> > ();
    cudaDeviceSynchronize();
    return 0;
}
