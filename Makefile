CC=gcc
CFLAGS=-Wall
EXEC_SERVER=server.out
EXEC_CLIENT=client.out
LIB_CLIENT=client
LIB_SERVER=server
SRC_CLIENT=$(wildcard *client.c)
SRC_SERVER=$(wildcard *server.c)

OBJ_CLIENT=$(SRC_CLIENT:.c=.o)
OBJ_SERVER=$(SRC_SERVER:.c=.o)

all : $(EXEC_CLIENT) $(EXEC_SERVER)

$(EXEC_CLIENT) : $(OBJ_CLIENT)
	$(CC) $^ -L . -l $(LIB_CLIENT) -o $@ 

$(EXEC_SERVER) : $(OBJ_SERVER)
	$(CC) $^ -L . -l $(LIB_SERVER) -o $@ 

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm *.o

clean_all:
	rm *.o *.out