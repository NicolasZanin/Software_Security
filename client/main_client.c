#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "../serverclient/client_server.h"

// Variable static Client
static int PORT_SERVER = 2000;
static int PORT_CLIENT = 2001;
static int SIZE_MESSAGE = 1024;

/**
 * \brief Fonction pour renvoyer le min entre deux nombres
 * \param a entier à comparer avec b
 * \param b entier à comparer avec a
 */
int min(const int a,const int b) {
    return a < b? a : b;
}

/**
 * \brief Vérifie les options utilisées dans la commande
 * \param argOptionUn le premier argument (hormis celui de la fonction)
 * \return un nombre >= 0 si l'option est reconnu sinon -1
 */
int verifyParametre(const char *argOptionUn) {
    if (strcmp("-up", argOptionUn) == 0)
        return 0;
    if (strcmp("-list", argOptionUn) == 0)
        return 1;
    if (strcmp("-down", argOptionUn) == 0)
        return 2;
    return -1; 
}

/**
 * \brief Initialise le message avec certains arguments
 * \param msg Le message à initialiser
 * \param nbArgs Le nombre d'arguments
 * \param ... Les arguments
 * \return La taille du contenu ajouté dans le message
 */
int initMsg(char msg[SIZE_MESSAGE],const int nbArgs, ...) {
    va_list args;
    int tailleInitMsg = 0;
    va_start(args, nbArgs);
    char *chaine = va_arg(args, char*);
    int i = 1;

    strcpy(msg, chaine);
    tailleInitMsg += strlen(msg);

    while(i++ < nbArgs) {
        chaine = va_arg(args, char*);
        strcat(msg, chaine);
        strcat(msg, ",");
        tailleInitMsg += strlen(chaine) + 1;
    }

    va_end(args);
    return tailleInitMsg;
}

/**
 * \brief Récupère le nom du fichier
 * \param pathFileName L'emplacement où se trouve le fichier
 * \return le nom du fichier
 */
char *getFileName(char *pathFileName) {
    char *fileName = strrchr(pathFileName, '/'); // Récupère la chaine où se trouve la dernière occurence de '/'

    // Si le caractère '/' n'est pas existant, on retourne la chaine d'origine sinon le nom du fichier
    return fileName == NULL? pathFileName: fileName;
}

/**
 * \brief Envoie le fichier au serveur
 * \param pathFile le path vers le fichier
 */
void sendFile(char *pathFile, int deleteFile) {
    // Récupère les informations du fichier et si il existe
    struct stat statFile;
    const int exist = stat(pathFile, &statFile);
    const int sizeFile = statFile.st_size;

    printf("Taille du fichier : %d octets (10Mo Max)\n", sizeFile);

    // Si il existe et que c'est un fichier et de taille < 10 mo
    if (exist == 0 && S_ISREG(statFile.st_mode) && sizeFile <= 1048576) {

        // Créer un tableau de caractère du nombre d'octet du fichier pour lire le fichier en une fois et permet donc d'éviter une modification lors de récupération de caractères étape par étape
        char file[sizeFile];
        const int fd = open(pathFile, O_RDONLY);
        read(fd, file, sizeFile);
        close(fd);

        // Initialise le message
        char msg[SIZE_MESSAGE];
        char *fileName = getFileName(pathFile);
        int lenInitMsg = initMsg(msg, 2, "SENT,", fileName);

        // Ajoute les 1024 caractères - la taille de l'initiations du fichier
        strncat(msg, file, min(sizeFile, SIZE_MESSAGE - lenInitMsg));
        int tailleRestante = sizeFile - (SIZE_MESSAGE - lenInitMsg);
        sendmessage(msg, PORT_SERVER);

        // Boucle tant qu'on a pas envoyé tous le fichier
        while (tailleRestante >= 0) {
            memset(msg, 0, sizeof(msg));

            // Permet d'ajouter du contenu dans le fichier envoyé au serveur
            lenInitMsg = initMsg(msg, 2, "APPEND,", fileName);
            strncat(msg, &file[sizeFile - tailleRestante], min(tailleRestante, 1024 - lenInitMsg));
            tailleRestante -= SIZE_MESSAGE - lenInitMsg;
            sendmessage(msg, PORT_SERVER);
        }

        printf("Envoie terminé\n");

        // Supprime le fichier si l'option -r est utilisé
        if (deleteFile) remove(pathFile);
    }
}

/**
 * \brief Permet de lire les messages du serveur et ecrire soit dans un fichier soit sur la sortie standard
 * \param fd le descripteur de fichier donc soit un fichier, soit la sortie standard
 */
void lireMessageServer(const int fd) {
    char msg[SIZE_MESSAGE];

    while(1) {
        const int tailleMessage = getmessage(msg);

        // Vérifie si la taille du message est de la taille de l'indicateur de fin 'FINI'
        if (tailleMessage == 4) {

            // Regarde si le message est 'FINI'
            if (strcmp("FINI", msg) == 0)
                break;
        }

        write(fd, msg, tailleMessage);
    }
}

/**
 * \brief Affiche les fichiers stocker dans le serveur
 */
void getFilesServer() {
    // Demande au serveur de récupérer les fichiers du serveur
    char msg[1024] = "LIST\0";
    sendmessage(msg, PORT_SERVER);

    // Démarre le serveur pour recevoir les messages côté client
    start_server(PORT_CLIENT);

    // Lis les messages reçu côté serveur et l'affiche sur la sortie standard
    lireMessageServer(fileno(stdout));
    stop_server();
}

void getFileServer(const char *file, int deleteFile) {
    // Ouvre le fichier en lecture
    const int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR| S_IWUSR);

    // Demande au serveur de récupérer le fichier du serveur
    char msg[1024] = "FILE,";
    strcat(msg, file);
    if (deleteFile) strcat(msg, ",-r");
    sendmessage(msg, PORT_SERVER);

    // Démarre le serveur pour recevoir les messages côté client
    start_server(PORT_CLIENT);
    lireMessageServer(fd);
    stop_server();
    close(fd);
}

/**
 * \brief Permet d'afficher les informations d'utilisation de l'application
 */
void ecrireInfo() {
    printf("La commandes a utiliser pour le client est : \n");
    printf("sectrans <option> [parametre]\n\n");
    printf("Ses options et parametres sont :\n");
    printf("-up file , ou file est le chemin du fichier\n");
    printf("-list , affiche les fichiers du serveur\n");
    printf("-down file, pour recuperer le fichier du serveur\n");
    printf("-r, pour supprimer le fichier du client apres l'avoir envoyer ou recuperer\n");
}


int main(int argc, char *argv[]) {
    if (argc > 1) {
        // Récupère la fonctionnalité et renvoie l'indice de la fonctionnalité correspondante, pour rediriger sur une certaine fonction
        const int numero_parametre = verifyParametre(argv[1]);

        if (numero_parametre == 0 && argc > 2) { // Si il faut envoyer un fichier au serveur
            sendFile(argv[2], argc > 3 && strcmp(argv[3], "-r") == 0);
        }
        else if (numero_parametre != -1) {
            if (numero_parametre == 1) // Récupérer les fichiers du serveur
                getFilesServer();
            if (numero_parametre == 2) // Récupère le fichier du serveur
                getFileServer(argv[2], argc > 3 && strcmp(argv[3], "-r") == 0);
        }
        return 0;
    }

    fprintf(stderr, "Erreur parametre option\n");
    ecrireInfo();

    return 1;
}
