#include <stdio.h>
#include <windows.h> // for DWORD
#include <unistd.h> // for sleep()

DWORD core_id;
void task1() {
    for (int i = 1; i <= 15; i++) {
        core_id = GetCurrentProcessorNumber();
        printf("Task 1 - Step %d: Running in core: %lu\n", i, core_id);
        // sleep(1); // simulate work
    }
}

void task2() {
    for (int i = 1; i <= 15; i++) {
        core_id = GetCurrentProcessorNumber();
        printf("Task 2 - Step %d: Running in core: %lu\n", i, core_id);
        // sleep(1); // simulate work
    }
}

int main() {
    printf("Sequential Execution Start\n\n");

    task1();  // executes first
    task2();  // starts only after task1 finishes

    printf("\nSequential Execution End\n");
    return 0;
}