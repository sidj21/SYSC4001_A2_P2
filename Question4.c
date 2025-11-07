// Code for part 4
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

struct SharedData {
    int multiple;   // shared multiple value
    int counter;    // shared counter updated by parent
};

int main() {
    int shm_id;
    struct SharedData *shared;

    // Create shared memory segment
    shm_id = shmget(IPC_PRIVATE, sizeof(struct SharedData), IPC_CREAT | 0666);
    // shm_id is the identifyer for the share memory segment
    // shmget is used to create a new shared memory segment or access an existing one
    // IPC_PRIVATE indicates a new private segment
    // sizeof(struct SharedData) specifies the size of the segment
    // IPC_CREAT | 0666 sets permissions to read and write for everyone
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }

    // Attach shared memory to this process
    shared = (struct SharedData *) shmat(shm_id, NULL, 0);
    //void *shmat(int shmid, const void *shmaddr, int shmflg); -> Syntax for refernce
    // shmat attaches the shared memory segment identified by shmid to the address space of the calling process
    // The second argument (NULL) lets the kernel choose the attachment address
    // The third argument is shmflg (0) indicates default behavior *
    // if shmat fails, it returns (void *) -1
    if (shared == (void *) -1) {
        perror("shmat failed");
        exit(1);
    }

    // Initialize shared memory variables
    shared->multiple = 3; // can be changed to test
    shared->counter = 0;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // ---------------- CHILD PROCESS ----------------
        while (shared->counter <= 500) {

            if (shared->counter <= 100) {
                // Child must wait until counter > 100
                usleep(100000);
                continue;
            }

            // Child prints status
            printf("[Child PID %d] Counter = %d (multiple = %d)\n",
                   getpid(), shared->counter, shared->multiple);

            usleep(200000); // Slow down
        }

        printf("[Child PID %d] Counter > 500. Child exiting.\n", getpid());

        // Detach shared memory
        shmdt(shared);
        exit(0);
    }

    else {
    // ---------------- PARENT PROCESS ----------------
        while (shared->counter <= 500) {

            // Only print multiples of 'multiple'
            if (shared->counter % shared->multiple == 0) {
                printf("[Parent PID %d] %d is a multiple of %d\n",
                       getpid(), shared->counter, shared->multiple);
            }

            shared->counter++;  // Update shared counter
            usleep(100000);
        }

        printf("[Parent PID %d] Counter > 500. Parent waiting for child...\n",
               getpid());

        wait(NULL);

        // Cleanup shared memory
        shmdt(shared); // detach shared memory
        shmctl(shm_id, IPC_RMID, NULL); // remove share memory segment 

        printf("[Parent PID %d] Shared memory removed. Parent exiting.\n",
               getpid());
    }

    return 0;
}
