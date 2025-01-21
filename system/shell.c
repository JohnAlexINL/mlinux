// https://github.com/nir9/welcome/blob/master/lnx/very-minimal-shell/shell.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h> // ls
#include "share/file.c"

extern char **environ; // ??

const char *cmd_cd = "cd";
const char *cmd_clear = "clear";
const char *cmd_exit = "exit";
const char *cmd_ls = "ls";
const char *cmd_pid = "pid";

void prenv(char **args, char **environ) {
    int i;
    printf("Process Arguments;\n");
    for (i=0; args[i]!=NULL; i++) { printf("   ... %s\n", args[i]); }
    printf("Environment variables\n");
    for (i=0; environ[i]!=NULL; i++) { printf("   ... %s\n", environ[i]); }
}

char *bwargs[255];
char **builtin_binargs(char *command){
    int i; int count = 0; int len = strlen(command);
    for(i=0;i<len;i++) {
        if (command[i] == ' ') { command[i]='\0'; bwargs[count+1] = (void*)command+(i+1); count++; }
    }   bwargs[count+1] = NULL;
    bwargs[0] = (void*)command;
    return bwargs;
}

char *builtin_binpath(char *binary, char *cwd) {
    char *result;
    char trypath[512];
    result = malloc(sizeof(trypath)); 
    // first strip name at space or eol
    int i; for (i=0;binary[i]!='\0';i++) {
        if (binary[i]==' ') {
            binary[i] = '\0'; break;
        }
    }
    
    // try literal
    if ( file_exists(binary)) {
        strncpy(result, trypath, sizeof(trypath));
        return result;
    }
    // try in bin
    sprintf(trypath, "/bin/%s", binary); if ( file_exists(trypath)) {
        strncpy(result, trypath, sizeof(trypath));
        return result;
    }
    // try local
    sprintf(trypath, "%s/%s", cwd, binary); if ( file_exists(trypath)) {
        strncpy(result, trypath, sizeof(trypath));
        return result;
    }
    return NULL;
}

// partially AI generated function; replace later with tidier code
void list_directory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror(path);
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if ( strcmp(entry->d_name, ".")==0 ) { continue; }
        if ( strcmp(entry->d_name, "..")==0 ) { continue; }
        printf("%s\n", entry->d_name);
    }   closedir(dir);
    return;
}

void builtin_cd (char *command) {
    char *dir = command + strlen(cmd_cd) + 1; // ignore the space
    if ( chdir(dir) != 0 ) {
        printf("\e[0;31mERROR(\"cd %s\"): %s\n", dir, strerror(errno));
    }   return;
}

void builtin_clear() {
    int i; int j;
    for (i=0;i<80;i++) { 
    for (j=0;j<8;j++)  { 
        printf("    ");} 
        printf("\n");}
    return;
}

int main(int argc, char **argv) {
    pid_t pid = getpid(); 
    printf("\e[=3h"); // set color 80x25 text mode
    char context[512];
	char command[512];
    char cwd[512];
	while (1) {
        // Write the context info and get a command
        getcwd(cwd, sizeof(cwd));
        sprintf(context, "\e[0;32m%s\e[0m:\e[0;34m%s\e[0m$ ", getenv("USER"), cwd);
		write(1, context, strlen(context));
		int count = read(0, command, sizeof(command)); if ( count <= 0 ) { continue; }
		command[count - 1] = 0; // /bin/ls\n -> /bin/ls\0

        // Check for built-in commands like cd
        if(strncmp(command, cmd_exit, strlen(cmd_exit))==0) { exit(0); }
        if(strncmp(command, cmd_cd, strlen(cmd_cd))==0) { builtin_cd(command); continue; }
        if(strncmp(command, cmd_ls, strlen(cmd_ls))==0) { list_directory(cwd); continue; }
        if(strncmp(command, cmd_clear, strlen(cmd_clear))==0) { builtin_clear(); continue; }
        if(strncmp(command, cmd_pid, strlen(cmd_pid))==0) { printf("current process id %d\n", pid); continue; }
        // Fork the process and try to run the command
		pid_t pid = fork();
		if (pid == 0) {
            int code;
            char **args = builtin_binargs(command);
            char *path = builtin_binpath(command, cwd); if ( path == NULL ) { printf("\e[0;31mFile not found\n"); exit(1); }
            // prenv(args, environ);
            code = execve(path, args, environ); if ( code == -1 ) {
                free(path);
                printf("\e[0;31m%s: \e[0m%s\n", path, strerror(errno));
                exit(1);
            }
			break;
		} else if ( pid > 0 ) {
			siginfo_t info;
			waitid(P_ALL, 0, &info, WEXITED);
		} else {
            printf("\e[0;31munable to fork process\e[0m\n");
            exit(1);
        }
	}
    exit(0);
}