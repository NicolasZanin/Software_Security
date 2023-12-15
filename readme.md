# Projet Software Security  

Il y a actuellement 6 fichiers :
- main_client.c
- client.h
- client.c
- main_server.c
- server.h
- server.c

Pour exécuter le programme, il faut faire :  

```sh
make
./exec_client.sh // Pour le client
./exec_server.sh // Pour le server
```

Les commandes classique pour compiler
```sh
// Pour le server
gcc -Wall -c serveur.c -o server.o
gcc server.out -L . -l server -o server.o

// Pour le client
gcc -Wall -c client.c -o client.o
gcc client.out -L . -l client -o client.o
```
