#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<fcntl.h>

#define MAXBUF 100
#define STDIN 0
#define STDOUT 1
#define STDERR 2

void printPrompt();
void acceptCommand(char*, char*, char*);
void printOptions(char *optionVector[MAXBUF], char *outputRedirectionVector[MAXBUF]);

char cwd[MAXBUF];
const char HOME[] = "/a/byn1f2/vol/byn1f2v13/u_t1322070295/apoorvi/";

int main(){
    
    /*go on forever or ctrl-C*/
    while(1){
        char command[MAXBUF];
        char *optionVector[MAXBUF], *optionPtr, 
             *outputRedirectionVector[MAXBUF], *inputRedirectionVector[MAXBUF];
        int optionCount = 0, status, outputRedirectionCount=0, inputRedirectionCount = 0;
        pid_t pid;
        int fdout, fdin;
        /*print prompt*/
        printPrompt();
        
        /*get command from user*/
        acceptCommand(command, *outputRedirectionVector, *optionVector);

        /* if command is exit quit program */
        if(!strcmp("exit", command)){ 
            exit(0);
        }
        /*tokenize command*/	
        optionPtr = strtok(command, " ");
        *optionVector = optionPtr;
        optionCount++;
        while(optionPtr){
            
            optionPtr = strtok(NULL, " ");
            /* TODO
             * input redirect
             */
            
             /* check if > or < is present*/
            if(optionPtr){
                if(*optionPtr == '>'){
                    outputRedirectionVector[outputRedirectionCount++] = optionPtr + 1;
                }else if(*optionPtr == '<'){
                    inputRedirectionVector[inputRedirectionCount++] = optionPtr + 1;
                }else{ 
                    optionVector[optionCount++]= optionPtr;
                }
            }
        }
        optionVector[optionCount]= NULL;
        outputRedirectionVector[outputRedirectionCount] = NULL;

        printOptions(optionVector, outputRedirectionVector);
        /*check if fork is necessary*/
        printf("Checking if fork is necessary...\n");
        /*fork not necessary*/
        if(!strcmp(optionVector[0], "cd")){
            /*if cd has no argument land on home*/
            if(!optionVector[1]){
            chdir(HOME);
            strcpy(cwd,HOME);
            }else{
            /*check if its absolute path*/
            if(optionVector[1][0]=='/'){
                chdir(optionVector[1]);
                strcpy(cwd, optionVector[1]);
            }else{
                /*add absolute of current directory*/  
                char temp_address[200];
                strcpy(temp_address, cwd);
                strcat(temp_address, optionVector[1]);
                status = chdir(temp_address);
                perror("This is weird\n");
                if(status == -1){
                printf("cd: %s No such directory found\n", optionVector[1]);
                }
                strcpy(cwd, optionVector[1]);
            }
            }
        }else if(!strcmp(optionVector[0], "pwd")){
            getcwd(cwd, MAXBUF);
            perror("This is weird\n");
            printf("%s\n", cwd);    
        }else{
        /* fork is necessary */
            pid = fork(); 
            printf("forking process...\n"); 
            /*check pid and exec*/
            if(pid == 0){
                /*this is the child process*/
                /*check if output redirection is necessary*/
                printf("Checking if redirection is necessary...\n");
                if(*outputRedirectionVector){
                    printf("Redirecting output...\n");
                    printf("Opening file %s...", *outputRedirectionVector);
                    fdout = open(*outputRedirectionVector, O_WRONLY| O_CREAT | O_TRUNC);
                    dup2(fdout, STDOUT);
                }
                
                if(*inputRedirectionVector){
                    printf("Redirecting input...\n");
                    printf("Opening file %s", *inputRedirectionVector);
                    fdin = open(*outputRedirectionVector, O_RDONLY | O_CREAT);
                    dup2(fdin, STDIN);
                }
                execvp(optionVector[0], optionVector);
                perror("Error!");
            }
            /*wait till child exits*/
            pid = wait(&status);
        }
    }/*looping till infinity*/
    return 0;
}

void printPrompt(){
    printf("shell$ ");
}

void acceptCommand(char *commandPtr, char* optionVector, char *outputRedirectionVector){
    char c;
    /*clean buffers*/
    int i;
    for(i=0;i<MAXBUF;i++){
        commandPtr[i] = '\0';
        optionVector = NULL;
        outputRedirectionVector = NULL;
    }
    while(c=getchar(), c!='\n'){
        *commandPtr++ = c;
    }
    commandPtr = '\0';
}

void printOptions(char *optionVector[MAXBUF], char* outputRedirectionVector[MAXBUF]){
    printf("\n--------------------------\n");
    printf("printing tokens: ");
    while(*optionVector){
        printf("%s;", *optionVector++);
    }
    printf("\n");
    printf("\nprinting redirection tokens: ");
    while(*outputRedirectionVector){
        printf("%s;", *outputRedirectionVector++);
    }
    printf("\n");
    printf("--------------------------\n");
}
