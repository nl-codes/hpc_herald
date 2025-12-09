#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

long encode(char* s) {
    long a, b, c, x;
    a = s[0];
    b = s[1];
    c = s[2];
    x = ((((a * 69) + c) * 137) + b) * 39;
    x = x % 54321;
    return x;
}

int main() {
    long x, y;
    char result[4] = { 0 };
    int found = 0;

    printf("Enter the code: ");
    scanf("%ld", &x);

#pragma omp parallel for collapse(3) num_threads(16) shared(found, result)
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
            for (int k = 0; k < 26; k++) {
                // if some thread already found the answer
                if (found) continue;
                char s[4];
                s[0] = 'a' + i;
                s[1] = 'a' + j;
                s[2] = 'a' + k;
                s[3] = '\0';

                long y = encode(s);
                if (y == x) {
#pragma omp critical
                    {
                        if (!found) {
                            found = 1;
                            result[0] = s[0];
                            result[1] = s[1];
                            result[2] = s[2];
                            result[3] = '\0';
                        }
                    }
                }
            }
        }
    }
    if (found)
        printf("The letters for code %ld are %s\n", x, result);
    else
        printf("No match found.\n");
    return 0;
}
