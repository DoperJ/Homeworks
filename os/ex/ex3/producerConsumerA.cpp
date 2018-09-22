#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/types.h>

using namespace std;

const int MAXSIZE = 10;

//down & up
int down(int sem_id);
int up(int sem_id);

void set_semaphore(int sem_id, int val);
void insert(char *buffer, char letter, int id);
void removeAny(char *buffer, int id);

int main() {
    pid_t producer1, producer2;
    pid_t consumer1, consumer2, consumer3;
    
    int mutex;
    int full;
    int empty;
    char *buffer;
    
    // initialize shared memory
    buffer = (char*) mmap(NULL, sizeof(char) * MAXSIZE,
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (int i = 0; i < MAXSIZE; i++) {
        buffer[i] = ' ';
    }
    
    // initialize semaphores
    mutex = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    full = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    empty = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    set_semaphore(mutex, 1);
    set_semaphore(full, 0);
    set_semaphore(empty, MAXSIZE);
    
    //producer1
    if ((producer1 = fork()) == 0)
    {
        // insert cpital letter
        while (true)
        {
            srand((unsigned)time(NULL));
            sleep(2 + (rand() % 100) / 100.0);
            down(empty);
            down(mutex);
            // insert capital letter
            srand((unsigned)time(NULL));
            char letter = 'A' + rand() % 26;
            insert(buffer, letter, 1);
            up(mutex);
            up(full);
        }
    }
    
    //producer2
    else if ((producer2 = fork()) == 0)
    {
        while (true)
        {
            srand((unsigned)time(NULL));
            sleep(2 + (rand() % 100) / 100.0);
            down(empty);
            down(mutex);
            // insert primary letter
            srand((unsigned)time(NULL)+1);
            char letter = 'a' + rand() % 26;
            insert(buffer, letter, 2);
            up(mutex);
            up(full);
        }
    }
    
    //consumer1
    else if ((consumer1 = fork()) == 0)
    {
        while (true)
        {
            srand((unsigned)time(NULL));
            sleep(3 + (rand() % 100) / 100.0);
            down(full);
            down(mutex);
            removeAny(buffer, 1);
            up(mutex);
            up(empty);
        }
    }
    
    //consumer2
    else if ((consumer2 = fork()) == 0)
    {
        while (true)
        {
            srand((unsigned)time(NULL));
            sleep(3 + (rand() % 100) / 100.0);
            down(full);
            down(mutex);
            removeAny(buffer, 2);
            up(mutex);
            up(empty);
        }
    }
    
    //consumer3
    else if ((consumer3 = fork()) == 0)
    {
        while (true)
        {
            srand((unsigned)time(NULL));
            sleep(3 + (rand() % 100) / 100.0);
            down(full);
            down(mutex);
            removeAny(buffer, 3);
            up(mutex);
            up(empty);
        }
    }
    
    //parent process
    else
    {
        while (true)
        {
            down(mutex);
            cout << "Current letters: ";
            for (int i = 0; i < MAXSIZE; i++)
            {
                cout << buffer[i];
            }
            cout << endl;
            up(mutex);
            sleep(10);
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
        perror("set the mutex error\n");
        exit(-1);
    }
}

//insert a random letter into buffer
void insert(char *buffer, char letter, int id) {
    for (int i = 0; i < MAXSIZE; i++)
    {
        if (buffer[i] == ' ')
        {
            buffer[i] = letter;
            cout << "producer" << id << " insert "
            << letter << " from buffer" << endl;
            break;
        }
    }
}

void removeAny(char *buffer, int id) {
    char letter = ' ';
    for (int i = 0; i < MAXSIZE; i++)
    {
        if ((buffer[i] >= 'A' && buffer[i] <= 'Z')
            || (buffer[i] >= 'a' && buffer[i] <= 'z'))
        {
            letter = buffer[i];
            buffer[i] = ' ';
            break;
        }
    }
    cout << "consumer" << id << " removes "
    << letter << " from buffer" << endl;
}

