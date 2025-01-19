mkdir -p bin
gcc -o bin/init system/init.c -static
gcc -o bin/shell system/shell.c -static