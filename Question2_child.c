#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int counter = 0;
    int cycle = 0;

    while (1) {
        printf("Cycle number: %d", cycle);

        if (counter % 3 == 0) {
            printf(" - %d is a multiple of 3\n", counter);
        } else {
            printf("\n");
        }

        counter--;    // decrementing counter 
        cycle++;
        usleep(2000000);  // 2 seconds
    }

    return 0;
}
