#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/sem.h>
#include <pthread.h>

using namespace std;

const int chairs = 5;
int waiting = 0;

int mutex_t = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
int barbers = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
int customers = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);

//down & up
int down(int sem_id);
int up(int sem_id);

void set_semaphore(int sem_id, int val);

void *barber(void *);
void *customer(void *);

int main() {
    pthread_t barber_id;
    pthread_t customer_id;
    
    // initialize semaphores
    set_semaphore(mutex_t, 1);
    set_semaphore(barbers, 1);
    set_semaphore(customers, 0);
    
    if((pthread_create(&barber_id, NULL, barber, NULL)) != 0) {
        perror("create barber failed\n");
        exit(-1);
    }
    
    while (true) {
        sleep(2); //kggjkyjg
        if ((pthread_create(&customer_id, NULL, customer, NULL)) != 0) {
            perror("create barber failed\n");
            exit(-1);
        }
    }
}

//down & up
int down(int sem_id) {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;
    if (semop(sem_id, &buf, 1) == -1)
    {
        fprintf(stderr, "down failed\n");
        return 0;
    }
    return 1;
}

int up(int sem_id) {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;
    if (semop(sem_id, &buf, 1) == -1) {
        fprintf(stderr, "up failed\n");
        return 0;
    }
    return 1;
}

//set semaphore original value
void set_semaphore(int sem_id, int val) {
    union semun sem_union;
    sem_union.val = val;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) {
        perror("set semaphore error\n");
        exit(-1);
    }
}

void *barber(void *) {
    while (true) {
        down(customers);
        down(mutex_t);
        waiting--;
        up(mutex_t);
        up(barbers);
        cout << "barber is working ..." << endl;
        sleep(3);
        cout << "finish one job!\n" << endl;
    }
}

void *customer(void *) {
    down(mutex_t);
    if (waiting < chairs) {
        waiting++;
        cout << "now " << waiting;
        if (waiting == 1)
            cout << " customer is ";
        else
            cout << " customers are ";
        cout << " waiting" << endl;
        up(customers);
        up(mutex_t);
        down(barbers);
    }
    else {
        cout << "no seat for customer, leave one" << endl;
        up(mutex_t);
    }
    return NULL;
}

