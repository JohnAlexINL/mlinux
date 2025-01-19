// #include <stdlib.h>
#include <unistd.h>
int main (int argc, char **argv) {
    char *args[] = { "/bin/shell", NULL };
    execve("/bin/shell", args, NULL);
}