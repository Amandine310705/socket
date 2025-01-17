#define _XOPEN_SOURCE
// Bibliothèques standard
#include <stdio.h>
#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h> // strlen()
#include <time.h>
#include <errno.h>

//Si nous sommes sous Windows
/*#if defined (WIN32)

    #include <winsock2.h>

    // typedef, qui nous serviront par la suite
    typedef int socklen_t;

// Sinon, si nous sommes sous Linux
#elif defined (linux)*/
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>

    // Define, qui nous serviront par la suite
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket(s) close (s)

    // De même
    typedef int SOCKET;
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef struct sockaddr SOCKADDR;
//#endif


#define MAX_MSG 2048
// 1 caractère pour le codes ASCII '\0'
#define MSG_ARRAY_SIZE (MAX_MSG + 1)

typedef struct Mail Mail;
struct Mail {
    int id; //remplie par le serveur
    char *sender;
    char *receiver;
    char *message;
    char *object;
    int etat; // 1 si mail recu, 0 si envoyé mais non recu (pas connecté), -1 si receiver non connu. 
    time_t timeMail;
    Mail *next;
    Mail *previous;
};

// Affichage sur la console le mail donné en paramètre
void printMail(Mail *mail);

// Sauvegarde un mail dans un fichier situé au même endroit que notre programme, le fichier contiendra l'ensemble des mails reçu et envoyé par notre client.
void saveMail(Mail *mail);

// Permet de lire tous les mails sauvegarder et de les mettre dans notre liste de mail.
void readAllSaveMail(Mail** listeMail);

// Crée un mail remplie par l'utilisateur. 
void createMail(Mail *mail);

// Envoit le mail donné en paramètre au serveur définit par le socket donné en paramètre.
void sendMail(Mail *mail, SOCKET socketDestination);

// Envoie un message au serveur définit par le socket en paramètre, vérifie si il n'y a pas d'erreur et renvoie la longueur du message.
int sendMessage(char *message, SOCKET socketDescriptor);

// Lit un int reçu par le servueur donné en paramètre
int readInt(SOCKET socketDescriptor);

// Reçoit un message du serveur définit par le socket en paramètre, vérifie si il n'y a pas d'erreur, complète le message par un caractère de fin de chaîne de caractère et renvoie la longueur du message. Le message est stocké dans le paramètre message.
int readMessage(char *message, SOCKET socketDescriptor);

// Lit un mail envoyé par le serveur et créer une variable mail pour stocker toutes les informations.
void readMail(Mail *mail, SOCKET socketDescriptor);

