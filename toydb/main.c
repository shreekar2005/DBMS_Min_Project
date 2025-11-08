#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork Failed\n");
        return 1;
    } else if (pid == 0) {
        // This is the child process
		execlp("make", "make", "am_test1", NULL);
		perror("execlp failed"); 
		exit(EXIT_FAILURE); 
    } else {
        wait(NULL);
    }

    return 0;
}