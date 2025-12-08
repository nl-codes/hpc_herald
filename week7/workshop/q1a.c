#include <stdio.h>
#include <omp.h>
#include <unistd.h>

int booksAvailable = 3;
omp_lock_t lock1;

void borrower(int);

int main() {
    omp_init_lock(&lock1);
#pragma omp parallel num_threads(5)
    {
        borrower(omp_get_thread_num() + 1);
    }
    omp_destroy_lock(&lock1);
    return 0;
}

void borrower(int id) {
    for (int i = 0; i < 5; i++) {
        while (1) {
            omp_set_lock(&lock1);

            if (booksAvailable > 0) {
                // take book
                printf("Borrower %d wants to borrow a book. Available = %d\n", id, booksAvailable);
                booksAvailable--;
                printf("Borrower %d got book. Remaining = %d\n", id, booksAvailable);
                printf("------------------------------------------------\n");
                omp_unset_lock(&lock1);
                break; // Exit when borrower gets the book
            }

            // no books → wait
            omp_unset_lock(&lock1);
            usleep(1000); // small wait before retry
        }

        // read book outside lock
        usleep(10000);

        // return book
        omp_set_lock(&lock1);
        booksAvailable++;
        printf("Borrower %d returned a book. Updated to = %d\n", id, booksAvailable);
        printf("------------------------------------------------\n");
        omp_unset_lock(&lock1);

        usleep(5000);
    }
}
