/*
 ============================================================================
 Name        : 660__Lab04.c
 Author      : Michael Kusold
 Version     : 0.3
 Description : This program is an extension of 660_Lab03.c which was an
 extension of 660_Lab01. It implements a persistent state
 history file. The history file location is ./kusold.history.
 The history file is saved immediately before the user exits
 the program by ^D. If the program is terminated in another
 fashion, no guarentees are made about the state of the
 history file.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
 * Constant Declarations
 */
#define MAX_LINE 80
#define BUFFER_SIZE 80
#define HIST_SIZE 10

static char buffer[BUFFER_SIZE];
char history[HIST_SIZE][BUFFER_SIZE];
int count = 0;
int caught = 0;
char historyFileLoc[] = "./kusold.history";

void loadHistory() {
	int i;
	char histCommand[BUFFER_SIZE];
	
	i = 0;
	FILE *hisFile = fopen(historyFileLoc, "r");
	
	if( hisFile ) {
		/*If a user edits the history file, only the first ten entries will be loaded */
		while(!feof(hisFile)) {
			strcpy(histCommand, "\0");
			fgets(histCommand, MAX_LINE, hisFile);
			if (strcmp(histCommand, "\0") != 0) {
				strcpy(history[i], histCommand);
			}
			i++;
			count++;
		}
	}
	
	if(hisFile != NULL){
		if(fclose(hisFile) != 0) {
			perror("History file (r) was not closed correctly");
		}
	}
}

void saveHistory() {
	int i;
	
	FILE *hisFile = fopen(historyFileLoc, "w");

/* Writes the history to hisFile */
	for(i=0; i < HIST_SIZE; i++){
			strcpy(buffer, history[i]);
			if(strcmp(buffer,"\0") != 0) {
				fprintf(hisFile,"%s", buffer);
			}
	}
	if(fclose(hisFile) != 0) {
		perror("History file was not closed correctly");
	}
}


void printHistory() {
	int i;
	int j = 0;
	int histcount = count;
	
	printf("\n");
	for (i=0; i < HIST_SIZE; i++) {
		printf("%d.   ",histcount); /* Used to print the correct hitory number */
		while (history[i][j] != '\n' && history[i][j] != '\0') {
			printf("%c",history[i][j]);
			j++;
		}
		printf("\n");
		j=0;
		
		histcount--;
		if(histcount == 0) {
			break;
		}
	}
	printf("\n");
	printf("sh -> ");
}

/* the signal handler function */
void handle_SIGINT() {
	write(STDOUT_FILENO,buffer,strlen(buffer));
	printHistory();
	caught = 1;
}

void setup(char inputBuffer[], char *args[], int *background) {
	int length, /* # of characters in the command line */
	i, /* loop index for accessing inputBuffer array */
	start, /* index where beginning of next command parameter is */
	ct, /* index of where to place the next parameter into args[] */
	k; /* Generic counter */
	
	ct = 0;
	
	/* read what the user enters on the command line */
	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
	
	if(caught == 1) {
		length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
		caught = 0;
	}
	
	/* checks to see if the command is a history retrieval command. If it isn't then add it to the history */
	if((strcmp(inputBuffer, "r\n\0") != 0) && (strncmp(inputBuffer, "r x", 2) != 0) ) {
		for(i= (HIST_SIZE - 1); i>0; i--) {
			strcpy(history[i], history[i-1]);
		}
		strcpy(history[0], inputBuffer);
		count++;
	}
	start = -1;
	if (length == 0) {
		saveHistory();
		exit(0); /* ^d was entered, end of user command stream */
	} else if ((length < 0) && (errno != EINTR)) {
		perror("error reading the command");
		saveHistory();
		exit(-1); /* terminate with error code of -1 */
	}
	
	/* Checks to see if r was entered. If so, it copies the command most recently in the input buffer */
	if(strcmp(inputBuffer, "r\n\0") == 0) {
		strcpy(inputBuffer,history[0]);
		/* Checks to see if r x was entered. If so then it searches for the most recent command that begins with x */	
	} else if(strncmp(inputBuffer, "r x", 2) == 0) {
		for(k=0; k<10; k++){
			if(inputBuffer[2] == history[k][0]) {
				strcpy(inputBuffer,history[k]);
				break;
			}
		}
	}
	
	length = strlen(inputBuffer);
	
	/* examine every character in the inputBuffer */
	for (i = 0; i < length; i++) {
		switch (inputBuffer[i]) {
			case ' ':
			case '\t': /* argument separators */
				if (start != -1) {
					args[ct] = &inputBuffer[start]; /* set up pointer */
					ct++;
				}
				inputBuffer[i] = '\0'; /* add a null char; make a C string */
				start = -1;
				break;
				
			case '\n': /* should be the final char examined */
				if (start != -1) {
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';
				args[ct] = NULL; /* no more arguments to this command */
				break;
			case '&':
				*background = 1;
				inputBuffer[i] = '\0';
				break;
				
			default: /* some other character */
				if (start == -1) {
					start = i;
				}
		}
		args[ct] = NULL; /* just in case the input line was > 80 */
	}
}


/*
 * The main() function presents the prompt "sh>" and then invokes setup(), which waits for the
 *  user to enter a command. The contents of the command entered by the user are loaded into the
 *  args array. For example, if the user enters ls -l at the COMMAND-> prompt, args[0] will be set
 *  to the string ls and args[1] will be set to the string Ðl. (By ÒstringÓ, we mean a
 *  null-terminated, C-style string variable.)
 */
int main(void) {
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background, status,i; /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE / 2 + 1]; /* command line arguments */
	pid_t pid; /* the process's id */
	
	for(i=0; i < HIST_SIZE; i++){
		memset(history[i],'\0', sizeof(history[i]));
	}
	memset(inputBuffer, '\0', sizeof(inputBuffer));
	
	/* set up the signal handler */
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	sigaction(SIGINT, &handler, NULL);
	
	loadHistory();
	
	while (1) {
		background = 0;
		strcpy(inputBuffer, "\0");
		
		printf("\nsh ->");
		fflush(0);
		setup(inputBuffer, args, &background); /* get next command */
		fflush(0);
		pid = fork(); /* assign the process id */
		if (pid < 0) {
			fprintf(stderr, "ERROR: Could not properly fork.");
			saveHistory();
			exit(-1); /* unsucessful exit because the fork could not be created */
		} else if(pid == 0 ){ /* PID was forked successfully */
			status = execvp(*args, args); /* execute the command */
			if (status < 0) {
				fprintf(stderr, "ERROR: Could not execute %s", args[0]);
				saveHistory();
				exit(1);
			}
		} else if (background == 0) { /* if the fork is run in the foreground */
			wait(NULL);
		}
	}
	return EXIT_SUCCESS;
}