Instruction for compilation :   make

Instruction to remove .o files :  make clean

Examples for execution : 
./mirror_client -n 1 -c ./common -i ./1_input -m 1_mirror -b 100 -l logfile1

./mirror_client -n 2 -c ./common -i ./2_input -m 2_mirror -b 100 -l logfile2

./mirror_client -n 3 -c ./common -i ./3_input -m 3_mirror -b 100 -l logfile3


Series of arguments is irrelevant as long as flags are inserted correctly!


The project files are organised as:
-> XInterface.h
-> XImplementation.c

where X is the corresponding types are being implemented, for example inside FileInterface.h are the prototypes of functions that are used to handle the files and directories.

Inside of a XInterface.h file are the function prototypes and inside a XImplementation.c file are the implementations of the referred functions.
I added a main() function that checks if command line arguments are added correctly (for example if input_directory of client doesn't exist, the application terminates) ,
creates directories common and X_mirror if they do not exist, creates a logfileX where it writes several messages. I have also added a Makefile and a test.sh to creat small 
input files.

Every client uses a dynamic array called id_array where it keeps the ids of new-coming clients in the system.

Checkdir() is a funtion that traverses the common directory in order to find new ids and returns the last one added. The function updateArray() is used to reallocate the size of 
the id_array when a change occurs for example a client leaves.


Only function found on internet was remove_directory() . Source: https://stackoverflow.com/questions/2256945/removing-a-non-empty-directory-programmatically-in-c-or-c/42978529

A client gets removed from the system by a signal CTRL+C from the terminal. Files in common and mirror are preserved after exit.