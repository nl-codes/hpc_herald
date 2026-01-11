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
    for (int i = 0; i < 10; i++) {
        if (i >= 0 && i < 6) { //checking all lower case letter limits
            if (rawPassword[i] > 122) {
                rawPassword[i] = (rawPassword[i] - 122) + 97;
            } else if (rawPassword[i] < 97) {
                rawPassword[i] = (97 - rawPassword[i]) + 97;
            }
        } else { //checking number section
            if (rawPassword[i] > 57) {
                rawPassword[i] = (rawPassword[i] - 57) + 48;
            } else if (rawPassword[i] < 48) {
                rawPassword[i] = (48 - rawPassword[i]) + 48;
            }
        }
    }

    rawPassword[10] = '\0';
}

int main() {
    srand(time(NULL));
    FILE* fp = fopen("EncryptedPasswords.txt", "w");
    FILE* hfp = fopen("PasswordHint.txt", "w"); // File pointer for hints

    if (fp == NULL || hfp == NULL) {
        printf("Error opening file(s)!\n");
        return 1;
    }

    // Arrays to store passwords for the second file
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
    // 1. Write all org_passwords
    for (int i = 0; i < 10; i++) {
        fprintf(hfp, "%s\n", orgStorage[i]);
    }

    // 2. Write the two new lines (blank lines as per your description)
    fprintf(hfp, "\n\n");

    // 3. Write all raw_passwords
    for (int i = 0; i < 10; i++) {
        fprintf(hfp, "%s\n", rawStorage[i]);
    }

    fclose(fp);
    fclose(hfp);

    printf("Encrypted passwords saved to EncryptedPasswords.txt\n");
    printf("Hints saved to PasswordHint.txt\n");

    return 0;
}
