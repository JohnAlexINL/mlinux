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

const char *cmd_cd = "cd";
const char *cmd_clear = "clear";
const char *cmd_exit = "exit";
const char *cmd_ls = "ls";

char *trypath(char *command, char *cwd) {
    char trypath[512]; char *result = malloc(sizeof(trypath));
    // try literal
    if ( file_exists(command)) {
        // technically redundant, 
        // would make free() easier, but free also never happens
        // bc the process should get replaced
        strncpy(result, trypath, sizeof(trypath));
        return command;
    }
    // try in bin
    sprintf(trypath, "bin/%s", command); if ( file_exists(trypath)) {
        strncpy(result, trypath, sizeof(trypath));
        return result;
    }
    // try local
    sprintf(trypath, "%s/%s", cwd, command); if ( file_exists(trypath)) {
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
    }
    closedir(dir);
}

int main(int argc, char **argv) {
    printf("\e[=3h"); // set color 80x25 text mode
    char context[512];
    char cwd[255];
	char command[255];
	while (1) {
        // Write the context info and get a command
        getcwd(cwd, sizeof(cwd));
        sprintf(context, "\e[0;32m%s\e[0m:\e[0;34m%s\e[0m$ ", getenv("USER"), cwd);
		write(1, context, strlen(context));
		int count = read(0, command, sizeof(command)); if ( count <= 0 ) { continue; }
		command[count - 1] = 0; // /bin/ls\n -> /bin/ls\0

        // Check for built-in commands like cd
        if(strncmp(command, cmd_exit, strlen(cmd_exit))==0) { exit(0); }
        if(strncmp(command, cmd_cd, strlen(cmd_cd))==0) {
            char *dir = command + strlen(cmd_cd) + 1; // ignore the space
            if ( chdir(dir) != 0 ) {
                printf("\e[0;31mERROR(\"cd %s\"): %s\n", dir, strerror(errno));
            }   continue;
        }
        if (strncmp(command, cmd_ls, strlen(cmd_ls))==0) {
            list_directory(cwd);
            continue;
        }
        if(strncmp(command, cmd_clear, strlen(cmd_clear))==0) {
            // printf("\e[H\e[J\e[2J");
            int i; int j;
            for (i=0;i<80;i++) {
            for (j=0;j<8;j++) {
                printf("    ");
            }   printf("\n");}
            // printf("\e[1;1H"); 
            continue;
        }
        // Fork the process and try to run the command
		pid_t pid = fork();
		if (pid == 0) {
            int code;
            char *args[] = { command, NULL };
            char *path = trypath(command, cwd);
            if ( path == NULL ) { printf("\e[0;31mFile not found\n"); exit(1); }
            code = execve(path, args, NULL);
            if ( code == -1 ) {
                free(path);
                printf("\e[0;31m%s: \e[0m%s\e[0m\n", path, strerror(errno));
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