#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h> /* for close() */
#include <string.h> /* memset */
#include <stdlib.h>
#define RCVBUFSIZE 1024 /* Size of receive buffer */
void DieWithError(char *errorMessage); /* Error handling function */


void HandleTCPClient(int clntSocket)
{
    char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
    int recvMsgSize; /* Size of received message */
    int firstInstance = 0;
    char *buffer = (char*)malloc(RCVBUFSIZE);

    

    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0){
        printf("Recieve message failed1 \n");
        close(clntSocket);
    }
    if(firstInstance < 1){
        printf("From client: %s", echoBuffer);
        printf("Enter message:");
        fgets(buffer, RCVBUFSIZE, stdin); 
        if (send(clntSocket, buffer, RCVBUFSIZE, 0) < 0)
            DieWithError("send() failed");
        strcpy(buffer,"");
        
    }

    firstInstance = 1;

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0) /* zero indicates end of transmission */
    {
        printf("If you want to exit chat please type 'Bye'\n");
        /* See if there is more data to receive */
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0){
            printf("Recieve message failed2 \n");
            close(clntSocket);
        }
        echoBuffer[recvMsgSize] = '\0'; /* Terminate the string! */
        printf("From client: %s", echoBuffer);
        if (strcmp(echoBuffer,"Bye\n")==0){
            close(clntSocket);
            exit(1);
        }
        strcpy(echoBuffer,"");

        printf("Enter message:");
        fgets(buffer, RCVBUFSIZE, stdin); 
        /* Send message to client */
        if (send(clntSocket, buffer, RCVBUFSIZE, 0) < 0)
            DieWithError("send() failed");
        if (strcmp(buffer,"Bye\n")==0){
            close(clntSocket);
            exit(1);
        }
        strcpy(buffer,"");
    }
}
