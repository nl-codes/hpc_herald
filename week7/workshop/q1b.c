#include <stdio.h>
#include <omp.h>
#include <unistd.h>

#define total_books 3
int booksAvailable = total_books;

void borrower(int);

int main() {
#pragma omp parallel num_threads(5)
    {
        borrower(omp_get_thread_num() + 1);
    }
return 0;
}

void borrower(int id) {
    int i;
    for (i = 0; i < 5; i++) {
        while (1) {
            int book_borrowed = 0;
#pragma omp critical(books)
            {
                if (booksAvailable > 0) {
                    printf("Borrower %d wants to borrow a book. Available = %d\n", id, booksAvailable);
                    booksAvailable--;
                    printf("Borrower %d got book. Remaining = %d\n", id, booksAvailable);
                    printf("------------------------------------------------\n");
                    book_borrowed = 1;
                }
            }
            usleep(1000);
            if (book_borrowed == 1) break;
        }

#pragma omp critical(books)
        {
            booksAvailable++;
            printf("Borrower %d returned a book. Updated to = %d\n", id, booksAvailable);
            printf("------------------------------------------------\n");
        }
        usleep(5000);
    }
}
