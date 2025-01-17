/**
 * @fn int main(void)
 * @brief Fonction principale du programme
 * @return 0 Arrêt normal du programme
 */

int main(void)
{
    SOCKET listenSocket;  // Socket d'écoute en attente des connexions des clients
    SOCKET clientSocket[30]; //la liste de socket utilisé pour la communication
    SOCKET newSocket; //socket utilisé lors de l'ajout d'un nouveau client

    //set of socket descriptors  
    fd_set readfds;  
    int max_sd, sd, activity, max_clients = 30; //variable pour le mutli client.  


    //initialise all client_socket[] to 0 so not checked  
    for (int i = 0; i < max_clients; i++)   
    {   
        clientSocket[i] = 0;   
        listMember[i] = NULL;
    }

    //initialise le socket du serveur.

    
    while (1) {
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(listenSocket, &readfds);   
        max_sd = listenSocket;   
             
        //add child sockets to set  
        for ( int i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = clientSocket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(listenSocket, &readfds))   
        {   
            clientAddressLength = sizeof(clientAddress);
            if ((newSocket = accept(listenSocket,  
                    (SOCKADDR *) &clientAddress,  &clientAddressLength))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , newSocket , inet_ntoa(clientAddress.sin_addr) , ntohs 
                  (clientAddress.sin_port));   
           
            //send new connection greeting message  
            if( send(newSocket, messageSend, strlen(messageSend), 0) != strlen(messageSend) )   
            {   
                perror("send");   
            }   
                 
            printf("SUCCESS: Envoie du message de bienvenue\n");  
                 
            //add new socket to array of sockets  
            for (int i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( clientSocket[i] == 0 )   
                {   
                    clientSocket[i] = newSocket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }

            //on récupère le mail du client qui vient se connecter.
            if ((msgLength = readMessage(messageReceived, newSocket)) != 0) {
                printf(" -- %s(%ld)\n", messageReceived, msgLength);

                if(strchr(messageReceived, '@') != NULL) {
                //on a bien reçu une adresse mail
                    printf("Le serveur a bien reçu l'adresse mail pour effectuer une connexion d'un client. \n");
                    printListMember(listMember);
                    if(findMemberByAdress(messageReceived, listMember) == NULL) {
                        addMemberToFile(newSocket,inet_ntoa(clientAddress.sin_addr),listMember,messageReceived);
                        //add the member to the file also.


                    }
                }
            }
            //envoie au nouveau client connecté le mail qui lui est en attente. 
            int indiceMail = findMailWaiting(newSocket, listMailAttente);
            if(indiceMail != -1) {
                sendInt(1, newSocket);
                sendMail(listMailAttente[indiceMail],newSocket);
                listMailAttente[indiceMail] = NULL;
            } else {
                sendInt(0, newSocket);
            }

        }   
             
        //else its some IO operation on some other socket 
        for (int i = 0; i < max_clients; i++)   
        {   
            sd = clientSocket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
                
                Mail *mail = malloc(sizeof(Mail));
                Member *receiver = readMail(mail, sd, listMember);
                listMail[mail->id] = mail;
                if(receiver == NULL) {
                    for(int i = 0; i < 10; i ++) {
                        if(listMailAttente[i] == NULL) {
                            listMailAttente[i] = mail;
                        } else if(i == 9) {
                            printf("plus de place pour stocker un mail sans destinataire \n");
                        }
                    }
                    
                } else  {
                    //on envoit le mail à la bonne personne en lui précisant qu'elle va recevoir un mail. 
                    sendInt(2, receiver->sock);
                    sendMail(mail, receiver->sock);
                }
            }   
        }   
    }

    //
    // Fermeture de la socket de travail (connexion avec le client)
    for (int i = 0; i < max_clients; i++)   
    {   
        if(clientSocket[i] != 0) {
            closesocket(clientSocket[i]);   
        }
    } 
    printf("\nFermeture de la socket de travail\n");

    //
    // Fermeture de la socket client
    closesocket(listenSocket);
    printf("Fermeture de la socket d'écoute\n");

    printf("\nFermeture du serveur\n\n");
    return EXIT_SUCCESS;
}