//
//  server.cpp
//  EE450
//
//  Created by XuanKe on 15/4/15.
//  Copyright (c) 2015年 XuanKe. All rights reserved.
//
/*
 I reference some codes from Beej's book
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
#define s_PORT_1 21074
#define s_PORT_2 22074
#define s_PORT_3 23074
#define IP_Server "localhost"
using namespace std;
#define LEN 13
#define ROWS 4
#define TOTAL 13
struct myServer
{
    string key[TOTAL];
    string value[TOTAL];
    int num;
};

myServer server1;
myServer server2;
myServer server3;
void open();
void serverProcess(int, int,myServer * );


extern int errno;


void open()
{

    ifstream File1;
    ifstream File2;
    ifstream File3;
    File1.open("server1.txt");
    if(!File1)
    {perror("can't open!");
        exit(-1);
    }
    File2.open("server2.txt");
    if(!File2)
    {perror("can't open!");
        exit(-1);
    }
    File3.open("server3.txt");
    if(!File3)
    {perror("can't open!");
        exit(-1);
    }
    int i;
    for(i=0;i<ROWS;i++)
    {
        File1>>server1.key[i]>>server1.value[i];
        server1.num++;
    }
    for(i=0;i<ROWS;i++)
    {
        File2>>server2.key[i]>>server2.value[i];
        server2.num++;
    }
    for(i=0;i<ROWS;i++)
    {
        File3>>server3.key[i]>>server3.value[i];
        server3.num++;
    }
    
    File1.close();
    File2.close();
    File3.close();
}
int udpCreate()
{

    int fd;
    
    if((fd=socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
        perror("fd:Can't create socket!\n");
        return -1;
    }
    int on=1;
    if((setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    return fd;
}
int tcpCreate()
{
    /* get a tcp/ip socket */
    /* bind to an arbitrary return address */
    int fd;
    
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("cannot create socket");
        return -1;
    }
    int on=1;
    if((setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    

    return fd;
}


void serverProcess(int server_num, int port,myServer*s_server)
{
    struct hostent *local;
    local=gethostbyname(IP_Server);
    char * IP_add=inet_ntoa(*((struct in_addr *)local->h_addr));
    /*  
     for(int t=0;t<s_server->num;t++)
        cout<<s_server->key[t]<<endl;
     */
    char info[LEN]="";/*server reeceived info*/
    char backinfo[LEN]="";/*server back info*/
    char recvinfo[LEN]="";/*client receved info*/
    string key;
    string value;
    if(server_num==1)
    {/* UDP connection*/
        printf("The Server %d has UDP port number %d and IP address %s.\n",server_num,port,IP_add);
        int client_num=1;
        
        
        int UDP_fd;
        UDP_fd=udpCreate();
        if(UDP_fd<0)
        {
            perror("Can't create UDP socket!\n");
            exit(-1);
        }
        
        
        /*bind*/
        
        struct sockaddr_in myaddr;
        memset((char *)&myaddr, 0, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        myaddr.sin_port = htons(port);
        
        
        if (::bind(UDP_fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)
        {
            perror("bind: failed");
            exit(-1);
        }

        
        while (client_num<=2)
        {
            memset(info, 0, sizeof(info));
            
            memset(recvinfo, 0, sizeof(recvinfo));
            
            /*Receive info*/
            struct sockaddr_in remaddr;
            memset((char *)&remaddr, 0, sizeof(remaddr));
            socklen_t addrlen=sizeof(remaddr);
            int recvlen;			/* bytes received */
            
/*            strcpy(info, "GET key09");*/
           
             recvlen = recvfrom(UDP_fd, info, LEN, 0, (struct sockaddr *)&remaddr, &addrlen);
             if (recvlen <= 0) {
             
             perror("recvfrom: UDP receive error!");
             exit(-1);
             }
             else
             {
             info[recvlen] = 0;
             }
            
          
            
            socklen_t udp_guest_len = sizeof(remaddr);
            char          udp_guest_ip[20];
            memset(udp_guest_ip, 0, sizeof(udp_guest_ip));
            /*getpeername(UDP_fd, (struct sockaddr *)&remaddr, &udp_guest_len);*/
            
            inet_ntop(AF_INET, &remaddr.sin_addr, udp_guest_ip, sizeof(udp_guest_ip));
            
        /*Check if existing*/
            
            key="";
            int n;
            for(n=4;n<9;n++)
            {
                key=key+info[n];
            }
            cout<<"The Server 1 has received a request with key "<<key;
            cout<<" from client "<<client_num<<" with port number "<<ntohs(remaddr.sin_port)<<" and IP address "<<udp_guest_ip<<"."<<endl;
    
            int flag=0;
            
            int numb;
           
            for(numb=0;numb<s_server->num;numb++)
                if (key==s_server->key[numb])
                {
                    flag=1;
                    value=s_server->value[numb];
                    break;
                }
            if(flag==1)
            {
                
                string tmp="POST "+value;
                memset(backinfo, 0, sizeof(backinfo));
                strcpy(backinfo,tmp.c_str());
                /*sendto*/
                cout<<"The Server 1 sends the reply "<<backinfo<<" to Client "<<client_num;
                cout<<" with port number "<<ntohs(remaddr.sin_port)<<" and IP address "<<udp_guest_ip<<"."<<endl;
                
            }
            else
            {
        /*Can't find, TCP connection*/
                
                int TCP_fd;
               

                int tcp_len;
                TCP_fd=tcpCreate();
            
               

                struct sockaddr_in serveraddr;
                memset((char *)&serveraddr, 0, sizeof(serveraddr));
                serveraddr.sin_family = AF_INET;
            
                struct hostent *dest;
                dest=gethostbyname(IP_Server);
                memcpy(&serveraddr.sin_addr, dest->h_addr, sizeof(serveraddr.sin_addr));
                serveraddr.sin_port = htons(s_PORT_2);

                
                                
               
                if(connect(TCP_fd,(struct sockaddr *)&serveraddr,sizeof(struct sockaddr))<0)
                {
                    

                    perror("1 connect");
                    close(TCP_fd);
                    break;
                }
                
                /*info*/
                
                struct sockaddr_in serv, guest;
                socklen_t serv_len = sizeof(serv);
                socklen_t guest_len = sizeof(guest);
                char        serv_ip[20], guest_ip[20];
                getsockname(TCP_fd, (struct sockaddr *)&guest, &guest_len);
                getpeername(TCP_fd, (struct sockaddr *)&serv, &serv_len);
                inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
                inet_ntop(AF_INET, &serv.sin_addr, serv_ip, sizeof(serv_ip));
                
                cout<<"The Server 1 sends the request "<<info<<" to the Server 2."<<endl;
                cout<<"The TCP port number is "<<ntohs(guest.sin_port)<<" and the IP address is "<<guest_ip<<"."<<endl;
            
         
                tcp_len=::send(TCP_fd,info,LEN,0);
                if(tcp_len<=0)
                {
                    close(TCP_fd);
                    perror("TCP send error!\n");
                   break;
                }

                tcp_len=recv(TCP_fd,recvinfo,LEN,0);
            
                if(tcp_len<=0)
                {
                    close(TCP_fd);
                    perror("TCP receive error!\n");
                   break;
                }
                recvinfo[tcp_len]=0;
               
                printf("The Server 1 received the value %s from the Server 2 ",(recvinfo+5));
                cout<<"with port number " <<ntohs(serv.sin_port)<<" and IP address "<<serv_ip<<"."<<endl;
                numb=s_server->num;
                s_server->key[numb]=key;
                s_server->value[numb]="";
                int tmp1;
               	
                
                for(tmp1=5;tmp1<LEN;tmp1++)
                {
                   
                    s_server->value[numb]=s_server->value[numb]+recvinfo[tmp1];
                }
        
                s_server->num=s_server->num+1;
                memset(backinfo, 0, sizeof(backinfo));
                strncpy(backinfo, recvinfo, sizeof(recvinfo));
                /*backinfo[tcp_len]=0;*/

           
                cout<<"The Server 1 closed the TCP connection with the Server 2."<<endl;
                cout<<"The Server 1, sent reply "<<backinfo<<" to Client "<<client_num;
                cout<<" with port number "<<ntohs(remaddr.sin_port)<<" and IP address "<<udp_guest_ip<<"."<<endl;
                close(TCP_fd);

            }
            
            /*else end*/
            
            if (::sendto(UDP_fd, backinfo, strlen(backinfo), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
                perror("sendto");
            

            client_num++;
           
    
        }/*while end*/
       
        close(UDP_fd);
        return ;
        
    }
    else/*2 or 3*/
    {
       
        printf("The Server %d has TCP port number %d and IP address %s.\n",server_num,port,IP_add);
        
        char backinfo[LEN]="";
        int server_sockfd;/*server fd*/
        int child_fd;
        int client_sockfd;/*client fd*/
        int clen;
        struct sockaddr_in my_addr;   /*local*/
        struct sockaddr_in client_addr;/*client*/
        struct sockaddr_in  server_addr;/*server*/
        int sin_size;

        server_sockfd=tcpCreate();
        
        memset(&my_addr,0,sizeof(my_addr));
        my_addr.sin_family=AF_INET;
        my_addr.sin_port = htons(port);
    
        struct hostent *local;
        local=gethostbyname(IP_Server);
        memcpy(&my_addr.sin_addr, local->h_addr, sizeof(my_addr.sin_addr));
        if (::bind(server_sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {

            perror("bind failed");
            exit(-1);
        }
        if (listen(server_sockfd, 10) < 0) {
            perror("listen failed");
            exit(1);
        }
       
       
        
      
        while (1)
        {
            
/*
                ;*/
            memset(info, 0, sizeof(info));
            memset(backinfo, 0, sizeof(backinfo));
            memset(recvinfo, 0, sizeof(recvinfo));
            while((child_fd=::accept(server_sockfd, NULL, 0))<0)
            {
                ;
            }
           
            struct sockaddr_in serv, guest;
            socklen_t serv_len = sizeof(serv);
            socklen_t guest_len = sizeof(guest);
            char        serv_ip[20], guest_ip[20];
            getsockname(child_fd, (struct sockaddr *)&serv, &serv_len);
            inet_ntop(AF_INET, &serv.sin_addr, serv_ip, sizeof(serv_ip));
            
            getpeername(child_fd, (struct sockaddr *)&guest, &guest_len);
            inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
   

            
            if((clen=recv(child_fd,info,LEN,0))>0)
            {
                info[clen]=0;
                cout<<"The Server "<<server_num<<" has received a request with key "<<(info+4);
                cout<<" from the Server "<<server_num-1<<" with port number "<<ntohs(guest.sin_port)<<" and IP address "<<guest_ip<<endl;
            }
            else
            {
            perror("less than 0\n");
            
            }
            
            key="";
            int n;
            for(n=4;n<9;n++)
            {
                key=key+info[n];
            }
       
            int flag=0;
            int numb;
            
            for(numb=0;numb<s_server->num;numb++)
                if (key==s_server->key[numb])
                {
                    flag=1;
                    value=s_server->value[numb];
                    break;
                }
            if(flag==1)
            {
                
                string tmp="POST "+value;
                memset(backinfo, 0, sizeof(backinfo));

                strcpy(backinfo,tmp.c_str());
               
                clen=::send(child_fd,backinfo,LEN,0);
                if(clen<=0)
                {
                    close(child_fd);
                    perror("TCP send error!\n");
                    break;
                }
                cout<<"The Server "<<server_num<<" sends the reply "<<backinfo<< " to the Server "<<server_num-1;

                cout<<" with port number "<<ntohs(guest.sin_port)<<" and IP address "<<guest_ip<<"."<<endl;

                
            }
            else
            {
                if(server_num==3)
                {
                    cout<<"I have check through 3 servers but can't find it :("<<endl;
                    exit(-1);
                }
                client_sockfd=tcpCreate();
                /*can't find*/
                /*Can't find, TCP connection*/
               
        
                int client_len;
        
                
                
                
                struct sockaddr_in serveraddr_2;
                memset((char *)&serveraddr_2, 0, sizeof(serveraddr_2));
                serveraddr_2.sin_family = AF_INET;
                
                struct hostent *dest2;
                dest2=gethostbyname(IP_Server);
                memcpy(&serveraddr_2.sin_addr, dest2->h_addr, sizeof(serveraddr_2.sin_addr));
                serveraddr_2.sin_port = htons(s_PORT_3);
                
                
                
                if(connect(client_sockfd,(struct sockaddr *)&serveraddr_2,sizeof(struct sockaddr))<0)
                {
                    
                    
                    perror("2:connect");
                    close(client_sockfd);
                    break;
                }
                
                /*info*/
                
                struct sockaddr_in serv_2, guest_2;
                socklen_t serv_len_2 = sizeof(serv_2);
                socklen_t guest_len_2 = sizeof(guest_2);
                char        serv_ip_2[20], guest_ip_2[20];
                getsockname(client_sockfd, (struct sockaddr *)&guest_2, &guest_len_2);
                getpeername(client_sockfd, (struct sockaddr *)&serv_2, &serv_len_2);
                inet_ntop(AF_INET, &guest_2.sin_addr, guest_ip_2, sizeof(guest_ip_2));
                inet_ntop(AF_INET, &serv_2.sin_addr, serv_ip_2, sizeof(serv_ip_2));
                 cout<<"The Server 2 sends the request "<<info<<" to the Server 3."<<endl;
                cout<<"The TCP port number is "<<ntohs(guest_2.sin_port)<<" and the IP address is "<<guest_ip_2<<"."<<endl;
               
                
                client_len=::send(client_sockfd,info,LEN,0);
                if(client_len<=0)
                {
                    close(client_sockfd);
                    perror("TCP send error!\n");
                    break;
                }
                
                client_len=recv(client_sockfd,recvinfo,LEN,0);
                
                if(client_len<=0)
                {
                    close(client_sockfd);
                    perror("TCP receive error!\n");
                    break;
                }
                recvinfo[client_len]=0;
                printf("The Server 2 received the value %s from the Server 3 ",(recvinfo+5));
                cout<<"with port number " <<ntohs(serv.sin_port)<<" and IP address "<<serv_ip<<"."<<endl;
                cout<<"The Server 2 closed the TCP connection with the Server 3."<<endl;
                
                close(client_sockfd);
                s_server->key[numb]=key;
                s_server->value[numb]="";
                int tmp_1;
                for(tmp_1=5;tmp_1<strlen(recvinfo);tmp_1++)
                {
                    s_server->value[numb]=s_server->value[numb]+recvinfo[tmp_1];
                }
                
                s_server->num++;
                strncpy(backinfo, recvinfo,sizeof(recvinfo));
                client_len=::send(child_fd,recvinfo,LEN,0);
                if(client_len<=0)
                {
                    close(child_fd);
                    perror("TCP send error!\n");
                    break;
                }
                
             

                cout<<"The Server "<<server_num<<", sent the reply "<<backinfo<< " to the Server "<<server_num-1;
                
                cout<<" with port number "<<ntohs(guest.sin_port)<<" and IP address "<<guest_ip<<"."<<endl;

                
                
                
                
            }

            
        }
        close(child_fd);
        close(server_sockfd);
        return;
    }
    
                
            
        
    
    
   
}
int main()
{
    open();
    
    /*for(int t=0;t<server1.num;t++)
        cout<<server1.key[t]<<endl;
    for(int t=0;t<server2.num;t++)
        cout<<server2.key[t]<<endl;
    for(int t=0;t<server3.num;t++)
        cout<<server3.key[t]<<endl;
    */
    pid_t p1,p2,p3;
    p1=fork();
    if(p1==0)
    {
        serverProcess(1,s_PORT_1,&server1);
        return 0;
    }
    
    
    p2 = fork();
    if (p2 == 0)
    {
        serverProcess(2,s_PORT_2,&server2);
        return 0;
    }
    p3 = fork();
    if (p3 == 0)
    {
        serverProcess(3,s_PORT_3,& server3);
        return 0;
    }
    
    
    waitpid(p1, NULL, 0);
/*
    waitpid(p2, NULL, 0);

    waitpid(p3, NULL, 0);*/
    kill(p2, SIGKILL);
    
    kill(p3, SIGKILL);
    
    return 0;
}

