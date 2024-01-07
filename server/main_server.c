#include "../serverclient/client_server.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

// Variable static Serveur
static int PORT_SERVER = 2000;
static char* DIRECTORY_SERVER = "../fileserver/";
static int TAILLE_PATH_DIRECTORY_SERVER = 14;
static int SIZE_MESSAGE = 1024;
static int PORT_CLIENT = 2001;

/**
 * \brief Fonction pour renvoyer le min entre deux nombres
 * \param a entier à comparer avec b
 * \param b entier à comparer avec a
 */
int min(const int a,const int b) {
    return a < b? a : b;
}

/**
 * \brief Permet de capturer le signal SIGINT pour une fermeture propre
 * \param sig le numéro du signal
 */
void handle_sigint(int sig) {
    printf("Close server\n");
    stop_server();
    exit(0);
}

/**
 * \brief Permet de vérifier ce que demande le client
 * \param argOptionUn Le premier argument du message (<FONCTIONNALITE>,[param et/ou contenu])
 * \return Un nombre > 0 si c'est une fonctionnalité reconnu sinon -1
 */
int verifyParametre(const char *argOptionUn) {
    if (strcmp("SENT", argOptionUn) == 0)
        return 0;
    if (strcmp("APPEND", argOptionUn) == 0)
        return 1;
    if (strcmp("LIST", argOptionUn) == 0)
        return 2;
    if (strcmp("FILE", argOptionUn) == 0)
        return 3;
    return -1;
}

/**
 * \brief Fonction pour ajouter un fichier au serveur, elle est utilisé quand le client utilise la commande sectrans -up file
 * \param msg Le message envoyé par le client
 * \param isNewFile Si il faut créer un nouveau fichier (SENT pour créer un nouveau Fichier, APPEND pour ajouter dans un fichier)
 * \param sizeMessage La taille du message envoyé par le client
 */
void writeFile(char msg[SIZE_MESSAGE], const int isNewFile, const int sizeMessage) {
    // Récupère le nom du fichier après la fonctionnalité (msg : SENT, nameFile, contenuFichier)
    const char *nameFile = strtok(NULL, ",");

    // Verifie si le client n'a pas envoyé uniquement (msg : SENT)
    if (nameFile != NULL) {

        // Donne le path vers le dossier où sont stocker les fichiers envoyé au serveur
        const int sizeNameFile = strlen(nameFile);
        char pathFile[sizeNameFile + TAILLE_PATH_DIRECTORY_SERVER];
        strcpy(pathFile, DIRECTORY_SERVER);
        strcat(pathFile, nameFile);

        // Ouvre le fichier en écriture et ajoute les droits de lecture et écriture pour l'utilisateur
        const int fdIn = isNewFile? open(pathFile, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR) : open(pathFile, O_WRONLY | O_APPEND | O_CREAT);

        // Variable pour stocker l'indice du début du contenu de message, 6 car SENT fait 4 caractère et les 2 ',' ou 8 pour APPEND qui fait 6 caractères et les 2 ','
        const int indexStartMsg = isNewFile? 6 + sizeNameFile : 8 + sizeNameFile;

        // Ecris le contenu du message dans le fichier
        write(fdIn, &msg[indexStartMsg], sizeMessage - indexStartMsg);
        close(fdIn);
    }
}

/**
 * \brief Récupère les fichiers du serveur, fonction utilisé quand le client utilise la commande sectrans -list
 */
void getFilesServer() {
    DIR *dir = opendir(DIRECTORY_SERVER); // Ouvre le dossier stocker dans le serveur
    struct dirent *d;
    char msg[1024] = "";
    int sizeMsg = 0;

    while((d = readdir(dir))) {
        const int sizeNameFile = strlen(d->d_name);

        if (d->d_name[0] == '.' && (sizeNameFile == 1  || (sizeNameFile == 2 && d->d_name[1] == '.')))
            continue;

        // Si le nom du Fichier + 1 (avec le '\n') est supérieur à la taille du message qu'on peut envoyer
        if (sizeMsg + sizeNameFile + 1 >= SIZE_MESSAGE) {
            sendmessage(msg, PORT_CLIENT);
            memset(msg, 0, sizeof(msg)); // Réinitialise à 0 le contenu du message
            sizeMsg = 0;
        }

        // Ajoute le nom du fichier + '\n' dans message
        sizeMsg += sizeNameFile + 1;
        strcat(msg, d->d_name);
        strcat(msg, "\n");
    }

    // Si le message est vide, on notifie le client qu'il n'y a pas de fichier
    if (sizeMsg == 0) {
        strcpy(msg, "No file found\n");
        sizeMsg = 7;
    }

    // Envoie le contenu restant de message et envoie un indicateur de Fin soit FINI
    sendmessage(msg, PORT_CLIENT);
    memset(msg, 0, sizeof(msg));
    strcpy(msg, "FINI");
    sendmessage(msg, PORT_CLIENT);
    closedir(dir);
}

/**
 * \brief Récupère le fichier côté serveur, fonction utilisé quand le client utilise la commande sectrans -down file
 * \param fileName le Nom du fichier à récupérer
 */
void getFileServer(const char *fileName, int deleteFile) {
    // Verifie si le client n'a pas envoyé uniquement (msg : FILE)
    if (fileName != NULL) {

        // Donne le path vers le dossier où sont stocker les fichiers envoyé au serveur
        const int lenNameFile = strlen(fileName);
        char pathFile[lenNameFile + TAILLE_PATH_DIRECTORY_SERVER];
        strcpy(pathFile, DIRECTORY_SERVER);
        strcat(pathFile, fileName);

        // Ouvre le fichier en lecture
        const int fd = open(pathFile, O_RDONLY);

        //  Récupère les infos du fichier
        struct stat s;
        stat(pathFile, &s);

        // Créer un tableau de caractère du nombre d'octet du fichier pour lire le fichier en une fois et permet donc d'éviter une modification lors de récupération de caractères étape par étape
        char file[s.st_size];
        int tailleRestante = s.st_size;
        read(fd, file, s.st_size);
        close(fd);

        // Copie 1024 caractères à chaque boucle jusqu'à qu'il n'y est plus de caractères
        while(tailleRestante > 0) {
            char msg[SIZE_MESSAGE];
            strncpy(msg, &file[(int) s.st_size - tailleRestante], min(tailleRestante, SIZE_MESSAGE));
            tailleRestante -= SIZE_MESSAGE;
            sendmessage(msg, PORT_CLIENT);
        }
        
        // Supprime le fichier si l'option -r est utilisé
        if (deleteFile) remove(pathFile);
    }

    // Envoie au client un indicateur de fin
    char msg[1024] = "FINI";
    sendmessage(msg, PORT_CLIENT);
}

int main(void) {
    const int portServer = start_server(PORT_SERVER);
    printf("Port : %d\n", portServer);

    // Verifie si le dossier de stockage existe, sinon le créer
    DIR *dir = opendir(DIRECTORY_SERVER);
    if (dir == NULL) {
        mkdir(DIRECTORY_SERVER, 0777);
        dir = opendir(DIRECTORY_SERVER);
    }

    // Change le signal SIGINT
    signal(SIGINT, handle_sigint);

    // Boucle à l'infini pour que le serveur récupère toujours le message
    while(1) {
        char message[SIZE_MESSAGE];
        const int sizeMessageClient = getmessage(message);

        // Récupère la fonctionnalité et renvoie l'indice de la fonctionnalité correspondante, pour rediriger sur une certaine fonction
        const int parametre = verifyParametre(strtok(message, ","));
        switch (parametre) {
            case -1: break; // Si la fonctionnalité n'est pas existante
            case 0: writeFile(message, 1, sizeMessageClient); break; // Si on ajoute un fichier
            case 1: writeFile(message, 0, sizeMessageClient); break; // Si on ajoute du contenu à un fichier
            case 2: load_library_client("libclient.so"); // Charge et décharge la librairie client et récupère les fichier serveur
                    getFilesServer();
                    unload_library_client();
                    break;

            case 3: load_library_client("libclient.so"); // Charge et décharge la librairie client et récupère un fichier serveur
                    char *fileName = strtok(NULL, ",");
                    char *deleteFile = strtok(NULL, ",");
                    getFileServer(fileName, deleteFile != NULL && strcmp(deleteFile, "-r") == 0);
                    unload_library_client();
                    break;
            default: break;
        }
    }
    return 0;
}