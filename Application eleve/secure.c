#include "serverMailEleve.h"
#include <string.h>

/**
 * @fn int main(void)
 * @brief Fonction principale du programme serveur.
 * @return 0 Arrêt normal du programme.
 */
#define PORT 7676

int findMailWaiting(SOCKET socket, Mail **listMailWait) {
    for (int i = 0; listMailWait[i] != NULL; i++) {
        if (listMailWait[i]->receiver.sock == socket && listMailWait[i]->etat == 0) {
            return i;
        }
    }
    return -1;
}

void sendMail(Mail *mail, SOCKET socketDescriptor) {
    char buffer[MSG_ARRAY_SIZE];
    snprintf(buffer, sizeof(buffer), "Mail de %s à %s : %s\nObjet : %s\nMessage : %s",
             mail->sender.adress, mail->receiver.adress, ctime(&(mail->timeMail)),
             mail->object, mail->message);
    sendMessage(buffer, socketDescriptor);
}

int sendMessage(char *message, SOCKET socketClient) {
    int messageLength = strlen(message);
    int bytesSent = send(socketClient, message, messageLength, 0);
    if (bytesSent < 0) {
        perror("send failed");
        return SOCKET_ERROR;
    }
    return bytesSent;
}

int readInt(SOCKET socketDescriptor) {
    int nb;
    int bytesRead = recv(socketDescriptor, &nb, sizeof(int), 0);
    if (bytesRead < 0) {
        perror("recv failed");
        return SOCKET_ERROR;
    }
    return nb;
}

int sendInt(int nb, SOCKET socketClient) {
    int bytesSent = send(socketClient, &nb, sizeof(int), 0);
    if (bytesSent < 0) {
        perror("sendInt failed");
        return SOCKET_ERROR;
    }
    return bytesSent;
}

int readMessage(char *message, SOCKET socketClient) {
    int bytesRead = recv(socketClient, message, MSG_ARRAY_SIZE - 1, 0);
    if (bytesRead < 0) {
        perror("recv failed");
        return SOCKET_ERROR;
    }
    message[bytesRead] = '\0';
    return bytesRead;
}

Member *readMail(Mail *mail, SOCKET socketDescriptor, Member **memberList) {
    char buffer[MSG_ARRAY_SIZE];
    Member *receiver = NULL;

    int bytesRead = readMessage(buffer, socketDescriptor);
    if (bytesRead < 0) {
        return NULL;
    }
    receiver = findMemberByAdress(buffer, memberList);
    if (receiver == NULL) {
        return NULL;
    }

    bytesRead = readMessage(buffer, socketDescriptor);
    strncpy(mail->object, buffer, MSG_ARRAY_SIZE - 1);
    mail->object[MSG_ARRAY_SIZE - 1] = '\0';

    bytesRead = readMessage(buffer, socketDescriptor);
    strncpy(mail->message, buffer, MSG_ARRAY_SIZE - 1);
    mail->message[MSG_ARRAY_SIZE - 1] = '\0';

    mail->sender.sock = socketDescriptor;
    mail->receiver = *receiver;
    mail->timeMail = time(NULL); 
    mail->etat = 0; 

    return receiver;
}

Member *findMemberByAdress(char *adress, Member **memberList) {
    for (int i = 0; memberList[i] != NULL; i++) {
        if (strcmp(memberList[i]->adress, adress) == 0) {
            return memberList[i];
        }
    }
    return NULL;
}

Member *findMemberBySocket(SOCKET socket, Member **memberList) {
    for (int i = 0; memberList[i] != NULL; i++) {
        if (memberList[i]->sock == socket) {
            return memberList[i];   
        }
    }
    return NULL; 
}

int addMember(SOCKET sock, char *ipaddr, Member **memberList, char *adress) {
    Member *newMember = (Member *)malloc(sizeof(Member));
    if (newMember == NULL) {
        perror("malloc failed");
        return -1;
    }

    newMember->sock = sock;
    newMember->ipAdress = ipaddr; 
    strncpy(newMember->adress, adress, sizeof(newMember->adress) - 1);
    newMember->adress[sizeof(newMember->adress) - 1] = '\0'; 

    int i = 0;
    while (memberList[i] != NULL) {
        i++;
    }
    memberList[i] = newMember;
    memberList[i + 1] = NULL;   

    return 0;
}

void printListMember(Member **memberList) {
    printf("Liste des membres connectés :\n");
    for (int i = 0; memberList[i] != NULL; i++) {
        printf("Membre %d: %s, IP: %s, Socket: %d\n", i + 1,
               memberList[i]->adress, memberList[i]->ipAdress, memberList[i]->sock);
    }
}

void printMail(Mail *mail) {
    printf("Mail ID: %d\n", mail->id);
    printf("De: %s\n", mail->sender.adress);
    printf("A: %s\n", mail->receiver.adress);
    printf("Objet: %s\n", mail->object);
    printf("Message: %s\n", mail->message);
    printf("Date: %s", ctime(&(mail->timeMail)));
}


int main(void)
{
    Mail newMail;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("socket failed");
        return EXIT_FAILURE;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        closesocket(sock);
        return EXIT_FAILURE;
    }

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);  
    addr.sin_addr.s_addr = INADDR_ANY;  

    if (bind(sock, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        perror("bind failed");
        closesocket(sock);
        return EXIT_FAILURE;
    }

    if (listen(sock, 10) == SOCKET_ERROR) {
        perror("listen failed");
        closesocket(sock);
        return EXIT_FAILURE;
    }

    printf("Serveur en attente de connexions sur le port 8080...\n");

    Member *memberList = NULL;

    while (1) {
        SOCKET clientSock;
        SOCKADDR_IN clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        clientSock = accept(sock, (SOCKADDR *)&clientAddr, &clientAddrLen);
        if (clientSock == INVALID_SOCKET) {
            perror("accept failed");
            continue; 
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
        printf("Client connecté depuis %s\n", clientIP);

        char buffer[1000];
        int bytesRead = readMessage(buffer, clientSock);
        if (bytesRead > 0) {
            printf("Adresse mail du client reçue : %s\n", buffer);

            addMember(clientSock, clientIP, &memberList, buffer);
            
        }
        char buffer2[1000];

        bytesRead = readMessage(buffer2, clientSock);
        if (bytesRead > 0) {
            Member *receiver = readMail(&newMail, clientSock, &memberList);
            printf("Message : %s\n", buffer2);
            if (receiver != NULL) {
                sendMail(&newMail, receiver->sock);
                printf("Adresse mail du destinataire %s\n", receiver->adress);
            } else {
                char errorMsg[] = "Destinataire inconnu. Mail non envoyé.";
                sendMessage(errorMsg, clientSock);
            }
        }

        closesocket(clientSock);
    }

    closesocket(sock);

    return EXIT_SUCCESS;
}
