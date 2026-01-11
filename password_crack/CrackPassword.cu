#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime.h>
#include <crypt.h>

// --- Constants ---
#define PASSWORD_LENGTH 4
#define RAW_PASSWORD_LENGTH 10
#define NUM_LETTERS 26
#define NUM_DIGITS 10
#define TOTAL_COMBINATIONS 67600  // 26*26*10*10 = 67,600
#define MAX_HASH_LENGTH 256
#define SALT "$6$AS$"

// --- Function Prototypes ---
__device__ void cudaCryptDevice(const char* orgPassword, char* rawPassword);
__global__ void generatePasswordsKernel(char* rawPasswords);
void indexToOriginalPassword(int idx, char* password);

/*
 * __device__ void cudaCryptDevice(const char* orgPassword, char* rawPassword)
 * ---------------------------------------------------------------------------
 * CUDA device function to transform a 4-character password into a 10-character
 * raw password using a custom algorithm. Handles bounds for letters and digits.
 */
__device__ void cudaCryptDevice(const char* orgPassword, char* rawPassword) {
    // Transform each character according to custom rules
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

    // Bounds checking for a-z (ASCII 97-122) and 0-9 (ASCII 48-57)
    for (int i = 0; i < 10; i++) {
        if (i >= 0 && i < 6) { // First 6 are letters
            if (rawPassword[i] > 122) {
                rawPassword[i] = (rawPassword[i] - 122) + 97;
            } else if (rawPassword[i] < 97) {
                rawPassword[i] = (97 - rawPassword[i]) + 97;
            }
        } else { // Last 4 are digits
            if (rawPassword[i] > 57) {
                rawPassword[i] = (rawPassword[i] - 57) + 48;
            } else if (rawPassword[i] < 48) {
                rawPassword[i] = (48 - rawPassword[i]) + 48;
            }
        }
    }

    rawPassword[RAW_PASSWORD_LENGTH] = '\0'; // Null-terminate
}

/*
 * __global__ void generatePasswordsKernel(char* rawPasswords)
 * -----------------------------------------------------------
 * CUDA kernel to generate all possible raw passwords from original 4-character
 * passwords. Each thread computes one transformation and stores it in global memory.
 */
__global__ void generatePasswordsKernel(char* rawPasswords) {
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

        // Create candidate password in local memory
        char candidate[PASSWORD_LENGTH + 1];
        candidate[0] = 'a' + letter1_idx;
        candidate[1] = 'a' + letter2_idx;
        candidate[2] = '0' + digit1_idx;
        candidate[3] = '0' + digit2_idx;
        candidate[PASSWORD_LENGTH] = '\0';

        // Transform and store in global memory
        char* rawPassword = &rawPasswords[idx * (RAW_PASSWORD_LENGTH + 1)];
        cudaCryptDevice(candidate, rawPassword);
    }
}

/*
 * void indexToOriginalPassword(int idx, char* password)
 * -----------------------------------------------------
 * Helper function to regenerate the original 4-character password from its index.
 */
void indexToOriginalPassword(int idx, char* password) {
    int letter1_idx = idx / (NUM_LETTERS * NUM_DIGITS * NUM_DIGITS);
    int remainder = idx % (NUM_LETTERS * NUM_DIGITS * NUM_DIGITS);
    int letter2_idx = remainder / (NUM_DIGITS * NUM_DIGITS);
    remainder = remainder % (NUM_DIGITS * NUM_DIGITS);
    int digit1_idx = remainder / NUM_DIGITS;
    int digit2_idx = remainder % NUM_DIGITS;

    password[0] = 'a' + letter1_idx;
    password[1] = 'a' + letter2_idx;
    password[2] = '0' + digit1_idx;
    password[3] = '0' + digit2_idx;
    password[PASSWORD_LENGTH] = '\0';
}

/*
 * int main(int argc, char* argv[])
 * --------------------------------
 * Main function. Loads target hashes, launches CUDA kernel to generate raw passwords,
 * compares hashes, and writes results to file.
 */
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

    // Read up to 10 hashes, removing newlines
    while (fgets(targetHashes[numHashes], sizeof(targetHashes[0]), file) && numHashes < 10) {
        size_t len = strlen(targetHashes[numHashes]);
        if (len > 0 && targetHashes[numHashes][len - 1] == '\n') {
            targetHashes[numHashes][len - 1] = '\0';
        }
        numHashes++;
    }
    fclose(file);

    printf("Loaded %d target hashes\n", numHashes);
    printf("Using salt: %s\n\n", SALT);

    // Allocate device memory for raw passwords
    char* d_rawPasswords;
    cudaError_t err;

    err = cudaMalloc((void**)&d_rawPasswords, TOTAL_COMBINATIONS * (RAW_PASSWORD_LENGTH + 1) * sizeof(char));
    if (err != cudaSuccess) {
        printf("CUDA error allocating rawPasswords: %s\n", cudaGetErrorString(err));
        return 1;
    }

    // Configure kernel launch parameters
    dim3 blockSize(16, 16);
    int threadsPerBlock = blockSize.x * blockSize.y;
    int blocksPerGrid = (TOTAL_COMBINATIONS + threadsPerBlock - 1) / threadsPerBlock;

    printf("Launching CUDA kernel to generate passwords:\n");
    printf("  Grid size: %d blocks\n", blocksPerGrid);
    printf("  Block size: %dx%d (%d threads per block)\n",
        blockSize.x, blockSize.y, threadsPerBlock);
    printf("  Total combinations: %d\n", TOTAL_COMBINATIONS);

    // Launch kernel to generate all raw passwords on GPU
    generatePasswordsKernel << <blocksPerGrid, blockSize >> > (d_rawPasswords);

    cudaDeviceSynchronize();

    // Check for kernel errors
    err = cudaGetLastError();
    if (err != cudaSuccess) {
        printf("Kernel launch error: %s\n", cudaGetErrorString(err));
        return 1;
    }

    // Allocate host memory for raw passwords
    char* h_rawPasswords = (char*)malloc(TOTAL_COMBINATIONS * (RAW_PASSWORD_LENGTH + 1) * sizeof(char));

    // Copy results back to host
    printf("Copying raw passwords from GPU to host...\n");
    cudaMemcpy(h_rawPasswords, d_rawPasswords,
        TOTAL_COMBINATIONS * (RAW_PASSWORD_LENGTH + 1) * sizeof(char),
        cudaMemcpyDeviceToHost);

    printf("Passwords copied to host. Starting SHA512 comparison on CPU...\n\n");

    // Arrays to store cracked password indices and flags
    int crackedIndices[10];
    int crackedFlags[10] = { 0 }; // Track which positions have been cracked
    int crackedCount = 0;

    // Compare each raw password's hash to target hashes
    for (int i = 0; i < TOTAL_COMBINATIONS && crackedCount < numHashes; i++) {
        char* rawPassword = &h_rawPasswords[i * (RAW_PASSWORD_LENGTH + 1)];

        // Encrypt using SHA512 with crypt
        const char* encrypted = crypt(rawPassword, SALT);

        // Compare with all target hashes
        for (int h = 0; h < numHashes; h++) {
            if (!crackedFlags[h] && strcmp(encrypted, targetHashes[h]) == 0) {
                // Found a match! Store the index
                crackedIndices[h] = i;
                crackedFlags[h] = 1;
                crackedCount++;

                // Regenerate original password for display
                char orgPassword[PASSWORD_LENGTH + 1];
                indexToOriginalPassword(i, orgPassword);

                printf("Found match for hash %d! Password: %s -> Raw: %s\n", h + 1, orgPassword, rawPassword);
                break;
            }
        }

        // Progress indicator every 10,000 combinations
        if ((i + 1) % 12000 == 0) {
            printf("Tested %d/%d combinations... (found %d)\n", i + 1, TOTAL_COMBINATIONS, crackedCount);
        }
    }

    printf("\n=== CRACKING COMPLETE ===\n");
    printf("Found %d passwords!\n\n", crackedCount);

    // Write results to file in the order of the original hashes
    FILE* resultFile = fopen("result.txt", "w");
    if (resultFile) {
        for (int i = 0; i < numHashes; i++) {
            if (crackedFlags[i]) {
                char orgPassword[PASSWORD_LENGTH + 1];
                indexToOriginalPassword(crackedIndices[i], orgPassword);

                printf("Hash line %d: %s\n", i + 1, orgPassword);
                fprintf(resultFile, "%s\n", orgPassword);
            } else {
                printf("Hash line %d: NOT CRACKED\n", i + 1);
                fprintf(resultFile, "\n"); // Empty line for uncracked hashes
            }
        }
        fclose(resultFile);
        printf("\nResults written to result.txt\n");
    }

    // Free memory
    free(h_rawPasswords);
    cudaFree(d_rawPasswords);

    cudaDeviceReset();
    return 0;
}
