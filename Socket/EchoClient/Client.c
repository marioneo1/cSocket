#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#define RCVBUFSIZE 1024 /* Size of receive buffer */
#define SENDBUFSIZE 1024 /*Size of send buffer */
#define LOGINBUFSIZE 1024 /*Size of login buffer */
#define PASSBUFSIZE 1024
#define BUFFER 1024

void DieWithError(char *errorMessage); /* Error handling function */

void HandleTCPClient(int newClientSock);

int main(int argc, char *argv[])
{
    int sock; /* Socket descriptor */
    int c2cSock;
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort; /* Echo server port */
    char *servlP = (char*)malloc(20*sizeof(char)); /* Server IP address (dotted quad) */
    char *c2cString = (char*)malloc(SENDBUFSIZE); /* String to send to echo server (Max 250)*/
    char *message = (char*)malloc(SENDBUFSIZE); /* String to send to server */
    char *input = (char*)malloc(RCVBUFSIZE);
    char *login = (char*)malloc(LOGINBUFSIZE);
    char *password = (char*)malloc(PASSBUFSIZE);
    char *option = (char*)malloc(2*sizeof(char));
    char *userlist = (char*)malloc(BUFFER);
    char *target_user = (char*)malloc(LOGINBUFSIZE);
    char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
    unsigned int echoStringLen, loginStringLen, c2cStringLen; /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd, receivedBytes, c2cBytesRcvd,c2cTotalBytesRcvd; /* Bytes read in single recv() and total bytes read */
    int userOption;
    int socketOpen = 0;
    struct sockaddr_in newServAddr; /* Local address */
    int newSock; /* Socket descriptor for server */
    int newClientSock;
    struct sockaddr_in newClntAddr; /* Client address */
    unsigned int newClntLen; /* Length of client address data structure */
    

    do{
    printf("----------------------------------\n");
    printf("0. Connect to the server\n");
    printf("1. Get user list\n");
    printf("2. Send a message\n");
    printf("3. Get my messages\n");
    printf("4. Initiate a chat with my friend\n");
    printf("5. Chat with my friend\n");
    printf("Your option <enter a number> :\t");

    fgets(input, RCVBUFSIZE, stdin);
    userOption = strtol(input, NULL, 10);

    switch(userOption){
        case 0:
        printf("Your choice is %d\n", userOption);

        /*Set server IP Address*/
        printf("Please enter the IP Address:\t");
        fgets(servlP, RCVBUFSIZE, stdin); 

        /*Set port number*/
        printf("Please enter the port number:\t");
        fgets(input, RCVBUFSIZE, stdin);
        echoServPort = strtol(input, NULL, 10);

        /* Create a reliable, stream socket using TCP */
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError(" socket () failed") ;


        /* Construct the server address structure */
        memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
        echoServAddr.sin_family = AF_INET; /* Internet address family */
        echoServAddr.sin_addr.s_addr = inet_addr(servlP); /* Server IP address */
        echoServAddr.sin_port = htons(echoServPort); /* Server port */


        /* Establish the connection to the echo server */
        if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError(" connect () failed");

        printf("Please log in.\n");

        /*Login*/
        printf("Username: ");
        fgets(login, LOGINBUFSIZE, stdin);
        password[strcspn(login,"\n")] = 0;
        // printf("login : %s", login);
        if (send(sock, login, strlen(login), 0) < 0){
            DieWithError("send() user sent a different number of bytes than expected");
        }

        printf("Password: ");
        fgets(password,PASSBUFSIZE,stdin);
        password[strcspn(password,"\n")] = 0;
        // printf("password: %s", password);
        if (send(sock, password, strlen(password), 0) < 0){
            DieWithError("send() pass sent a different number of bytes than expected");
        }
        
        /*Socket Condition Initialized*/
        socketOpen = 1;
        break;

        case 1:
        /*Send option to server*/
        printf("Your choice is %d\n", userOption);
        option = "1";
        if (send(sock, option, strlen(option), 0) < 0)
        DieWithError("send() option sent a different number of bytes than expected");
        printf("%s\n",option);

        /*Get Bytes of Array*/
        memset(userlist,0,strlen(userlist));
        if ((recv(sock, userlist, 1024, 0)) <= 0)
                DieWithError("recv() failed at getting num users or connection closed prematurely");

        int numUsers = strtol(userlist,NULL,10);
        printf("Num of users: %s\n",userlist);

        /*Get Num of users*/
   
        printf("Users:\n");
        if ((recv(sock, userlist, 1024, 0)) <= 0)
                DieWithError("recv() failed at getting usernames or connection closed prematurely");
        printf("%s",userlist);
        
        break;

        case 2:
        /*Send option to server*/
        printf("Your choice is %d\n", userOption);
        option = "2";
        if (send(sock, option, strlen(option), 0) < 0)
        DieWithError("send() option sent a different number of bytes than expected");
        printf("%s\n",option);

        printf("Please enter the username: ");
        fgets(target_user, LOGINBUFSIZE, stdin);
        target_user[strcspn(target_user,"\n")] = 0;
        // memset(target_user,0,strlen(target_user));
        if (send(sock, target_user, strlen(target_user), 0) < 0)
        DieWithError("send() option sent a different number of bytes than expected");
        printf("Target User:%s\n",target_user);
        

        printf("\nPlease enter the message: ");
        fgets(message, SENDBUFSIZE, stdin);
        message[strcspn(message,"\n")] = 0;

        echoStringLen = strlen(message); /* Determine input length */

        /* Send the string to the server */
        if (send(sock, message, strlen(message), 0) < 0)
        DieWithError("send() option sent a different number of bytes than expected");
        printf("%s\n",message);
        break;

        case 3:
        /*Send option to server*/
        printf("Your choice is %d\n", userOption);
        option = "3";
        if (send(sock, option, strlen(option), 0) < 0)
        DieWithError("send() option sent a different number of bytes than expected");
        printf("%s\n",option);

        totalBytesRcvd = 0;

        /* Receive the same string back from the server */
        printf("Received: "); /* Setup to print the echoed string */
        // while (totalBytesRcvd < echoStringLen)
        // {
        
            /* Receive up to the buffer size (minus i to leave space for a null terminator) bytes from the sender */
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
                DieWithError("recv() failed or connection closed prematurely");
            totalBytesRcvd += bytesRcvd; /* Keep tally of total bytes */
            echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
            printf(echoBuffer); /* Print the echo buffer */
            printf("\n");
        // }
        break;

        case 4:
        /*Send option to server*/
        printf("Your choice is %d\n", userOption);
        option = "4";
        if (send(sock, option, strlen(option), 0) < 0)
        DieWithError("send() option sent a different number of bytes than expected");
        printf("%s\n",option);

        printf("Please enter the port number:\t");
        fgets(input, RCVBUFSIZE, stdin);
        echoServPort = strtol(input, NULL, 10);

        /* Create socket for incoming connections */
        if ((newSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError( "socket () failed") ;

        /* Construct local address structure */
        memset(&newServAddr, 0, sizeof(newServAddr)); /* Zero out structure */
        newServAddr.sin_family = AF_INET; /* Internet address family */
        newServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
        newServAddr.sin_port = htons(echoServPort); /* Local port */

        /* Bind to the local address */
        if (bind(newSock, (struct sockaddr *)&newServAddr, sizeof(newServAddr)) < 0)
        DieWithError ( "bind () failed");
        /* Mark the socket so it will listen for incoming connections */
        if (listen(newSock, 7) < 0)
        DieWithError("listen() failed") ;

        printf("Server Started!\n");

        for(;;){
            newClntLen = sizeof(newClntAddr);
            if((newClientSock = accept(newSock,(struct sockaddr *) &newClntAddr,&newClntLen))<0)
            {
                DieWithError("accept() failed");
            }
            /* clntSock is connected to a client! */
            printf("Handling client %s\n", inet_ntoa(newClntAddr.sin_addr));
            HandleTCPClient(newClientSock);
        }
        break;

        case 5:
        /*Send option to server*/
        printf("Your choice is %d\n", userOption);
        option = "5";

        /*Set server IP Address*/
        printf("Please enter the IP Address:\t");
        fgets(servlP, RCVBUFSIZE, stdin); 

        /*Set port number*/
        printf("Please enter the port number:\t");
        fgets(input, RCVBUFSIZE, stdin);
        echoServPort = strtol(input, NULL, 10);

        /* Create a reliable, stream socket using TCP */
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError(" socket () failed") ;


        /* Construct the client server address structure */
        memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
        echoServAddr.sin_family = AF_INET; /* Internet address family */
        echoServAddr.sin_addr.s_addr = inet_addr(servlP); /* Server IP address */
        echoServAddr.sin_port = htons(echoServPort); /* Server port */


        /* Establish the connection to the client server */
        if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError(" connect () failed");

        for(;;){
        printf("Enter Message: ");
        fgets(c2cString, RCVBUFSIZE, stdin); 
        c2cStringLen = strlen(c2cString);
        int bytesSentToClient;

        /* Send the string to the client */
        if ((bytesSentToClient = send(sock, c2cString, c2cStringLen, 0)) != c2cStringLen)
        DieWithError("send() sent a different number of bytes than expected");


        /* Receive the string back from the client */
        c2cTotalBytesRcvd = 0;

        while (c2cTotalBytesRcvd < c2cStringLen)
            {
        
            /* Receive up to the buffer size (minus i to leave space for a null terminator) bytes from the sender */
            if ((c2cBytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
                DieWithError("recv() failed or connection closed prematurely");
            c2cTotalBytesRcvd += c2cBytesRcvd; /* Keep tally of total bytes */
            echoBuffer[c2cBytesRcvd] = '\0'; /* Terminate the string! */
            }
        }

        socketOpen = 1;

        printf("\n"); /* Print a final linefeed */

        break;
    }

    /*Input Checkers*/
    // printf("%s",servlP);
    // printf("%d\n",echoServPort);

    printf("\n"); /* Print a final linefeed */
    printf("Socket still open (1=True, 0=False):\t%d\n",socketOpen);
    } while(socketOpen == 1);

    // close(sock);
    exit(0);
}
