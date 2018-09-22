#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/sem.h>
#include <pthread.h>

using namespace std;

int reader_num = 0;
int writer_num = 0;

//define semaphores
int read_db = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
int write_db = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
int mutex_read = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
int mutex_write = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);

//down & up
int down(int sem_id);
int up(int sem_id);

void set_semaphore(int sem_id, int val);
void *reader(void *);
void *writer(void *);
pthread_t create_writer();
pthread_t create_reader();

int main() {
    set_semaphore(read_db, 1);
    set_semaphore(write_db, 1);
    set_semaphore(mutex_read, 1);
    set_semaphore(mutex_write, 1);
    pthread_t reader_first = create_reader();
    pthread_t reader_second = create_reader();
    sleep(1);
    pthread_t writer_first = create_writer();
    pthread_t reader_third = create_reader();
    pthread_t writer_second = create_writer();
    sleep(1);
    pthread_join(reader_first, NULL);
    pthread_join(reader_second, NULL);
    pthread_join(writer_first, NULL);
    pthread_join(reader_third, NULL);
    pthread_join(writer_second, NULL);
    return 0;
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

void *reader(void *) {
    down(mutex_write); //waiting until no writer is waiting
    up(mutex_write);
    down(mutex_read);
    reader_num++;
    if (reader_num == 1) {
        down(read_db);
    }
    up(mutex_read);
    cout << "reading..." << endl;
    sleep(2);
    cout << "finishes one reading!" << endl;
    down(mutex_read);
    if (reader_num == 1) {
        up(read_db);
    }
    reader_num--;
    up(mutex_read);
    return NULL;
}

void *writer(void *) {
    down(mutex_write);
    writer_num++;
    if (writer_num == 1) {
        down(read_db); // waiting untill no reader is reading
    }
    up(mutex_write);
    down(write_db);
    cout << "writing..." << endl;
    sleep(3);
    cout << "finishes one writing!" << endl;
    up(write_db);
    down(mutex_write);
    if (writer_num == 1) {
        up(read_db);
    }
    writer_num--;
    up(mutex_write);
    return NULL;
}

pthread_t create_writer() {
    pthread_t writer_id;
    if((pthread_create(&writer_id, NULL, writer, NULL)) != 0) {
        perror("create barber failed\n");
        exit(-1);
    }
    return writer_id;
}

pthread_t create_reader() {
    pthread_t reader_id;
    if((pthread_create(&reader_id, NULL, reader, NULL)) != 0) {
        perror("create barber failed\n");
        exit(-1);
    }
    return reader_id;
}
