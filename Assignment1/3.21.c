#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Include the starting number as argument");
        exit(1);
    }
    pid_t pid;
    int n = atoi(argv[1]);
    pid = fork();
    if (pid == 0) {
        printf("%d, ", n);
        while (n != 1) {
            //if the number is even
            if (n % 2 == 0)
                n = n/2;
            //if the number is odd
            else if (n % 2 == 1)
                n = 3 * n + 1;
            //if it's 1 then you aren't printing another number so print newline
            if (n == 1)
                printf("%d\n",n);
            // otherwise print a comma
            else
                printf("%d, ",n);
        }
    }
    else
        wait(0);
    return 0;
}