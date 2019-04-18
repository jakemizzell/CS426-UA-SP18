#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE 80

//this is to hold the past commands and to execute previous ones
char history[100000][MAX_LINE];
//how many commands you've entered
int count = 0;

void addToHistory(char *);
int commands(char *, char **,int *);

int main(void) {
    char inputBuffer[MAX_LINE];
    int background;
    char *args[MAX_LINE/2 + 1];
    pid_t pid;
    int shouldrun = 1;
    while (shouldrun){
        background = 0;
        printf("osh>");
        fflush(stdout);
        shouldrun = commands(inputBuffer,args,&background);
        //will exit the program
        if (strcmp(inputBuffer, "exit") == 0)
            exit(0);
        //prints the history
        else if (strcmp(inputBuffer,"history") == 0) {
            if(count == 0)
                printf("There is no history!\n");
            int i,j;
            //prints out the history
            for (i = count, j = 0; i > 0 && j < 10; i--, j++)
                printf("%d \t %s", i, history[i]);
            continue;
        }
        if (shouldrun) {
            pid = fork();
            if (pid < 0) {
                printf("Fork Failed\n");
                exit(1);
            }
            else if (pid == 0) {
            	//checking if the argument is a valid one
            	int run = execvp(args[0],args);
                //added the "history" strcmp because sometimes if you typed history it would cause the message readout
                if ((run < 0) && (strcmp(inputBuffer,"history") != 0)){
                    printf("Failed to run command.\n");
                    exit(1);
                }
            }
            else {
                if (background == 0)
                    while (pid != wait(0));
            }
        }
    }
    return 0;
}

int commands(char inputBuffer[], char *args[],int *background) {
	int pastCommand = 0;
    int next = 0;
    //read the arguments
    int length = read(STDIN_FILENO,inputBuffer,MAX_LINE);
    int c = -1;
    if (length == 0)
        exit(0);
    //if there is an ! in the command
    if (inputBuffer[0] == '!') {
        if (inputBuffer[1] == '!') {
            //if there have been no commands yet
            if(count == 0) {
                printf("No commands in history\n");
                return 1;
            }
            //copy that command into the inputBuffer so you can run it later
            strcpy(inputBuffer,history[count]);
            //turn on pastCommand so it's printed after being cleaned
            pastCommand = 1;
            length = strlen(inputBuffer) + 1;
        }
        //if there is a number after the !
        else if (isdigit(inputBuffer[1])) {
            //if the number is greater than the number of commands
            if ((atoi(&inputBuffer[1]) > count) || (atoi(&inputBuffer[1]) <= 0)) {
                printf("No such command in history.\n");
                return 1;
            }
            //copy that command into inputBuffer
            strcpy(inputBuffer,history[atoi(&inputBuffer[1])]);
            //turn on pastCommand so it's printed after being cleaned
            pastCommand = 1;
            length = strlen(inputBuffer) + 1;
        }
    }
    //add whatever is in inputBuffer to your history array
    addToHistory(inputBuffer);
    //echo it if it was a past command
    if (pastCommand == 1) {
    	int i;
    	for (i = 0; inputBuffer[i] != '\0'; i++)
    		printf("%c",inputBuffer[i]);
    }
    //cleans up the commands
    int i;
    for (i=0;i<length;i++) {
    	//if its a space or tab
        if (inputBuffer[i] == ' ' || inputBuffer[i] == '\t') {
            if(c != -1){
                args[next] = &inputBuffer[c];
                next++;
            }
            inputBuffer[i] = '\0';
            c = -1;
        }
        //if its a newline
        else if (inputBuffer[i] == '\n') {
            if (c != -1){
                args[next] = &inputBuffer[c];
                next++;
            }
            inputBuffer[i] = '\0';
            args[next] = NULL;
        }
        //if its anything else
        else {
            if (c == -1)
                c = i;
            if (inputBuffer[i] == '&') {
                *background = 1;
                inputBuffer[i-1] = '\0';
            }
        }
    }
    //if & was used
    if (*background)
        args[--next] = NULL;
    args[next] = NULL;
    return 1;
}

void addToHistory(char inputBuffer[]) {
    //don't add "history" to history, strcmp didn't work for some reason
    if (inputBuffer[0] == 'h' && inputBuffer[1] == 'i' && inputBuffer[2] == 's' && inputBuffer[3] == 't' && inputBuffer[4] == 'o' && inputBuffer[5] =='r' && inputBuffer[6] == 'y')
        return;
    count++;
    //read the inputBuffer into the history array
    int i;
    for (i = 0; inputBuffer[i] != '\n' && inputBuffer[i] != '\0'; i++)
        history[count][i] = inputBuffer[i];
    //append a newline so commands can be called
    history[count][i] = '\n';
}