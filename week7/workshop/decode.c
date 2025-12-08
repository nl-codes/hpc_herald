#include <stdio.h>
#include <stdlib.h>

long encode(char* s) {
    long a, b, c, x;
    a = s[0];
    b = s[1];
    c = s[2];
    x = ((((a * 69) + c) * 137) + b) * 39;
    x = x % 54321;
    return x;
}

void main() {
    char s[4];
    long x, y;
    int i, j, k;
    printf("Enter the code: ");
    scanf("%ld", &x);
    s[3] = '\0';
    for (i = 0;i < 26;i++) {
        s[0] = i + 'a';
        for (j = 0;j < 26;j++) {
            s[1] = j + 'a';
            for (k = 0;k < 26;k++) {
                s[2] = k + 'a';
                y = encode(s);
                if (x == y) {
                    printf("The letters for code %ld are %s\n", y, s);
                    exit(0);
                }
            }
        }
    }
}
