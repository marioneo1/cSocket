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

int main(int argc, char *argv[])
{
    int sock; /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort; /* Echo server port */
    char *servlP = (char*)malloc(20*sizeof(char)); /* Server IP address (dotted quad) */
    // char *echoString = (char*)malloc(SENDBUFSIZE); /* String to send to echo server (Max 250)*/
    char *message = (char*)malloc(SENDBUFSIZE); /* String to send to server */
    char *input = (char*)malloc(RCVBUFSIZE);
    char *login = (char*)malloc(LOGINBUFSIZE);
    char *password = (char*)malloc(PASSBUFSIZE);
    char *option = (char*)malloc(2*sizeof(char));
    char *userlist = (char*)malloc(BUFFER);
    char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
    unsigned int echoStringLen, loginStringLen; /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd, receivedBytes; /* Bytes read in single recv() and total bytes read */
    int userOption;
    int socketOpen = 0;
    

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
        printf("login : %s", login);
        if (send(sock, login, strlen(login), 0) < 0){
            DieWithError("send() user sent a different number of bytes than expected");
        }

        printf("Password: ");
        fgets(password,PASSBUFSIZE,stdin);
        password[strcspn(password,"\n")] = 0;
        printf("password: %s", password);
        if (send(sock, password, strlen(password), 0) < 0){
            DieWithError("send() pass sent a different number of bytes than expected");
        }

        // if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0){
        //     DieWithError("recv() failed or connection closed prematurely");
        // }
        // echoBuffer[bytesRcvd] = '\0';
        // printf("%s\n",echoBuffer);
        
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

        // printf("Please enter the username: ");
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
        while (totalBytesRcvd < echoStringLen)
        {
        
            /* Receive up to the buffer size (minus i to leave space for a null terminator) bytes from the sender */
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
                DieWithError("recv() failed or connection closed prematurely");
            totalBytesRcvd += bytesRcvd; /* Keep tally of total bytes */
            echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
            printf(echoBuffer); /* Print the echo buffer */
            printf("\n");
        }
        break;

        case 4:
        // /*Send option to server*/
        // printf("Your choice is %d\n", userOption);
        // option = "4";
        // if (send(sock, option, strlen(option), 0) < 0)
        // DieWithError("send() option sent a different number of bytes than expected");
        // printf("%s\n",option);

        socketOpen = 0;
        // close(sock);
        break;

        case 5:
        // /*Send option to server*/
        // printf("Your choice is %d\n", userOption);
        // option = "5";
        // if (send(sock, option, strlen(option), 0) < 0)
        // DieWithError("send() option sent a different number of bytes than expected");
        // printf("%s\n",option);

        break;
        case 6:
        printf("Your choice is %d\n", userOption);
        socketOpen = 0;
        // close(sock);
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
