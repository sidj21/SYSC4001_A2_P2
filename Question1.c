// Code for part 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Used for any process related system calls  
 // usleep provides a delay in microseconds between each counter increment
int main() {
    pid_t pid; // pid_t is a data type for process IDs in Linux  
    
    pid = fork(); 
        if (pid < 0){
            perror("fork() failed"); 
            exit(1);
        }
        if (pid == 0){
            int count_child = 0; 
            while(1){
                printf("Child process counter: %d\n" , count_child++);  
                usleep(2000000); // Creates a delay of 2 secs
            }
        }
        else{
            int count_parent = 0;            
            while(1){
                printf("Parent process counter: %d\n", count_parent++); 
                usleep(2000000); // Creates a delay of 2 secs

            }
        }
    
    return 0;
}
