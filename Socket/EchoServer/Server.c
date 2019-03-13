#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <sys/time.h> /* FD_SET*/
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>

#define BUFFER 1024
#define MAXPENDING 5 /* Maximum outstanding connection requests */

void DieWithError(char *errorMessage); /* Error handling function */
void HandleTCPClient(int clntSocket); /* TCP client handling function */

// char *concat(const char *s1, const char *s2){
//     char *result = malloc(strlen(s1) + strlen(s2)+1);
//     strcpy(result,s1);
//     strcat(result,s2);
//     return result;
// }

int main(int argc, char *argv[])
{
    int servSock; /* Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    int newSock;
    int chatsd1,chatsd2;
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort; /* Server port */
    unsigned int clntLen; /* Length of client address data structure */
    int servAddrLen;
    int max_sd, max_clients = 30, clientSockets[30], socketDescriptor, socketActivity, rVal, rVal2, err;
    fd_set readfds;
    char *target =malloc(BUFFER);
    char *usermail =malloc(BUFFER);
    char *message =malloc(BUFFER);
    char *chat =malloc(BUFFER);
    char *buffer =malloc(BUFFER);
    char *tempString =malloc(BUFFER);
    char *option = (char*)malloc(2*sizeof(char));
    int num_users = 2;
    char uval[25], pval[25];
    char *input = (char*)malloc(BUFFER);

    struct Account{
    char username[25];
    char password[25];
    int fdid;
    };

    struct Account alice = {"Alice", "12345"};
    struct Account bob = {"Bob", "56789"};
    struct Account accounts[2] = {alice, bob};

    /*Zero out client sockets*/
    for(int i =0; i < max_clients; i++){
        clientSockets[i] = 0;
    }

    printf("Port number: ");
    fgets(input, BUFFER, stdin);
    echoServPort = strtol(input, NULL, 10);

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError( "socket () failed") ;

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort); /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError ( "bind () failed");
    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed") ;

printf("Server Started!\n");
printf("Listening on port: %d\n",echoServPort);
servAddrLen = sizeof(echoServAddr);

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);
        
        /*Clear socket set*/
        FD_ZERO(&readfds);

        /*Add server socket to set*/
        FD_SET(servSock, &readfds);
        max_sd = servSock;
        // printf("maxsd:%d\n",max_sd);

        /*Add other sockets to set*/
        for(int i = 0; i < max_clients; i++){
            socketDescriptor = clientSockets[i];
            // printf("SD:%d\n",socketDescriptor);

            if(socketDescriptor > 0){
                FD_SET(socketDescriptor,&readfds);
            }
            
            if(max_sd<socketDescriptor){
                max_sd = socketDescriptor;
            }
        }

        socketActivity = select(max_sd+1,&readfds,NULL,NULL,NULL);
        if(socketActivity == 0){
            printf("Select timeout\n");
        }
        else if (socketActivity < 0 && err != EINTR){
            printf("Select() error\n");
        }
        printf("socketActivity:%d\n",socketActivity);

        /*If something happens to main socket*/
        if(FD_ISSET(servSock, &readfds)){
        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
             DieWithError("accept() failed");

             /* clntSock is connected to a client! */
             printf("Handling client %s, Socket fd is %d, Port:%d\n", inet_ntoa(echoClntAddr.sin_addr), clntSock, ntohs(echoClntAddr.sin_port));


            for (int i=0; i < max_clients; i++){
                if(clientSockets[i]==0){
                    clientSockets[i]=clntSock;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }

             /*Login*/
            
            if(recv(clntSock, buffer, 1024, 0)>0){
                buffer[strcspn(buffer,"\n")] = 0; /*Remove leftover newline from fgets*/
                strcpy(uval,buffer); /*Copy buffer value to uval*/
                // printf("uval : %s\n",uval);
            }

            if(recv(clntSock, buffer, 1024, 0)>0){
                buffer[strcspn(buffer,"\n")] = 0; /*Remove leftover newline from fgets*/
                strcpy(pval,buffer);
                // printf("pval : %s\n",pval);
            }

            // int userFound = 0;
            for(int i=0;i<num_users;i++){
                // printf("uval:%s pass:%s\n", uval,pval);
                // printf("User comp: %d, Pass comp %d\n",strcmp(accounts[i].username,uval),strcmp(accounts[i].password,pval));

                if((strcmp(accounts[i].username,uval) == 0) && (strcmp(accounts[i].password,pval) == 0)){ /*Compare username and password*/
                    printf("success!\n");
                    accounts[i].fdid = clntSock; /*Assign socket if succesful*/
                    // userFound = 1;
                    break;
                }
            }
            // if (userFound == 1){
            //     userFound = 0;
            // }
            // else{
            //     close(clntSock);
            // }
        }
        memset(buffer,0,strlen(buffer));
            for (int i=0; i < max_clients; i++){
                socketDescriptor= clientSockets[i];
                /*Actions here*/
                if(FD_ISSET(socketDescriptor, &readfds)){
                        // (rVal = read(socketDescriptor, buffer, 1024)
                    if((rVal = recv(socketDescriptor, buffer, 1024, 0))<=0){
                        // getpeername(socketDescriptor, (struct sockaddr*)&echoServAddr, &clntLen);
                        // printf("Host disconnected, ip %s, port %d\n",inet_ntoa(echoServAddr.sin_addr),ntohs(echoServAddr.sin_port));
                        getpeername(socketDescriptor, (struct sockaddr*)&echoClntAddr, &clntLen);
                        printf("Host disconnected, ip %s, port %d\n",inet_ntoa(echoClntAddr.sin_addr),ntohs(echoClntAddr.sin_port));
                        close(socketDescriptor);
                        clientSockets[i] = 0;
                    }
                    else{
                        buffer[strcspn(buffer,"\n")] = 0;
                        printf("Current sd:%d\n",socketDescriptor);
                        printf("Buffer Result: %s\n", buffer);
                        option = buffer;
                        if (strcmp(option,"1")==0){
                            /*List users*/
                            memset(buffer,0,strlen(buffer));
                            sprintf(buffer,"%d",num_users);
                            send(socketDescriptor, buffer, 1024,0); /*Send the number of users*/

                            for(int j = 0; j < num_users; j++){
                                strcpy(tempString,accounts[j].username); /*Make a copy to not erase data with memset*/
                                strcat(tempString,"\n"); /*Add newline to the copy*/
                                send(socketDescriptor, tempString, strlen(tempString) , 0); /*Send users*/
                            }
                            // printf("Accounts: %s, %s\n", accounts[0].username,accounts[1].username);
                            memset(tempString,0,strlen(tempString));
                        }
                        else if (strcmp(option,"2")==0){
                            /*Received by username*/
                            if(recv(socketDescriptor, target, 1024, 0)>0){
                                message[strcspn(target,"\n")] = 0;
                                printf("Target User is : %s\n",target);
                            }
                            else{
                                printf("Error in targeting user.");
                                exit(-1);
                            }

                            /*Use username to find which socket the messagee's in*/
                            for(int k = 0; k < num_users; k++){
                                if(strcmp(target,accounts[k].username) == 0){
                                    printf("Target Matched!\n");
                                    // target = accounts[k].username;
                                    strcpy(target,accounts[k].username);
                                    break;
                                } 
                                else{
                                    printf("No Match\n");
                                }

                            }

                            if(rVal2 = recv(socketDescriptor, message, 1024, 0)>0){
                                message[strcspn(message,"\n")] = 0;
                                printf("Message is : %s\n",message);

                            }
                            else{
                                printf("Error in message.");
                                exit(-1);
                            }
                        }
                        else if (strcmp(option,"3")==0){
                            /*Acquire Target FDID*/
                            int targetfdid;
                            for(int k = 0; k < num_users; k++){
                                if(strcmp(target,accounts[k].username) == 0){
                                    // printf("Target Matched2!\n");
                                    targetfdid = accounts[k].fdid;
                                    // printf("fdid: %d\n", targetfdid);
                                    int messageSize = sizeof(message);
                                    // printf("%d\n",messageSize);
                                    send(targetfdid, message, strlen(message), 0);
                                    memset(target,0,strlen(target));
                                    memset(message,0,strlen(message));

                                    break;
                                } 
                                else{
                                    printf("No Match2\n");
                                }

                            }
                            // send(socketDescriptor, message, strlen(message), 0);
                            // memset(message,0,strlen(message));
                        }
                        else if (strcmp(option,"4")==0){
                            close(socketDescriptor);
                        }
                        else if (strcmp(option,"5")==0){
                            close(socketDescriptor);
                        }
                        
                        memset(buffer,0,strlen(buffer));
                    }
                    // HandleTCPClient (socketDescriptor);
                }
            }

    }
/* NOT REACHED */
}
