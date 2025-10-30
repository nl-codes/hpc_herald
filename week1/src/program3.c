#include <stdio.h>

typedef struct Student {
    char name[20];
    int age;
    float gpa;
} student;

int main() {
    int std_size = 3;
    student std_list[std_size];

    for (int i = 0; i < std_size; i++) {
        printf("Enter the details of student %d\n", i + 1);
        printf("Name: ");
        scanf("%s", std_list[i].name);
        printf("Age: ");
        scanf("%d", &std_list[i].age);
        printf("GPA: ");
        scanf("%f", &std_list[i].gpa);
        printf("\n");
    }
    printf("\nStudent Details:\n");
    for (int i = 0; i < std_size; i++) {
        printf("Name: %s, Age: %d, GPA:%.2f\n", std_list[i].name, std_list[i].age, std_list[i].gpa);
    }

    return 0;
}