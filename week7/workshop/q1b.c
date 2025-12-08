#include <stdio.h>
#include <omp.h>
#include <unistd.h>

int booksAvailable = 3;

void borrower(int id) {
    for (int i = 0; i < 5; i++) {

        int success = 0;
        while (!success) {
            int old;

            // atomic read + decrement
#pragma omp atomic capture
            { old = booksAvailable; booksAvailable--; }

            if (old > 0) {
                success = 1;   // borrow successful
            } else {
                // rolled below zero → revert
#pragma omp atomic
                booksAvailable++;

                usleep(1000);
            }
        }

#pragma omp critical(printing)
        printf("Borrower %d borrowed a book. Remaining = %d\n", id, booksAvailable);

        usleep(10000); // reading

        // return book
#pragma omp atomic
        booksAvailable++;

#pragma omp critical(printing)
        printf("Borrower %d returned a book. Now = %d\n", id, booksAvailable);

        usleep(5000);
    }
}

int main() {
#pragma omp parallel num_threads(5)
    borrower(omp_get_thread_num() + 1);
}
