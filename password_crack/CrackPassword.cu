#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime.h>
#include <crypt.h>

#define PASSWORD_LENGTH 4
#define RAW_PASSWORD_LENGTH 10
#define NUM_LETTERS 26
#define NUM_DIGITS 10
#define TOTAL_COMBINATIONS 67600  // 26*26*10*10 = 67,600
#define MAX_HASH_LENGTH 256
#define SALT "$6$AS$"

// CUDA transformation function (device version)
__device__ void cudaCryptDevice(const char* orgPassword, char* rawPassword) {
    // Transform indices 0-2 from orgPassword[0]
    rawPassword[0] = orgPassword[0] + 2;
    rawPassword[1] = orgPassword[0] - 2;
    rawPassword[2] = orgPassword[0] + 1;
    rawPassword[3] = orgPassword[1] + 3;
    rawPassword[4] = orgPassword[1] - 3;
    rawPassword[5] = orgPassword[1] - 1;
    rawPassword[6] = orgPassword[2] + 2;
    rawPassword[7] = orgPassword[2] - 2;
    rawPassword[8] = orgPassword[3] + 4;
    rawPassword[9] = orgPassword[3] - 4;

    // Bounds checking for a-z (97-122)
    for (int i = 0; i < 6; i++) {
        if (rawPassword[i] < 97) {
            rawPassword[i] = 97 + (97 - rawPassword[i]);
        } else if (rawPassword[i] > 122) {
            rawPassword[i] = 122 - (rawPassword[i] - 122);
        }
    }

    // Bounds checking for 0-9 (48-57)
    for (int i = 6; i < 10; i++) {
        if (rawPassword[i] < 48) {
            rawPassword[i] = 48 + (48 - rawPassword[i]);
        } else if (rawPassword[i] > 57) {
            rawPassword[i] = 57 - (rawPassword[i] - 57);
        }
    }

    rawPassword[RAW_PASSWORD_LENGTH] = '\0';
}

// CUDA kernel - Generate all candidate passwords and raw passwords
__global__ void generatePasswordsKernel(char* orgPasswords, char* rawPasswords) {
    // Calculate unique combination index using 2D block structure
    int idx = blockIdx.x * blockDim.x * blockDim.y + threadIdx.y * blockDim.x + threadIdx.x;

    if (idx < TOTAL_COMBINATIONS) {
        // Decompose index into password components
        int letter1_idx = idx / (NUM_LETTERS * NUM_DIGITS * NUM_DIGITS);
        int remainder = idx % (NUM_LETTERS * NUM_DIGITS * NUM_DIGITS);
        int letter2_idx = remainder / (NUM_DIGITS * NUM_DIGITS);
        remainder = remainder % (NUM_DIGITS * NUM_DIGITS);
        int digit1_idx = remainder / NUM_DIGITS;
        int digit2_idx = remainder % NUM_DIGITS;

        // Create candidate password
        char* candidate = &orgPasswords[idx * (PASSWORD_LENGTH + 1)];
        candidate[0] = 'a' + letter1_idx;
        candidate[1] = 'a' + letter2_idx;
        candidate[2] = '0' + digit1_idx;
        candidate[3] = '0' + digit2_idx;
        candidate[PASSWORD_LENGTH] = '\0';

        // Apply transformation to get raw string
        char* rawPassword = &rawPasswords[idx * (RAW_PASSWORD_LENGTH + 1)];
        cudaCryptDevice(candidate, rawPassword);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <path_to_EncryptedPasswords.txt>\n", argv[0]);
        return 1;
    }

    // Read target hashes from file
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Error opening file: %s\n", argv[1]);
        return 1;
    }

    char targetHashes[10][MAX_HASH_LENGTH];
    int numHashes = 0;

    while (fgets(targetHashes[numHashes], sizeof(targetHashes[0]), file) && numHashes < 10) {
        // Remove newline
        size_t len = strlen(targetHashes[numHashes]);
        if (len > 0 && targetHashes[numHashes][len - 1] == '\n') {
            targetHashes[numHashes][len - 1] = '\0';
        }
        numHashes++;
    }
    fclose(file);

    printf("Loaded %d target hashes\n", numHashes);
    printf("Using salt: %s\n\n", SALT);

    // Allocate device memory for all password combinations
    char* d_orgPasswords;
    char* d_rawPasswords;

    cudaError_t err;

    err = cudaMalloc((void**)&d_orgPasswords, TOTAL_COMBINATIONS * (PASSWORD_LENGTH + 1) * sizeof(char));
    if (err != cudaSuccess) {
        printf("CUDA error allocating orgPasswords: %s\n", cudaGetErrorString(err));
        return 1;
    }

    err = cudaMalloc((void**)&d_rawPasswords, TOTAL_COMBINATIONS * (RAW_PASSWORD_LENGTH + 1) * sizeof(char));
    if (err != cudaSuccess) {
        printf("CUDA error allocating rawPasswords: %s\n", cudaGetErrorString(err));
        return 1;
    }

    // Configure kernel with 2D block dimensions
    dim3 blockSize(16, 16); // 256 threads per block as requested
    int threadsPerBlock = blockSize.x * blockSize.y;
    int blocksPerGrid = (TOTAL_COMBINATIONS + threadsPerBlock - 1) / threadsPerBlock;

    printf("Launching CUDA kernel to generate passwords:\n");
    printf("  Grid size: %d blocks\n", blocksPerGrid);
    printf("  Block size: %dx%d (%d threads per block)\n",
        blockSize.x, blockSize.y, threadsPerBlock);
    printf("  Total combinations: %d\n\n", TOTAL_COMBINATIONS);

    // Launch kernel to generate all passwords on GPU
    generatePasswordsKernel << <blocksPerGrid, blockSize >> > (d_orgPasswords, d_rawPasswords);

    cudaDeviceSynchronize();

    // Check for kernel errors
    err = cudaGetLastError();
    if (err != cudaSuccess) {
        printf("Kernel launch error: %s\n", cudaGetErrorString(err));
        return 1;
    }

    // Allocate host memory
    char* h_orgPasswords = (char*)malloc(TOTAL_COMBINATIONS * (PASSWORD_LENGTH + 1) * sizeof(char));
    char* h_rawPasswords = (char*)malloc(TOTAL_COMBINATIONS * (RAW_PASSWORD_LENGTH + 1) * sizeof(char));

    // Copy results back to host
    printf("Copying passwords from GPU to host...\n");
    cudaMemcpy(h_orgPasswords, d_orgPasswords,
        TOTAL_COMBINATIONS * (PASSWORD_LENGTH + 1) * sizeof(char),
        cudaMemcpyDeviceToHost);
    cudaMemcpy(h_rawPasswords, d_rawPasswords,
        TOTAL_COMBINATIONS * (RAW_PASSWORD_LENGTH + 1) * sizeof(char),
        cudaMemcpyDeviceToHost);

    printf("Passwords copied to host. Starting SHA512 comparison on CPU...\n\n");

    // Now compare on host using SHA512
    char crackedPasswords[10][PASSWORD_LENGTH + 1];
    int crackedCount = 0;

    for (int i = 0; i < TOTAL_COMBINATIONS && crackedCount < numHashes; i++) {
        char* rawPassword = &h_rawPasswords[i * (RAW_PASSWORD_LENGTH + 1)];
        char* orgPassword = &h_orgPasswords[i * (PASSWORD_LENGTH + 1)];

        // Encrypt using SHA512 with crypt
        const char* encrypted = crypt(rawPassword, SALT);

        // Compare with all target hashes
        for (int h = 0; h < numHashes; h++) {
            if (strcmp(encrypted, targetHashes[h]) == 0) {
                // Found a match!
                strcpy(crackedPasswords[crackedCount], orgPassword);
                crackedCount++;
                printf("Found match! Password: %s -> Raw: %s\n", orgPassword, rawPassword);
                break;
            }
        }

        // Progress indicator
        if ((i + 1) % 10000 == 0) {
            printf("Tested %d/%d combinations... (found %d)\n", i + 1, TOTAL_COMBINATIONS, crackedCount);
        }
    }

    printf("\n=== CRACKING COMPLETE ===\n");
    printf("Found %d passwords!\n\n", crackedCount);

    // Write results to file
    FILE* resultFile = fopen("result.txt", "w");
    if (resultFile) {
        for (int i = 0; i < crackedCount; i++) {
            printf("Cracked password %d: %s\n", i + 1, crackedPasswords[i]);
            fprintf(resultFile, "%s\n", crackedPasswords[i]);
        }
        fclose(resultFile);
        printf("\nResults written to result.txt\n");
    }

    // Free memory
    free(h_orgPasswords);
    free(h_rawPasswords);
    cudaFree(d_orgPasswords);
    cudaFree(d_rawPasswords);

    cudaDeviceReset();
    return 0;
}
