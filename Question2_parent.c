#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child will exec Program 2
        execl("./Question2_child.c", "./Question2_child.c", NULL); // Fails and runs the next line which deals with the error
        // execl(path, arg0, arg1 ..., command line arguments); 
        // after exec is successful, the child process image is replaced by the new program
        // If execl fails:
        perror("execl failed");
        exit(1);
    }

    // Parent process runs its own infinite loop
    int counter = 0;
    int cycle = 0;

    while (1) {
        printf("Cycle number: %d", cycle);

        if (counter % 3 == 0) {
            printf(" - %d is a multiple of 3\n", counter);
        } else {
            printf("\n");
        }

        counter++;
        cycle++;
        usleep(2000000);  // 2 seconds
    }

    return 0;
}
