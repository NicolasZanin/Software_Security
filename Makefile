CC = gcc
CFLAGS = -Wall
DIR_UTILS = serverclient
DIR_CLIENT = client
DIR_SERVEUR= server
SRC_UTILS = $(wildcard $(DIR_UTILS)/*.c)
SRC_CLIENT = $(DIR_CLIENT)/main_client.c
SRC_SERVEUR = $(DIR_SERVEUR)/main_server.c
OBJ_UTILS = $(SRC_UTILS:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)
OBJ_SERVEUR = $(SRC_SERVEUR:.c=.o)
EXEC_CLIENT = $(DIR_CLIENT)/sectrans
EXEC_SERVEUR = $(DIR_SERVEUR)/main_server

all: sectrans main_serveur

sectrans: $(OBJ_CLIENT) $(OBJ_UTILS)
	$(CC) $^ -o $(EXEC_CLIENT)

main_serveur: $(OBJ_SERVEUR) $(OBJ_UTILS)
	$(CC) $^ -o $(EXEC_SERVEUR)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(DIR_CLIENT)/*.o $(DIR_SERVEUR)/*.o $(DIR_UTILS)/*.o

mrproper: clean
	rm $(EXEC_CLIENT) $(EXEC_SERVEUR)