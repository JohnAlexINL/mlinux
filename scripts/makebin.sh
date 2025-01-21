gcc -o image/root/bin/init system/init.c -static
gcc -o image/root/bin/shell system/shell.c -static
gcc -o image/root/bin/cat system/cat.c -static
gcc -o image/root/bin/nex src/nex/nex.c -static