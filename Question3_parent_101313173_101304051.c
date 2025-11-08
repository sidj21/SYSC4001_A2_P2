#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>     // for wait() system call 

int main() {
    pid_t pid = fork();   // Create child process

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // CHILD: exec into Program 2
        execl("./Q3_c", "Q3_c", NULL);

        // Only runs if exec fails
        perror("exec failed");
        exit(1);
    }

 
    // --------------------PARENT PROCESS (Process 1)----------------------

    int counter = 0;
    int cycle = 0;

    printf("Parent waiting for child to reach -500...\n");

    // Parent prints multiples of 3 until child finishes
    while (1) {
        printf("Cycle number: %d", cycle);

        if (counter % 3 == 0) {
            printf(" - %d is a multiple of 3\n", counter);
        } else {
            printf("\n");
        }

        counter++;
        cycle++;

        usleep(2000000); // 2 second delay 

        // Check if child has finished yet
        pid_t result = waitpid(pid, NULL, WNOHANG);
        // waitpid(pid of process waiting for, return values of process (in our case we don't need it), option to block or not block the current process from running during these checks)


        if (result == pid) {
            // Child finished
            printf("Child has finished (counter < -500). Parent exiting...\n");
            exit(0);
        }
    }

    return 0;
}

