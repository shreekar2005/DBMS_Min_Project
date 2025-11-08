#include <stdio.h>
#include <unistd.h> // For fork()
#include <stdlib.h>
#include <sys/types.h> // For pid_t
#include <sys/wait.h> // For wait()

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork Failed\n");
        return 1;
    } else if (pid == 0) {
        // This is the child process
        // CORRECT
		execlp("make", "make", "am_test1", NULL);
		perror("execlp failed"); 
		exit(EXIT_FAILURE); 

    } else {
        printf("Parent process: My PID is %d, My child's PID is %d\n", getpid(), pid);
        wait(NULL); // Parent waits for the child to complete
        printf("Parent process: Child has finished execution.\n");
    }

    return 0;
}