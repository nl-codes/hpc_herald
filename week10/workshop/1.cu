#include <stdio.h>
__device__ void displayHelloWorld() {
    printf("Hello World\n");
}
__global__ void display() {
    displayHelloWorld();
}
int main() {
    display << <1, 1 >> > ();
    cudaDeviceSynchronize();
    return 0;
}
