#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <crypt.h>

#define SALT "$6$AS$"

// Custom cudaCrypt function to transform 4-char password to 10-char rawPassword
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

    // Bounds checking with wrapping/reflection for a-z (97-122)
    for (int i = 0; i < 6; i++) {
        if (rawPassword[i] < 97) {
            rawPassword[i] = 97 + (97 - rawPassword[i]);
        } else if (rawPassword[i] > 122) {
            rawPassword[i] = 122 - (rawPassword[i] - 122);
        }
    }

    // Bounds checking with wrapping/reflection for 0-9 (48-57)
    for (int i = 6; i < 10; i++) {
        if (rawPassword[i] < 48) {
            rawPassword[i] = 48 + (48 - rawPassword[i]);
        } else if (rawPassword[i] > 57) {
            rawPassword[i] = 57 - (rawPassword[i] - 57);
        }
    }

    rawPassword[10] = '\0';
}

int main() {
    srand(time(NULL));
    FILE* fp = fopen("EncryptedPasswords.txt", "w");

    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

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

        // Encrypt using SHA512 with SALT
        const char* encrypted = crypt(rawPassword, SALT);

        // Write to file
        fprintf(fp, "%s\n", encrypted);

        printf("Password %2d: %s -> %s\n", i + 1, orgPassword, rawPassword);
        printf("Hash: %s\n\n", encrypted);
    }

    fclose(fp);
    printf("Encrypted passwords saved to EncryptedPasswords.txt\n");

    return 0;
}
