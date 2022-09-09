#include "systemcalls.h"
#include "stdlib.h"
#include "unistd.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    //holds the return code
    int rc;

    rc = system(cmd);

    if(rc == -1){
        return false;
    }

    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    //get parent pid
    //pid_t parent = getpid();

///*

    //fork
    pid_t pid = fork();
    

    if(pid == -1){
        //failed to make child
        //printf("\n\n\npid fork error\n\n\n");
        return false;
    }
    else if (pid > 0){
        //this is the parent so wait
        int parent_rc;
        wait(&parent_rc);
        if(parent_rc != 0){
            printf("\nCommand returned non-zero exit code");
            return false;
        }
        else{
            return true;
        }
    }
    else{
        //this is the child
        //do the execv
        int child_rc;
        
        //child_rc = execv(command[0], &command[1]);
        child_rc = execv(command[0], command);
        //printf("\nexecv return value: %d\n", child_rc);
        //if(child_rc != 0){
            //execv shouldn't return so if we get here error
            printf("\n*** ERROR: exec failed with return value %d\n", child_rc);
            
            //return false;
            //TA suggested use exit(-1)
            exit(-1);
        //}
    }

//*/

    va_end(args);
    //printf("\nreturning true\n");
    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/


   
    //redirect the output
    //printf("\n\n\n opening file\n\n\n");
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if(fd < 0){
        printf("\n\n\n error opening file\n\n\n");
        return false;
    }

    //fork
    pid_t pid = fork();

    if(pid == -1){
        //failed to make child
        return false;
    }
    else if (pid > 0){
        //close fd
        close(fd);
        //this is the parent so wait
        int parent_rc;
        wait(&parent_rc);
        if(parent_rc != 0){
            printf("\nCommand returned non-zero exit code");
            return false;
        }
    }
    else{
        //this is the child

        //redirect
        if(dup2(fd,1) < 0){
            printf("\n\n\ndup2 error\n\n\n");
            return false;
        }

        close(fd);

        //do the execv
        int child_rc = 0;
        
        //child_rc = execv(command[0], &command[1]);
        child_rc = execv(command[0], command);
        //printf("\nexecv return value: %d\n", child_rc);
        if(child_rc != 0){
            //execv shouldn't return so if we get here error
            printf("\n*** ERROR: exec failed with return value %d\n", child_rc);
            
            return false;
        }
    }

    va_end(args);

    return true;
}
