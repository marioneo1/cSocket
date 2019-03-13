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
        printf("%s", echoBuffer);

        // /* Send message to client */
        // if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
        //     DieWithError("send() failed");
    }

    firstInstance = 1;

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0) /* zero indicates end of transmission */
    {
        
        /* Send message to client */
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            DieWithError("send() failed");

        /* See if there is more data to receive */
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0){
            printf("Recieve message failed2 \n");
            close(clntSocket);
        }

        echoBuffer[recvMsgSize] = '\0'; /* Terminate the string! */
        printf("%s", echoBuffer);

        strcpy(echoBuffer,"");
    }
}
