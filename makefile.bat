gcc -c interface.c -o interface.o
gcc -c book_management.c -o book_management.o
gcc -c loan.c -o loan.o
gcc -c user.c -o user.o
gcc -c main.c -o main.o
gcc main.o interface.o book_management.o loan.o user.o -o library.exe
del *.o
library.exe