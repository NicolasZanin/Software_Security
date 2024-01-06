# Projet Software Security  

Il y a actuellement 6 fichiers :
- main_client.c
- client.h
- client.c
- main_server.c
- server.h
- server.c

## Execution normal :
Pour compiler le programme: 
```sh
make
```

Pour exécuter le serveur : 
```sh
./main_server.sh
```

Pour exécuter le client :
```sh
./sectrans.sh
```

## Les commandes classique pour compiler :

Pour compiler entièrement, il faut compiler les fichiers communs :
```sh
gcc -Wall -c ./serverclient/client_server.c -o ./serverclient/client_server.o
```

Pour compiler le serveur :
```sh
gcc -Wall -c ./server/main_server.c -o ./server/main_server.o
gcc ./server/main_server.o ./serverclient/client_server.o -L./serverclient -lclient -lserver -o ./server/main_server
```

Pour compiler le client :
```sh
gcc -Wall -c ./client/sectrans.c -o ./client/sectrans.o
gcc ./client/sectrans.o ./serverclient/client_server.o -L./serverclient -lclient -lserver -o ./client/sectrans
```
