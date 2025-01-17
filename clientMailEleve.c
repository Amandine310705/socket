#include "clientMailEleve.h"

/**
 * @fn int main(void)
 * @brief Fonction principale du programme
 * @return 0 Arrêt normal du programme
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

int main(void)
{
    Mail mail;
    int client_fd, status, valread;
    char buffer[1024] = { 0 };

    struct sockaddr_in serv_addr;

    // Créer un socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket creation failed ");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("172.16.107.4");

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) == -1)
    {
        perror("connection failed ");
        return -1;
    }

    do
    {
        printf("Entrez votre email pour vous connecter au serveur (au format xxx@bts-ciel.fr) :\n");
        scanf("%s", buffer);
    } while (strstr(buffer, "@bts-ciel.fr") == NULL);

    strcpy(mail.sender, buffer);

    if(send(client_fd, buffer, strlen(buffer), 0) == -1)
    {
        perror("mail sending failed ");
        return -1;
    }// else { printf("email sent\n"); }
    buffer[0] = '\0';

    createMail(&mail);
    printMail(&mail);

    // Fermer le socket connecté
    close(client_fd);

    return EXIT_SUCCESS;
}

// Crée un mail remplie par l'utilisateur
void createMail(Mail *mail)
{
    char buffer[1024] = { 0 };

    mail->sender = (char *)malloc(100 * sizeof(char));
    mail->receiver = (char *)malloc(100 * sizeof(char));
    mail->message = (char *)malloc(1024 * sizeof(char));
    mail->object = (char *)malloc(256 * sizeof(char));

    mail->timeMail = time(NULL);
    do
    {
        printf("A qui voulez vous envoyer votre mail ?\n");
        scanf("%s", buffer);
    } while (strstr(buffer, "@bts-ciel.fr") == NULL);

    strcpy(mail->receiver, buffer);

    printf("Entrez l'objet de votre mail :\n");
    scanf("%s", buffer);

    strcpy(mail->object, buffer);
    
    while(getchar() != '\n');

    printf("Entrez votre message :\n");
    scanf("%s", buffer);

    strcpy(mail->message, buffer);

    free(mail->sender);
    free(mail->receiver);
    free(mail->message);
    free(mail->object);

    return;
}

// Affichage sur la console le mail donné en paramètre
void printMail(Mail *mail)
{
    printf("Destinataire : %s\n", mail->receiver);
    printf("Objet : %s\n", mail->object);
    printf("Message : %s\n", mail->message);

    return;
}