#
# In order to execute this "Makefile" just type "make"
#

OBJS 	= main.o TransferImplementation.o FileImplementation.o PipeImplementation.o CheckImplementation.o 
SOURCE	= main.c TransferImplementation.c FileImplementation.c PipeImplementation.c CheckImplementation.c 
HEADER  = TransferInterface.h FileInterface.h PipeInterface.h CheckInterface.h log.h
OUT  	= mirror_client
CC	= gcc
FLAGS   = -g -c
# -g option enables debugging mode 
# -c flag generates object code for separate files

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS) main.c

FileImplementation.o: FileImplementation.c
	$(CC) $(FLAGS) FileImplementation.c

PipeImplementation.o: PipeImplementation.c
	$(CC) $(FLAGS) PipeImplementation.c

CheckImplementation.o: CheckImplementation.c
	$(CC) $(FLAGS) CheckImplementation.c

TransferImplementation.o: TransferImplementation.c
	$(CC) $(FLAGS) TransferImplementation.c


# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
