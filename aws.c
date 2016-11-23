/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/fcntl.h>

#define TCPPORT "25341"  // the port users will be connecting to

#define UDPPORT "24341" // UDP Port

#define APORT "21341"    // ServerA Port
#define BPORT "22341"    // ServerB Port
#define CPORT "23341"    // ServerC Port


#define BACKLOG 10     // how many pending connections queue will hold

#define MAXDATASIZE 50000 // max number of bytes we can get at once 

void sigchld_handler(int s)                                                 //Beej Guide reference starts
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;                                                    //Beej Guide reference ends
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)                                      //Beej Guide reference starts
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);                        //Beej Guide reference ends
}

int main(void)
{
    int sockfd, new_fd,numbytes;  // listen on sock_fd, new connection on new_fd    //Beej Guide reference starts
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;        
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char buf[MAXDATASIZE]; //Holding Reduction

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    int *arr = malloc(sizeof(int) * 1000); //array to hold input numbers.

    if ((rv = getaddrinfo(NULL, TCPPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("%s\n", "The AWS is up and running");

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    //printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        memset(&buf,0, sizeof buf);
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            //perror("accept problem");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        //printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener

            if ((numbytes = recv(new_fd, buf, 5, 0)) == -1) {  // Receive Reduction from client.
                perror("recv");
                exit(1);
            }

            buf[numbytes] = '\0';                                              //Beej Guide reference ends.

            // Receive numbers.

            int received_int = 0;
            int return_status = 0;
            int received_val = 0;
            int i=0;

            int totalNumber;
            totalNumber = 0;

            //Receive total number of elements to receive from client.
            return_status = read(new_fd, &received_int, sizeof(received_int));     //Stack overflow
            if (return_status > 0) {                                               //Stack overflow
                received_val = ntohl(received_int);                                //Stack overflow
                totalNumber = received_val;
                //fprintf(stdout, "totalNumber = %d\n", totalNumber);    
            }
            else {
                // Handling erros here  
                perror("Total Number read problem");
            }

            received_val  = 0;
            int ytr;
            ytr = 0;     


           // Read all numbers sent by client.
           while(ytr < totalNumber)
            {
                return_status = read(new_fd, &received_int, sizeof(received_int));      //Stack overflow
                if (return_status > 0) {                                                //Stack overflow
                    received_val = ntohl(received_int);                                 //Stack overflow
                    arr[i] = received_val;
                    //fprintf(stdout, " i = %d Received int = %d\n", i,arr[i]);
                    i++;
                }
                else {
                    // Handling erros here  
                    perror("read problem");
                    break;
                 }
                 ytr++;
            }
            printf("The AWS received %d numbers from the client using TCP over port %s\n",i, TCPPORT);

    int sendSize = i/3;

/* Open UDP Socket to communicate to Backend server A,B and C's */


    int udpsockfd;                                                              //Beej Guide reference starts
    struct addrinfo udphints, *servinfo, *p;
    int rv;
    int udpnumbytes;

    memset(&udphints, 0, sizeof udphints);
    udphints.ai_family = AF_INET;
    udphints.ai_socktype = SOCK_DGRAM;


    //Server A
    if ((rv = getaddrinfo("localhost", APORT, &udphints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((udpsockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }
    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }
  
    // Send Reduction function to Server A.
    if ((numbytes = sendto(udpsockfd, buf, strlen(buf), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);                                                                //Beej Guide reference ends.
    }                                                                                   
    // Send numbers to Server A.
    int32_t arrayToSendA[sendSize+1];
    arrayToSendA[0] = htonl(sendSize);    //send total number of elements to send A as first element.
    int ia = 0;
    for (ia = 0 ; ia < sendSize ; ++ia) {
        arrayToSendA[ia+1] = htonl(arr[ia]);
        //printf("%d\n",arr[i] );
    }
    sendto(udpsockfd, arrayToSendA, sizeof(arrayToSendA), 0, p->ai_addr, p->ai_addrlen); //Beej Guide reference
    printf("The AWS has sent %d numbers to Backend-Server A\n",sendSize);
    freeaddrinfo(servinfo);
    close(udpsockfd);

    //Send to Server B

    int udpsockfdB;                                                               //Beej Guide reference starts
    struct addrinfo udphintsB, *servinfoB, *pB;
    int rvB;
    int udpnumbytesB;

     if ((rvB = getaddrinfo("localhost", BPORT, &udphints, &servinfoB)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rvB));
        return 1;
    }
    // loop through all the results and make a socket
    for(pB = servinfoB; pB != NULL; pB = pB->ai_next) {
        if ((udpsockfdB = socket(pB->ai_family, pB->ai_socktype,
                pB->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }
    if (pB == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }
   
    // Send Reduction function to Server B.
    if ((udpnumbytesB = sendto(udpsockfdB, buf, strlen(buf), 0,
             pB->ai_addr, pB->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);                                                                //Beej Guide reference ends.
    }
    // Send numbers to Server B.
    int32_t arrayToSendB[sendSize+1];
    arrayToSendB[0] = htonl(sendSize);        //send total number of elements to send B as first element.
    int q=0;
    int ib = 0;
    for (ib = sendSize ; ib < (sendSize+sendSize) ; ib++) {
        arrayToSendB[q+1] = htonl(arr[ib]);
        q++;
        //printf("%d\n",arr[i]);
    }
    sendto(udpsockfdB, arrayToSendB, sizeof(arrayToSendB), 0, pB->ai_addr, pB->ai_addrlen); //Beej Guide reference.
    printf("The AWS has sent %d numbers to Backend-Server B\n",sendSize);
    freeaddrinfo(servinfoB);
    close(udpsockfdB);


    //Send to Server C

    int udpsockfdC;                                                                 //Beej Guide reference starts
    struct addrinfo udphintsC, *servinfoC, *pC;
    int rvC;
    int udpnumbytesC;

     if ((rvC = getaddrinfo("localhost", CPORT, &udphints, &servinfoC)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rvC));
        return 1;
    }
    // loop through all the results and make a socket
    for(pC = servinfoC; pC != NULL; pC = pC->ai_next) {
        if ((udpsockfdC = socket(pC->ai_family, pC->ai_socktype,
                pC->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }
    if (pC == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }
    
    // Send Reduction function to Server C.
    if ((udpnumbytesC = sendto(udpsockfdC, buf, strlen(buf), 0,
             pC->ai_addr, pC->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);                                                                    //Beej Guide reference ends.
    }
    // Send numbers to Server C.
    int32_t arrayToSendC[sendSize+1];
    arrayToSendC[0] = htonl(sendSize);                  //send total number of elements to send C as first element.
    int w = 0;
    int start_num = sendSize+sendSize;
    int end_num = sendSize+sendSize+sendSize;
    int ic =0;
    for (ic = start_num ; ic < end_num ; ic++) {
        arrayToSendC[w+1] = htonl(arr[ic]);
        w++;
        //printf("%d\n",arr[i] );
    }
    if(sendto(udpsockfdC, arrayToSendC, sizeof(arrayToSendC), 0, pC->ai_addr, pC->ai_addrlen) < 1) //Beej Guide reference
        perror("send");
    printf("The AWS has sent %d numbers to Backend-Server C\n ",sendSize);
    freeaddrinfo(servinfoC);
    close(udpsockfdC);

    /* -------------------------------------------------------------------------------------------*/

    
    //Receive value from Back End Server's 

    int awsudpsockfd;                                                               //Beej Guide reference starts
    struct addrinfo awsudphints, *awsservinfo, *awsp;
    int awsrv;
    int awsudpnumbytes;
    struct sockaddr_storage the_addr;//their_address
    socklen_t awsaddr_len; 
    awsaddr_len = sizeof the_addr;
    char s[INET6_ADDRSTRLEN];

    memset(&awsudphints, 0, sizeof awsudphints);
    awsudphints.ai_family = AF_INET;
    awsudphints.ai_socktype = SOCK_DGRAM;
    awsudphints.ai_flags = AI_PASSIVE;

    if ((awsrv = getaddrinfo(NULL,UDPPORT, &awsudphints, &awsservinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(awsp = awsservinfo; awsp != NULL; awsp = awsp->ai_next) {
        if ((awsudpsockfd = socket(awsp->ai_family, awsp->ai_socktype,
                awsp->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(awsudpsockfd, awsp->ai_addr, awsp->ai_addrlen) == -1) {
            close(awsudpsockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (awsp == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }
    //freeaddrinfo(awsservinfo); // Never uncomment    

    //Receive from A                                                    
    char resultA[MAXDATASIZE];
    memset(&resultA,0,sizeof resultA);

    awsudpnumbytes = recvfrom(awsudpsockfd, resultA, sizeof(resultA), 0, (struct sockaddr *)&the_addr, &awsaddr_len);   //Beej Guide reference ends.

    resultA[awsudpnumbytes] = '\0';
    printf("The AWS received reduction result of %s from Backend-Server A using UDP over port 21341 and it is %s\n", buf,resultA);

    //Receive from B

    struct sockaddr_storage their_addrC;//their_address
    socklen_t awsaddr_lenC; 
    awsaddr_lenC = sizeof their_addrC;

    char resultC[MAXDATASIZE];
    memset(&resultC,0,sizeof resultC);

    int awsudpnumbytesC = recvfrom(awsudpsockfd, resultC, sizeof(resultC), 0, (struct sockaddr *)&their_addrC, &awsaddr_lenC);
    resultC[awsudpnumbytesC] = '\0';
    printf("The AWS received reduction result of %s from Backend-Server B using UDP over port 22341 and it is %s\n", buf, resultC);

    //Receive from C

    struct sockaddr_storage their_addrE;//their_address
    socklen_t awsaddr_lenE; 
    awsaddr_lenE = sizeof their_addrE;

    char resultE[MAXDATASIZE];
    memset(&resultE,0,sizeof resultE);

    int awsudpnumbytesE = recvfrom(awsudpsockfd, resultE, sizeof(resultE), 0, (struct sockaddr *)&their_addrE, &awsaddr_lenE);
    resultE[awsudpnumbytesE] = '\0';
    printf("The AWS received reduction result of %s from Backend-Server C using UDP over port 23341 and it is %s\n", buf, resultE);


    close(awsudpsockfd);


    int resA;
    sscanf(resultA,"%d",&resA);

    int resB;
    sscanf(resultC,"%d",&resB);

    int resC;
    sscanf(resultE,"%d",&resC);

    int final_result=0;

    if (strcmp(buf, "max") == 0) 
    {
        int maximum = resA;
        if(resB > resA)
        {
            maximum = resB;
        }    

        if(resC > resB && resC > resA)
        {
            maximum = resC;
        }
        final_result = maximum;  
    } 
    else if (strcmp(buf, "min") == 0)
    {
        int minimum = resA;
        if(resB < resA)
        {
            minimum = resB;
        }    

        if(resC < resB && resC < resA)
        {
            minimum = resC;
        }
        final_result = minimum;    
    } 
    else if( strcmp(buf, "sum") == 0)
    {
        int sum = resA + resB + resC;
        final_result = sum;     
    } 
    else /* default: SOS*/
    {
        int sos = resA+ resB + resC;  
        final_result = sos;    
    }

     printf("The AWS has successfully finished the reduction %s : %d\n", buf,final_result);

     /*--------------------------------------------------------------------------------------------------*/

    /*  End of UDP connection.*/

    // Send Data back to client.

     char finalValueSend[MAXDATASIZE];
     sprintf(finalValueSend,"%d",final_result);

    if (send(new_fd, finalValueSend, sizeof (finalValueSend), 0) < 1) //send final result to the client. 
        perror("send");

    printf("The AWS has successfully finished sending the reduction value to client\n");
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}