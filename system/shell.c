// https://github.com/nir9/welcome/blob/master/lnx/very-minimal-shell/shell.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

const char *cmd_cd = "cd";
const char *cmd_clear = "clear";
const char *cmd_exit = "exit";

int main(int argc, char **argv) {
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
        if(strncmp(command, cmd_clear, strlen(cmd_clear))==0) {
            printf("\e[H\e[J");
            continue;
        }
        // Fork the process and try to run the command
		pid_t pid = fork();
		if (pid == 0) {
            char *args[] = { command, NULL };
			int code = execve(command, args, NULL);
            if ( code == -1 ) {
                printf("\e[0;31mERR(execve \"%s\"): %s\e[0m\n", command, strerror(errno));
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