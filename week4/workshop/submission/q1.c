#include <stdio.h>
#include <pthread.h>

typedef struct {
    int accountNumber;
    double balance;
    pthread_mutex_t locker;
} Account;

Account accounts[10];

void* withdraw(void* p) {
    int accountId = *(int*)p;
    double amount = 100;  // Withdraw amount

    pthread_mutex_lock(&accounts[accountId].locker);
    accounts[accountId].balance -= amount;
    printf("Withdrawing %.2lf from Account %d.\n", amount, accountId);
    pthread_mutex_unlock(&accounts[accountId].locker);

    return NULL;
}

void* deposit(void* p) {
    int accountId = *(int*)p;
    double amount = 100;  // Deposit amount

    pthread_mutex_lock(&accounts[accountId].locker);
    accounts[accountId].balance += amount;
    printf("Depositing %.2lf from Account %d.\n", amount, accountId);
    pthread_mutex_unlock(&accounts[accountId].locker);

    return NULL;
}

int main() {
    pthread_t threads[20];
    int ids[10];

    // Create multiple threads to simulate transactions on the same account
    for (int i = 0; i < 10; i++) {
        ids[i] = i;
        pthread_mutex_init(&accounts[i].locker, NULL);
        pthread_create(&threads[i + 10], NULL, deposit, &ids[i]);
        pthread_create(&threads[i], NULL, withdraw, &ids[i]);
    }
    for (int i = 0; i < 20; i++) {
        pthread_join(threads[i], NULL);
    }
    // Print final balance
    for (int i = 0; i < 10; i++) {
        pthread_mutex_destroy(&accounts[i].locker);
        printf("Account %d balance = %.2f\n", i, accounts[i].balance);
    }
    return 0;
}
