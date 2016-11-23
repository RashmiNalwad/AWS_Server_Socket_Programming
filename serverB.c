/*
** listener.c -- a datagram sockets "server" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MYPORT "22341"    // the port ServerB will be connecting to
#define SERVERPORT "24341" // UDP Port

#define MAXBUFLEN 50000

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)                                  //Beej Guide reference starts
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }
    freeaddrinfo(servinfo);

    printf("%s\n","The Server B is up and running using UDP on port 22341" );


    while(1) //TO keep server running till ctrl+c is pressed.
    {

    //Receive Reduction Function.
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    buf[numbytes] = '\0';                                                       //Beej Guide reference ends.

    // Receive numbers.
    int32_t array[1000];
    int32_t arrayReceived[1000] ;
    addr_len = sizeof their_addr;
    recvfrom(sockfd, arrayReceived, sizeof(arrayReceived), 0, (struct sockaddr *)&their_addr, &addr_len);
    int numbersRecvd = ntohl(arrayReceived[0]);
    printf("The Server B has received %d numbers\n", numbersRecvd);
    int i = 0;
    for (i = 0 ; i < numbersRecvd ; i++) { 
        array[i] = ntohl(arrayReceived[i+1]);
        //printf("%d\n", array[i]);
    }

    //Compute the result and send the result to the server.

    int result=0;

    if (strcmp(buf, "max") == 0) 
    {
        int maximum = array[0];
        int c = 0;
        for (c = 0; c < numbersRecvd; c++)
        {
            if (array[c] > maximum)
            {
                maximum  = array[c];
            }
        }    
        result = maximum;    
    } 
    else if (strcmp(buf, "min") == 0)
    {
        int minimum = array[0];
        int zx = 0;
        for (zx = 0; zx < numbersRecvd; zx++)
        {
            if (array[zx] < minimum)
            {
                minimum  = array[zx];
            }
        }    
        result = minimum;   
    } 
    else if( strcmp(buf, "sum") == 0)
    {
        int sum = 0;
        int zy = 0;
        for (zy = 0;zy <numbersRecvd; zy++)
        {
            sum = sum + array[zy];
        }    
        result = sum;    
    } 
    else /* default: SOS*/
    {
        int sos = 0;
        int z = 0;
        for (z = 0; z < numbersRecvd; z++)
        {
            sos = sos + (array[z] * array[z]);
        }    
        result = sos;    
    }


    //Send results to AWS Server.

    int udpsockfd;                                                                  //Beej Guide reference starts.
    struct addrinfo udphints, *awsservinfo, *awsp;
    int r;
    int udpnumbytes;

    memset(&udphints, 0, sizeof udphints);
    udphints.ai_family = AF_INET;
    udphints.ai_socktype = SOCK_DGRAM;
    udphints.ai_flags = AI_PASSIVE;

    //AWS Server
    if ((r = getaddrinfo("localhost", SERVERPORT, &udphints, &awsservinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and make a socket
    for(awsp = awsservinfo; awsp != NULL; awsp = awsp->ai_next) {
        if ((udpsockfd = socket(awsp->ai_family, awsp->ai_socktype,
                awsp->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }
    if (awsp == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }

    char *sendResult;
    char buffer[MAXBUFLEN];
    
    memset(buffer,0,sizeof(buffer));                                            //Beej Guide reference ends.
    
    //printf("Result is %d\n",result);
    sprintf(buffer,"%d",result);  // copy result into buffer
    
    sendResult = malloc(sizeof(char) * MAXBUFLEN);
    memset(sendResult,0,sizeof(sendResult));

    strcat(sendResult,buffer);
   
    printf(" The Server B has successfully finished the reduction %s : %s \n", buf,sendResult);
 

    // Send Reduced Result to AWS Server.
    if ((numbytes = sendto(udpsockfd, sendResult, strlen(sendResult), 0,
             awsp->ai_addr, awsp->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

   close(udpsockfd);

}
    close(sockfd);

    return 0;
}