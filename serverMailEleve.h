#define _XOPEN_SOURCE
// Bibliothèques standard
#include <ctype.h> // toupper()
#include <stdio.h>
#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h> // strlen()



#include <time.h>
#include <errno.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 

//Si nous sommes sous Windows
#if defined (WIN32)

    #include <winsock2.h>

    // typedef, qui nous serviront par la suite
    typedef int socklen_t;

// Sinon, si nous sommes sous Linux
#elif defined (linux)

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

#endif
#define MAX_MSG 2048
// 1 caractère pour le codes ASCII '\0'
#define MSG_ARRAY_SIZE (MAX_MSG + 1)

typedef struct Member Member;
struct Member {
    char adress[128]; //adress mail
    SOCKET sock; //socket
    char *ipAdress; 
};

typedef struct Mail Mail;
struct Mail {
    int id;
    Member sender;
    Member receiver;
    char *message;
    char *object;
    int etat; // 1 si mail recu, 0 si envoyé mais non recu (pas connecté), -1 si receiver non connu. 
    time_t timeMail;
};

/*
Retourne l'indice du mail dans le tableau si il y a un mail en attente pour l'adresse donné en paramètre, sinon retourne -1
*/
int findMailWaiting(SOCKET socket, Mail **listMailWait);

/*
Envoit le mail donné en paramètre au client définit par le socket donné en paramètre. 
*/
void sendMail(Mail *mail, SOCKET socketDescriptor);

/*
Envoie un message à un client définit par le socket en paramètre, vérifie si il n'y a pas d'erreur et renvoie la longueur du message.
*/
int sendMessage(char *message, SOCKET socketClient);

/*
Lit un int reçu par le client donné en paramètre
*/
int readInt(SOCKET socketDescriptor);

/*
Envoie un int à un client définit par le socket donné en paramètre.
*/
int sendInt(int nb, SOCKET socketClient);

/*
Reçoit un message d'un client définit par le socket en paramètre, vérifie si il n'y a pas d'erreur, complète le message par un caractère de fin de chaîne de caractère et renvoie la longueur du message. Le message est stocké dans le paramètre message.
*/
int readMessage(char *message, SOCKET socketClient);

/*
Lit un mail envoyé par un client et créer une variable mail pour stocker toutes les informations. La fonction return le member a qui est destiné le mail, éventuellement NULL si le destinataire n'est pas connu. 
*/
Member *readMail(Mail *mail, SOCKET socketDescriptor, Member **memberList);

/*
Cherche et renvoie un membre dans la liste identifié par son adresse.
*/
Member *findMemberByAdress(char *adress, Member **menberList);

/*
Cherche et renvoie un membre dans la liste identifié par son socket.
*/
Member *findMemberBySocket(SOCKET socket, Member **memberList);

/*
Créer un membre avec le socket, ip et adress donné en paramètre puis l'ajoute à la liste de membre. 
*/
int addMember(SOCKET sock, char * ipaddr, Member **menberList, char *adress);

/*
affiche sur la console la liste de membre.
*/
void printListMember(Member **memberList);

/*
affichage sur la console le mail donné en paramètre
*/
void printMail(Mail *mail);