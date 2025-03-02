#include "systemcalls.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

bool do_system(const char *cmd)
{
    int status = system(cmd);
    
    // system() returns -1 on error in invocation
    if (status == -1) {
        return false;
    }
    
    // Check if command executed successfully (return value 0)
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

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

    pid_t pid = fork();
    
    if (pid == -1) {  // Fork failed
        va_end(args);
        return false;
    }
    
    if (pid == 0) {  // Child process
        execv(command[0], command);
        // If execv returns, it failed
        _exit(1);  // Exit child process with error
    }
    
    // Parent process
    int status;
    waitpid(pid, &status, 0);
    
    va_end(args);
    
    // Check if child exited normally and with success (0)
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

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

    pid_t pid = fork();
    
    if (pid == -1) {  // Fork failed
        va_end(args);
        return false;
    }
    
    if (pid == 0) {  // Child process
        // Open file for redirection
        int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            _exit(1);
        }
        
        // Redirect stdout to file
        if (dup2(fd, STDOUT_FILENO) == -1) {
            close(fd);
            _exit(1);
        }
        
        close(fd);  // No longer needed
        
        execv(command[0], command);
        _exit(1);  // If execv returns, it failed
    }
    
    // Parent process
    int status;
    waitpid(pid, &status, 0);
    
    va_end(args);
    
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}