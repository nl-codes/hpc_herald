#include <stdio.h>
#include <sched.h> // for DWORD
#include <unistd.h> // for sleep()

int core_id;
void task1() {
    for (int i = 1; i <= 15; i++) {
            // Get current CPU core ID
        int core_id = sched_getcpu();
        printf("Task 1 - Step %d: Running in core: %d\n", i, core_id);
        // sleep(1); // simulate work
    }
}

void task2() {
    for (int i = 1; i <= 15; i++) {
        core_id = sched_getcpu();
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