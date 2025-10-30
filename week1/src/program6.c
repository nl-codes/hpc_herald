#include <stdio.h>

int main() {
    for (int i = 1; i < 100; i++) {
        if (i % 2 == 0 && i % 3 == 0 && i % 5 == 0) {
            printf("BishBashBosh\n");
        } else if (i % 2 == 0 && i % 3 == 0) {
            printf("BishBash\n");
        } else if (i % 3 == 0 && i % 5 == 0) {
            printf("BashBosh\n");
        } else if (i % 2 == 0 && i % 5 == 0) {
            printf("BishBosh\n");
        } else if (i % 2 == 0) {
            printf("Bish\n");
        } else if (i % 3 == 0) {
            printf("Bash\n");
        } else if (i % 5 == 0) {
            printf("Bosh\n");
        } else {
            printf("%d\n", i);
        }
    }
    return 0;
}