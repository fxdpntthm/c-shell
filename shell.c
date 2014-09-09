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

void printPrompt(void);
void acceptCommand(char *);
void printOptions(char *optionVector[MAXBUF], char *outputRedirectionVector[MAXBUF], char *inputRedirectionVector[MAXBUF]);
void printCommand(char *commandVector[MAXBUF]);
char cwd[MAXBUF];
const char HOME[] = "/a/byn1f2/vol/byn1f2v13/u_t1322070295/apoorvi/";

int main(){
    
    /*go on forever or ctrl-C*/
    while(1){
        char commandString[MAXBUF], *commandVector[MAXBUF], *commandPtr;
        char *optionVector[MAXBUF], *optionPtr, 
             *outputRedirectionVector[MAXBUF], *inputRedirectionVector[MAXBUF];
        int process_status, status, commandCount = 0, optionCount = 0, outputRedirectionCount = 0, inputRedirectionCount = 0, i;
        pid_t pid;
        int fdout, fdin;

        /*print prompt*/
        printPrompt();
        
        /*get command from user*/
        acceptCommand(commandString);

	/*ignore on \n or on \0 */
	if(commandString[0] == '\n' || commandString[0] == '\0'){
	    continue;
	}
	/*tokenize on pipe*/
	commandPtr = strtok(commandString, "|");
	commandVector[commandCount++] = commandPtr;
	while(commandPtr){
	    commandPtr = strtok(NULL, "|");
	    if(commandPtr){
		commandVector[commandCount++] = commandPtr+1;	
	    }	
	}
	commandVector[commandCount] = NULL;
	printCommand(commandVector);
        /* if command is exit quit program */
        if(!strcmp("exit", commandVector[0])){ 
            exit(0);
        }
	if(*commandVector[0] == '\n' || *commandVector[0] == '\0'){
	   /* printf("No command entered\n");*/
	    continue;
	}
	/* Run piping for commandcount - 1 times */
	for(i=0; i < commandCount - 1; i++){
	    optionPtr = strtok(commandVector[i], " ");
	    *optionVector = optionPtr;
	    optionCount++;
	    while(optionPtr){
		
		optionPtr = strtok(NULL, " ");
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
	    inputRedirectionVector[inputRedirectionCount] = NULL;
	    printOptions(optionVector, outputRedirectionVector, inputRedirectionVector);

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
		status = 0;
		process_status = 0;
		printf("forking process...\n"); 
		/*check pid and exec*/
		if(pid == 0){
		    /*this is the child process*/
		    /*check if output redirection is necessary*/
		    printf("Checking if redirection is necessary...\n");
		    if(*outputRedirectionVector){
			printf("Redirecting output...\n");
			printf("Opening file %s...", *outputRedirectionVector);
			fdout = open(*outputRedirectionVector, O_WRONLY| O_CREAT | O_TRUNC, 0644);
			status = dup2(fdout, STDOUT);
			if(status == -1){
			    printf("Error opening file");
			}
		    }
		    
		    if(*inputRedirectionVector){
			printf("Redirecting input...\n");
			printf("Opening file %s", *inputRedirectionVector);
			fdin = open(*inputRedirectionVector, O_RDONLY);
			status = dup2(fdin, STDIN);
			if(status == -1){
			    printf("Error opening file");
			}
		    }
		    if(status != -1){
			execvp(optionVector[0], optionVector);
			perror("Error!");
		    }
		}
		/*wait till child exits*/
		pid = wait(&process_status);
	    }
	}	
	   
    }/*looping till infinity*/
    return 0;
}

void printPrompt(){
    printf("shell$ ");
}

void acceptCommand(char *commandPtr){
    char c;
    /*clean buffers*/
    int i;
    for(i=0;i<MAXBUF;i++){
        commandPtr[i] = '\0';
    }
    while(c=getchar(), c!='\n'){
        *commandPtr++ = c;
    }
    commandPtr = '\0';
}

void printOptions(char *optionVector[MAXBUF], char* outputRedirectionVector[MAXBUF], char *inputRedirectionVector[MAXBUF]){
    printf("\n--------------------------\n");
    printf("printing tokens: ");
    if(optionVector){
	while(*optionVector){
	    printf("%s;", *optionVector++);
	}
    }
    printf("\n");
    printf("\nprinting output redirection tokens: ");
    if(outputRedirectionVector){
	while(*outputRedirectionVector){
	    printf("%s ; ", *outputRedirectionVector++);
	}
    }
    printf("\n");
    printf("\nprinting intput redirection tokens: ");
    if(inputRedirectionVector){
	while(*inputRedirectionVector){
	    printf("%s ; ", *inputRedirectionVector++);
	}
    }
    printf("\n--------------------------\n");
}

void printCommand(char *commandVector[MAXBUF]){
    printf("\n++++++++++++++++++++++++\n");
    printf("printing command vectors...\n");
    if(commandVector){
	while(*commandVector){
	    printf("%s ; ", *commandVector++);
	}
    }
    printf("\n++++++++++++++++++++++++\n");
}
