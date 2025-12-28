#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define THREADS_PER_BLOCK 256  // Number of threads per CUDA block

/**
 * Counts the number of lines in a file
 * @param fptr - Pointer to the FILE object
 * @return Number of lines in the file
 */
int countLines(FILE* fptr);

/**
 * Reads quadratic equation coefficients from file into memory
 * @param fptr - Pointer to the FILE object
 * @param a - Array to store 'a' coefficients
 * @param b - Array to store 'b' coefficients
 * @param c - Array to store 'c' coefficients
 * @param number_of_lines - Number of lines/equations to read
 */
void readFileIntoMemory(FILE* fptr, int* a, int* b, int* c, int number_of_lines);

/**
 * Deallocates host memory for coefficient arrays
 * @param h_a - Host array for 'a' coefficients
 * @param h_b - Host array for 'b' coefficients
 * @param h_c - Host array for 'c' coefficients
 */
void deallocateHostMemory(int* h_a, int* h_b, int* h_c);

/**
 * Deallocates device memory for coefficient arrays
 * @param d_a - Device array for 'a' coefficients
 * @param d_b - Device array for 'b' coefficients
 * @param d_c - Device array for 'c' coefficients
 */
void deallocateDeviceMemory(int* d_a, int* d_b, int* d_c);

/**
 * Deallocates memory for solution arrays (both host and device)
 * @param h_x1 - Host array for first solutions (x1)
 * @param h_x2 - Host array for second solutions (x2)
 * @param d_x1 - Device array for first solutions (x1)
 * @param d_x2 - Device array for second solutions (x2)
 */
void deallocateResultMemory(double* h_x1, double* h_x2, double* d_x1, double* d_x2);

/**
 * CUDA kernel function to solve quadratic equations in parallel
 * Each thread solves one quadratic equation: ax² + bx + c = 0
 * @param d_a - Device array of 'a' coefficients
 * @param d_b - Device array of 'b' coefficients
 * @param d_c - Device array of 'c' coefficients
 * @param d_x1 - Device array to store first solutions
 * @param d_x2 - Device array to store second solutions
 * @param number_of_lines - Total number of equations to solve
 */
__global__ void solve_quad(int* d_a, int* d_b, int* d_c, double* d_x1, double* d_x2, int number_of_lines);

int main() {
    int lines;  // Number of equations/lines in the file

    // Host (CPU) memory pointers for coefficients
    int* h_a, * h_b, * h_c;

    // Device (GPU) memory pointers for coefficients
    int* d_a, * d_b, * d_c;

    // Host and device memory pointers for solutions
    double* h_x1, * h_x2;
    double* d_x1, * d_x2;

    // 1. Open file to read data
    FILE* fptr = fopen("QuadData.txt", "r");
    if (fptr == NULL) {
        perror("File open failed");
        return 1;
    }

    // 2. Count number of lines in the file
    lines = countLines(fptr);

    // 3. Allocate host memory to store the file data
    h_a = (int*)malloc(lines * sizeof(int));
    h_b = (int*)malloc(lines * sizeof(int));
    h_c = (int*)malloc(lines * sizeof(int));

    // 4. Read the data from file to host memory
    readFileIntoMemory(fptr, h_a, h_b, h_c, lines);

    // 5. Close the file
    fclose(fptr); // File Closed

    // 6. Allocate device memory for coefficients and solutions
    cudaMalloc(&d_a, lines * sizeof(int));
    cudaMalloc(&d_b, lines * sizeof(int));
    cudaMalloc(&d_c, lines * sizeof(int));
    cudaMalloc(&d_x1, lines * sizeof(double));
    cudaMalloc(&d_x2, lines * sizeof(double));

    // 7. Copy coefficients from host to device memory
    cudaMemcpy(d_a, h_a, lines * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, lines * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_c, h_c, lines * sizeof(int), cudaMemcpyHostToDevice);

    // 8. Launch kernel routine to solve equations in parallel
    // Calculate block size based on number of equations and threads per block
    int num_blocks = lines / THREADS_PER_BLOCK;
    if (lines % THREADS_PER_BLOCK > 0)
        num_blocks++;  // Add extra block if lines don't divide evenly

    // Define grid and block dimensions
    dim3 gd(num_blocks, 1, 1);  // Grid dimensions
    dim3 bd(THREADS_PER_BLOCK, 1, 1);  // Block dimensions

    // Launch the CUDA kernel
    solve_quad << <gd, bd >> > (d_a, d_b, d_c, d_x1, d_x2, lines);
    cudaDeviceSynchronize();  // Wait for kernel to complete

    // 9. Allocate host memory to store results
    h_x1 = (double*)malloc(lines * sizeof(double));
    h_x2 = (double*)malloc(lines * sizeof(double));

    // 10. Copy results from device to host
    cudaMemcpy(h_x1, d_x1, lines * sizeof(double), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_x2, d_x2, lines * sizeof(double), cudaMemcpyDeviceToHost);

    // 11. Print the solutions
    for (int i = 0; i < lines; i++) {
        printf("Line (%d) : x1 = %lf and x2 = %lf\n", i + 1, h_x1[i], h_x2[i]);
    }

    // 12. Free all allocated memory
    deallocateHostMemory(h_a, h_b, h_c);
    deallocateDeviceMemory(d_a, d_b, d_c);
    deallocateResultMemory(h_x1, h_x2, d_x1, d_x2);

    return 0;
}

int countLines(FILE* fptr) {
    int number_of_lines = 0;
    int ch, prev = '\n';

    rewind(fptr);  // Reset file pointer to beginning

    // Count newline characters
    while ((ch = fgetc(fptr)) != EOF) {
        if (ch == '\n')
            number_of_lines++;
    }

    // Handle case where file doesn't end with newline
    if (prev != '\n') number_of_lines++;

    rewind(fptr);  // Reset file pointer for subsequent reading
    return number_of_lines;
}

void readFileIntoMemory(FILE* fptr, int* a, int* b, int* c, int number_of_lines) {
    rewind(fptr);  // Ensure we're at the beginning of the file

    // Read coefficients in format: a,b,c (comma-separated)
    for (int i = 0; i < number_of_lines; i++) {
        if (fscanf(fptr, "%d,%d,%d", &a[i], &b[i], &c[i]) != 3) {
            printf("Invalid data at line %d\n", i + 1);
            deallocateHostMemory(a, b, c);
            exit(1);
        }
    }
}

void deallocateHostMemory(int* h_a, int* h_b, int* h_c) {
    free(h_a);
    free(h_b);
    free(h_c);
}

void deallocateDeviceMemory(int* d_a, int* d_b, int* d_c) {
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
}

void deallocateResultMemory(double* h_x1, double* h_x2, double* d_x1, double* d_x2) {
    cudaFree(d_x1);  // Free device memory for x1 solutions
    cudaFree(d_x2);  // Free device memory for x2 solutions
    free(h_x1);      // Free host memory for x1 solutions
    free(h_x2);      // Free host memory for x2 solutions
}

__global__ void solve_quad(int* d_a, int* d_b, int* d_c, double* d_x1, double* d_x2, int number_of_lines) {
    // Calculate thread ID - each thread handles one equation
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    // Check if thread ID is within bounds of number of equations
    if (tid >= number_of_lines) return;

    double a, b, c;
    double x1 = 0.0, x2 = 0.0;

    // Get coefficients for this thread's equation
    a = (double)d_a[tid];
    b = (double)d_b[tid];
    c = (double)d_c[tid];

    // Calculate discriminant: b² - 4ac
    double discriminant = (b * b) - 4.0 * a * c;

    // Check if solutions are real
    if (discriminant < 0) {
        // No real solutions - store NaN (Not a Number)
        x1 = NAN;
        x2 = NAN;
    } else {
        // Real solutions exist - calculate using quadratic formula
        double square_root = sqrt(discriminant);
        double twice_a = 2 * a;
        x1 = (-b + square_root) / twice_a;  // First root
        x2 = (-b - square_root) / twice_a;  // Second root
    }

    // Store solutions in device memory
    d_x1[tid] = x1;
    d_x2[tid] = x2;
}
