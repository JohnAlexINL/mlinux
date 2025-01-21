#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // syscalls
#include <sys/mount.h> // filesystem mounting
#include <sys/wait.h> // fork
#include <sys/stat.h> // mkdir
#include <errno.h> // EEXIST and other error numbers

const char *proc_target = "/proc";
const char *proc_filesystemtype = "proc";

int main (int argc, char **argv) {
    if (mkdir("/proc", 0755) != 0 && errno != EEXIST) {
        printf("\e[0;31mFailed to mkdir /proc\e[0m\n");
    }
    if (mount("proc", proc_target, proc_filesystemtype, 0, NULL) != 0) {
        printf("\e[0;31mFailed to mount /proc\e[0m\n");
    }

    char *args[] = { "/bin/shell", NULL };
    char *env[] = {
        "USER=root", 
        "HOME=/", 
        "PWD=/",
        "SHELL=/bin/shell",
        "PATH=/bin",
        NULL
    };
    while (1) {
        pid_t pid = fork();
	    if (pid == 0) {
            if ( execve("/bin/shell", args, env) == -1 ) {
                printf("\e[0;31mInit failed, unable to launch shell\e[0m\n");
                while(1) {}
            }
	    } else if (pid < 0 ) {
            siginfo_t info;
			waitid(P_ALL, 0, &info, WEXITED);
        } else {
     	    siginfo_t info;
	        waitid(P_ALL, 0, &info, WEXITED);
            // if we ever get to this point, that means the main shell was killed
            // so we should display an error and spawn a new one
            printf("\e[0;31mShell terminated, init spawning new shell\e[0m\n");
        }
    }
}