#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int counter = 0;
    int cycle = 0;

    while (1) {
        printf("Child cycle %d: %d\n", cycle, counter);

        counter--;  
        cycle++;

        usleep(2000000);  // A 2 second delay 

        if (counter < -500) {
            printf("Child reached below -500. Exiting...\n");
            exit(0);      // Child ends → parent detects using wait()
        }
    }

    return 0;
}
