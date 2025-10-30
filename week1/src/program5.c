#include <stdio.h>

int main() {
    char name[20];
    int age;

    printf("Enter details:\nName: ");
    scanf("%s", &name);
    printf("Age: ");
    scanf("%d", &age);

    printf("Hello %s, you are %d years old.\n", name, age);
    return 0;
}
