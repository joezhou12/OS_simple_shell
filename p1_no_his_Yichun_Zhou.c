/*CS 425 Project 1: Unix Shell and History Feature
 ######## a Unix Shell without a history feature ########
Yichun Zhou
WKUID:801050675
yichun.zhou474@topper.wku.edu

*/

/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>


#define MAX_LINE		80 /* 80 chars per line, per command */
#define MAX_ARGS        40

struct command {
  int argc;              
  char *argv[MAX_ARGS];   // arguments list
};

void SysCmd(struct command *cmd, int check){

    pid_t pid;
    int check_execvp;

    /**
     * After reading user input, the steps are:
     * (1) fork a child process
     * (2) the child process will invoke execvp()
     * (3) if command included &, parent will invoke wait()
     */

    pid = fork();

    if(pid==0){
        check_execvp = execvp(cmd->argv[0],cmd->argv);
        if(check_execvp < 0){
            printf("ERROR! CANNOT FIND COMMAND %s\n",cmd->argv[0]);
        }
        //printf("execvp() successuly\n");
    } 

    else if(pid<0){
            printf("fork ERROR");
        //exit(0);
    } 
    
    else{
        if(check){
            printf("BACKGROUND: GET CHILDREN pid =[%d]\n",pid);
        }

    else{
        wait(&pid);
        }

    }
}

int parse(char *args,struct command *cmd){//passing the input
    static char copyargs[MAX_LINE];          
    char *line = copyargs;                   
    char *end_args;                          
    char *endline;                        
    int check_background;                           

    const char split[10] = " \t\r\n";   // split args

    if (args == NULL) {
        printf("command line is NULL\n");
    }

    (void) strncpy(line, args, MAX_LINE);
    endline = line + strlen(line);

    cmd->argc = 0;
    while (line < endline) {
        
        line += strspn (line, split);
        if (line >= endline) {
            break;
        }
        end_args = line + strcspn (line, split);
        *end_args = '\0';
        cmd->argv[cmd->argc++] = line;

        if (cmd->argc >= MAX_ARGS-1){
            break;
        }

        line = end_args + 1;
    }
    //set a pointer
    cmd->argv[cmd->argc] = NULL;

    if (cmd->argc == 0){  // if it is blank
        return 1;
    }

    //if it is in the background
    if ((check_background = (*cmd->argv[cmd->argc-1] == '&')) != 0){
        cmd->argv[--cmd->argc] = NULL;
    }

    return check_background;
}

void implement(char *args){
    int check;
    struct command cmd;

    //printf("input = '%s'\n",args);
    check = parse(args, &cmd);

    if(check ==-1) {
        return   ;
    }

    if(cmd.argv[0]==NULL){
        return;
    }

    else {    // cmd.builtin == NONE)
        SysCmd(&cmd,check);
    }
}

int main(void){
	char args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    int should_run = 1;
	//int i, upper;

    int status_ptr;
		
    while (should_run){   
        printf("zycosh>");
        //fflush(stdout);
        
        if ((fgets(args, MAX_LINE, stdin) == NULL) && ferror(stdin)){
            printf("getting input ERROR");
        }

        if(strcmp(args, "exit\n")==0) {
            break;
        }

        implement(args);
        
    /**
     * fork() & execvp() in SysCmd function
     */
    }
    
	return 0;
}