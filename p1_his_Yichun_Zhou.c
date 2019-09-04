/*CS 425 Project 1: Unix Shell and History Feature
 ######## a Unix Shell with history feature ########
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
//#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


#define MAX_LINE		80 /* 80 chars per line, per command */
#define MAX_ARGS        40
#define BUFFER_SIZE     50
#define HISTORY_SIZE    10
int print_history();
char buffer[BUFFER_SIZE];
char *history[HISTORY_SIZE];
//char *check_r[MAX_ARGS];
int historySize = 0;
// int r_size = 0;
//char fileLocation[] = "./sh.history"; //file store and read the history

struct command {
  int argc;              
  char *argv[MAX_ARGS];   // arguments list
  char *hist[HISTORY_SIZE];
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
            exit(0);
        }
        //printf("execvp() successuly\n");
    } 

    else if(pid<0){
            printf("fork ERROR");
            //break;
        exit(0);
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
    char *hline = copyargs;                 
    char *end_args;                          
    char *endline;                        
    int check_background;   
    const char split[10] = " \t\r\n";   // split args        

 
    if (args == NULL) {
        printf("command line is NULL\n");
        exit(0);
        //break;
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

    cmd->argv[cmd->argc] = NULL;
    //save_history();
    if (cmd->argc == 0){  // if it is blank
        return 1;
    }

    //if it is in the background
    if ((check_background = (*cmd->argv[cmd->argc-1] == '&')) != 0){
        cmd->argv[--cmd->argc] = NULL;
       // save_history();
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

    else {    
        //save_history();
        SysCmd(&cmd,check);
    }
}

void handle_SIGINT(){
    //char input[MAX_LINE/2 + 1];
    
    write(STDOUT_FILENO, buffer, strlen(buffer));
    //exit(0);
    print_history();
    
    // if(fgets(input, MAX_LINE, stdin) != NULL){
    // if_r(input);
    // }
}

int print_history(){
    char index[MAX_LINE];
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    sigaction(SIGINT, &handler, NULL);

    printf("\nHISTORY:\n");
    int i;
    int j;
    
    for (i = 0; i< 10;i++)
    {
        if(history[i] == '\0') {
            return 0;
        }

        // if(historySize < 10){
        //     j = historySize % 10;
        // }
        // if(historySize >= 10){
            j = (historySize - 10) % 10;
        //}
        
        //j = historySize % 10;
        //command index
        //printf("%d.  ", hist_index);	
		//printing command
            //printf(history[(j+i) % 10],"this line\n");

            if(historySize <= 10){
             //printf("here1");

            printf("%02d. %s", i+1, history[i]);
               // sprintf(index, "%d", historySize); 
                //printf(index,".");
                //printf(history[i],"this line\n");
                //printf("test%d", historySize);
               
            }
            else if(historySize > 10){
             printf("%02d. %s", (j+i+1), history[(j+i) % 10]);
             //printf("here2\n");
                //sprintf(index, "%d", historySize); 
                //printf(index, "%s. ");
                //printf(history[(j+i) % 10],"this line\n");
  
            }
            
    }
    //check_r();
    
    return 0;
}

// int if_r(char *input){
            
//     // char input[MAX_LINE/2 + 1];
//     // fgets(input, MAX_LINE, stdin);

//     if(strncmp(input[0], "r", 1) == 0 ){
//             printf("rrrrrr");
//             implement(history);
            
//     }  
//     return 0;
// }


int main(void){
	char args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    int should_run = 1;
	int i, upper;
    
    int status_ptr;
	//char history[HISTORY_SIZE][BUFFER_SIZE];

    /* set up the signal handler */
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    sigaction(SIGINT, &handler, NULL);
    /* generate the output message */
    strcpy(buffer, "\nCaught Control C\n");
        
    /* loop until we receive <Control><C> */    
    while (should_run){   
        printf("zycosh>");
        //fflush(stdout);
        
        if ((fgets(args, MAX_LINE, stdin) == NULL) && ferror(stdin)){
            printf("getting input ERROR");
            break;
        }

        
        if(strcmp(args, "exit\n")==0) {
            break;
        }

        if((strcmp(args, "\n") != 0)){
        history[historySize % 10] = strdup(args);
        historySize++;
        
        }

        implement(args);


        //save_history(history[historySize]);
        
    /**
     * fork() & execvp() in SysCmd function
     */
    }
    
	return 0;
}