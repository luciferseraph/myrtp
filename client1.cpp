/*
 //  client1.cpp
 //  EE450
 //
 //  Created by XuanKe on 15/4/12.
 //  Copyright (c) 2015年 XuanKe. All rights reserved.
 //
 */

#ifndef __HEADER_HAS_DEFINE__
#define __HEADER_HAS_DEFINE__

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <signal.h>
#include <string>


#endif
#define PORT_1 "21074"
#define IP_1 "localhost"
using namespace std;
#define MLEN 13
#define INET6_ADDRSTRLEN 46
struct my_Client
{
    string search;
    string key;
};
struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr;


void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(){
    
    int i;/*loop*/
    fstream readFile;/*read*/
    int flag=0;
    
    readFile.open("client1.txt",ios::in);
    if(!readFile)
    {perror("can't open!");
        exit(-1);
    }
    my_Client client1[12];
    
    /*code*/
    for(i=0;i<12;i++)
        readFile>>client1[i].search>>client1[i].key;
    
    readFile.close();
    /*for(i=0;i<12;i++)
     cout<<client1[i].search<<" "<<client1[i].key<<endl;*/
    cout<<"Please Enter Your Search (USC, UCLA etc.):";
    string search;
    string key;
    
    cin>>search;
    for(i=0;i<12;i++)
    {
        if(client1[i].search==search)
        {
            key=client1[i].key;
            flag=1;
        }
    }
    if(flag==0)
    {
        printf("can't find the search!\n");
        exit(-1);
    }
    
    cout<<"The Client 1 has received a request with search word "<<search;
    cout<<", which maps to key "<<key<<"."<<endl;
    string msg="GET "+key;
    const char * sendmsg;
    sendmsg=msg.c_str();
    /*codes below are referenced from Beej's book*/
    int fd;
    int rv;
    long numbytes;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if ((rv = getaddrinfo(IP_1, PORT_1, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(-1);
    }
    
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((fd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("socket");
            continue;
        }
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "failed to bind socket\n");
        
        exit(-1);
    }
    
    /*    cout<<strlen(sendmsg);*/
    
    if ((numbytes = ::sendto(fd, sendmsg, strlen(sendmsg), 0,p->ai_addr, p->ai_addrlen)) == -1)
    {
        perror("sendto");
        exit(1);
    }
    
      /*codes above are referenced from Beej's book*/
    sockaddr_in addrLocal;
    int nTemp=sizeof(addrLocal);
    
    
    
    if(getsockname(fd, (struct sockaddr*)&addrLocal, (socklen_t*)&nTemp)!=0)
    {
        perror("Cant' get local sock name!\n");
        exit(-1);
    }
    
    char          udp_guest_ip[20];
    memset(udp_guest_ip, 0, sizeof(udp_guest_ip));
    /*getpeername(UDP_fd, (struct sockaddr *)&remaddr, &udp_guest_len);*/
    
    inet_ntop(AF_INET, &addrLocal.sin_addr, udp_guest_ip, sizeof(udp_guest_ip));
    /*port and IP */
    struct hostent *local;
    local=gethostbyname(IP_1);
    char * IP_add=inet_ntoa(*((struct in_addr *)local->h_addr));
    struct sockaddr_in * tmp=(sockaddr_in*)p->ai_addr;
    socklen_t udp_len = sizeof(struct sockaddr_in);
    char          udp_ip[20];
    memset(udp_ip, 0, sizeof(udp_ip));
    /*getpeername(UDP_fd, (struct sockaddr *)&remaddr, &udp_guest_len);*/
    
    inet_ntop(AF_INET, &(tmp->sin_addr), udp_ip, sizeof(udp_ip));
    
    cout<<"The Client 1 sends the request "<<sendmsg<<" to the Server 1 with port number "<<ntohs(tmp->sin_port);
    cout<<" and IP address "<<udp_ip<<"."<<endl;
    cout<<"The Client1’s port number is "<<ntohs(addrLocal.sin_port);
    cout<<" and the IP address is "<<udp_ip<<"."<<endl;
    
    /*receive*/
    
    
    char buf[MLEN];
    socklen_t addr_len;
    
    memset(buf, 0, sizeof(buf));
    
    
    addr_len = sizeof(their_addr);
    if ((numbytes = ::recvfrom(fd, buf, MLEN , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }
    buf[numbytes] = 0;
    if(getsockname(fd, (struct sockaddr*)&addrLocal, (socklen_t*)&nTemp)!=0)
    {
        perror("Cant' get local sock name!\n");
        exit(-1);
    }
    cout<<"The Client 1 received the value "<<(buf+5)<<" from the Server 1 ";
    cout<<" with port number "<<ntohs(tmp->sin_port)<<" and IP address "<<udp_ip<<"."<<endl;
    cout<<"The Client1’s port number is "<<ntohs(addrLocal.sin_port)<<" and IP address is "<<udp_ip<<"."<<endl;
    
    
    
    close(fd);
    
    return 0;
    
}

