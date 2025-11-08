// Code for part 5
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

struct SharedData {
    int multiple;   // shared multiple value
    int counter;    // shared counter updated by parent
};

// Define union semun (required by semctl)
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// Semaphore wait (P) operation
void sem_wait(int sem_id) {
    struct sembuf p = {0, -1, 0};
    semop(sem_id, &p, 1);
}

// Semaphore signal (V) operation
void sem_signal(int sem_id) {
    struct sembuf v = {0, 1, 0};
    semop(sem_id, &v, 1);
}

int main() {
    int shm_id, sem_id;
    struct SharedData *shared;

    // -------------------- Shared Memory --------------------
    shm_id = shmget(IPC_PRIVATE, sizeof(struct SharedData), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }

    shared = (struct SharedData *) shmat(shm_id, NULL, 0);
    if (shared == (void *) -1) {
        perror("shmat failed");
        exit(1);
    }

    shared->multiple = 3;   // Initialize multiple
    shared->counter = 0;    // Initialize counter

    // -------------------- Semaphore --------------------
    sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (sem_id < 0) {
        perror("semget failed");
        exit(1);
    }

    union semun sem_arg;
    sem_arg.val = 1;          // Binary semaphore
    semctl(sem_id, 0, SETVAL, sem_arg);

    // -------------------- Fork --------------------
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // -------------------- CHILD PROCESS --------------------
        while (1) {
            sem_wait(sem_id);  // Lock access

            if (shared->counter > 500) {  // Exit condition
                sem_signal(sem_id);
                break;
            }

            if (shared->counter > 100) {  // Start printing after 100
                if (shared->counter % shared->multiple == 0) {
                    printf("[Child PID %d] Counter = %d is a multiple of %d\n",
                        getpid(), shared->counter, shared->multiple);
                } else {
                    printf("[Child PID %d] Counter = %d\n",
                        getpid(), shared->counter);
                }
                fflush(stdout); // ensure immediate output
            }

            sem_signal(sem_id);  // Unlock access
            usleep(200000);      // delay for readability
        }

        printf("[Child PID %d] Counter > 500. Child exiting.\n", getpid());
        fflush(stdout);

        shmdt(shared); // detach shared memory
        exit(0);
    }
    else {
        // -------------------- PARENT PROCESS --------------------
        while (1) {
            sem_wait(sem_id);  // Lock access

            if (shared->counter > 500) {  // Exit condition
                sem_signal(sem_id);
                break;
            }

            shared->counter++;  // Increment shared counter

            if (shared->counter % shared->multiple == 0) {
                printf("[Parent PID %d] %d is a multiple of %d\n",
                       getpid(), shared->counter, shared->multiple);
            } else {
                printf("[Parent PID %d] Counter = %d\n",
                       getpid(), shared->counter);
            }
            fflush(stdout);

            sem_signal(sem_id);  // Unlock access
            usleep(100000);      // small delay
        }

        wait(NULL);  // Wait for child to finish

        // -------------------- Cleanup --------------------
        shmdt(shared);               // detach shared memory
        shmctl(shm_id, IPC_RMID, 0); // remove shared memory
        semctl(sem_id, 0, IPC_RMID); // remove semaphore

        printf("[Parent PID %d] Shared memory removed. Parent exiting.\n", getpid());
        fflush(stdout);
    }

    return 0;
}
