#include "clientMailEleve.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define FILE_NAME "data.xml"

/**
 * @fn int main(void)
 * @brief Fonction principale du programme
 * @return 0 Arrêt normal du programme
 */
int main(void)
{
    int sockfd;
    SOCKADDR_IN addr_in;
    FILE *file;
    char buffer[1024]; // Déclaration du buffer
    size_t bytes_read; // Déclaration de la variable de lecture

    // Configuration de l'adresse du serveur
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(7676);
    addr_in.sin_addr.s_addr = inet_addr("172.16.4.248");
printf("CONNECTED\n");
    // Création du socket 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Connexion au serveur
    connect(sockfd, (SOCKADDR *)&addr_in, sizeof(addr_in));

    // Ouverture et envoi du fichier "data.xml"
    file = fopen(FILE_NAME, "rb");
    if (file != NULL) { // Vérifie si le fichier s'ouvre correctement
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            send(sockfd, buffer, bytes_read, 0);
        }
        fclose(file);
    }
        printf("fichier envoyer avec succès\n");
    // Fermeture du socket
    close(sockfd);
    return 0;
}