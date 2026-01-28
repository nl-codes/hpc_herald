/*
 * Password Generator
 * ----------------------------------------------------------------------------
 * This program generates 10 random 4-character passwords and applies a custom
 * transformation algorithm (cudaCrypt) to expand them into 10-character strings.
 * These transformed strings are then hashed using the SHA512 algorithm with a
 * predefined salt. The program outputs two files one with the final hashes and
 * another a reference to original password for verification.
 * ----------------------------------------------------------------------------
 * Usage:
 *      gcc CreateEncryptedPasswordText.c -o create -lcrypt
 *      ./create
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <crypt.h>

#define SALT "$6$AS$"

void cudaCrypt(const char* orgPassword, char* rawPassword);

/*
 * Entry point of the program. Generates random passwords, transforms and hashes them,
 * and writes the results to output files.
 */
int main() {
    srand(time(NULL)); // Seed the random number generator

    // Open output files for encrypted passwords
    FILE* fp = fopen("EncryptedPasswords.txt", "w");
    // Open hint file for rawpasswords (reference for decryption)
    FILE* hfp = fopen("PasswordHint.txt", "w");

    if (fp == NULL || hfp == NULL) {
        printf("Error opening file(s)!\n");
        return 1;
    }

    // Arrays to store passwords for the hint file
    char orgStorage[10][5];
    char rawStorage[10][11];

    printf("Generating 10 random passwords and their SHA512 hashes...\n\n");

    for (int i = 0; i < 10; i++) {
        char orgPassword[5];
        char rawPassword[11];

        // Generate random 4-character password: [a-z][a-z][0-9][0-9]
        orgPassword[0] = 'a' + (rand() % 26);
        orgPassword[1] = 'a' + (rand() % 26);
        orgPassword[2] = '0' + (rand() % 10);
        orgPassword[3] = '0' + (rand() % 10);
        orgPassword[4] = '\0';

        // Transform using cudaCrypt
        cudaCrypt(orgPassword, rawPassword);

        // Store for the hint file later
        strcpy(orgStorage[i], orgPassword);
        strcpy(rawStorage[i], rawPassword);

        // Encrypt using SHA512 with SALT
        const char* encrypted = crypt(rawPassword, SALT);

        // Write to EncryptedPasswords.txt
        fprintf(fp, "%s\n", encrypted);

        printf("Password %2d: %s -> %s\n", i + 1, orgPassword, rawPassword);
        printf("Hash: %s\n\n", encrypted);
    }

    // --- Write to PasswordHint.txt ---
    // 1. Write all original passwords
    fprintf(hfp, "Original Passwords:\n\n");
    for (int i = 0; i < 10; i++) {
        fprintf(hfp, "%d. %s\n", i + 1, orgStorage[i]);
    }

    fprintf(hfp, "\ncudaCrypt Hashes:\n\n");

    // 2. Write all transformed (raw) passwords
    for (int i = 0; i < 10; i++) {
        fprintf(hfp, "%d. %s\n", i + 1, rawStorage[i]);
    }

    // Close files
    fclose(fp);
    fclose(hfp);

    printf("Encrypted passwords saved to EncryptedPasswords.txt\n");
    printf("Hints saved to PasswordHint.txt\n");

    return 0;
}

/**
 * @brief Transforms a 4-character password into a 10-character raw password using a custom algorithm.
 *  @param orgPassword: The original 4-character password (format: [a-z][a-z][0-9][0-9])
 *  @param rawPassword: The output buffer for the transformed 10-character password
 *
 * The first 6 characters are based on the first two letters (a-z),
 * the last 4 are based on the two digits (0-9).
 * Bounds checking ensures all characters remain valid.
 */
void cudaCrypt(const char* orgPassword, char* rawPassword) {
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

    // Bounds checking with wrapping/reflection for a-z (97-122) and 0-9 (48-57)
    for (int i = 0; i < 10; i++) {
        if (i >= 0 && i < 6) { // Check all lower case letter limits
            if (rawPassword[i] > 122) {
                rawPassword[i] = (rawPassword[i] - 122) + 97;
            } else if (rawPassword[i] < 97) {
                rawPassword[i] = (97 - rawPassword[i]) + 97;
            }
        } else { // Check number section
            if (rawPassword[i] > 57) {
                rawPassword[i] = (rawPassword[i] - 57) + 48;
            } else if (rawPassword[i] < 48) {
                rawPassword[i] = (48 - rawPassword[i]) + 48;
            }
        }
    }
    rawPassword[10] = '\0'; // Null-terminate the string
}
