#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include "httpstr.h"
#include "determinante.h"
#include "server.h"


int count=0;
pthread_mutex_t mutex;
pthread_cond_t full;


void fatal(char *string){
            printf("\n");
            printf(string);
            printf("\n");
            exit(1);
}

int avviaserver(void){
         int s,b,l,on=1;
         struct sockaddr_in channel;
         
         memset((struct sockaddr *) &channel, 0 , sizeof(struct sockaddr_in));
         channel.sin_family=AF_INET;
         channel.sin_addr.s_addr=htonl(INADDR_ANY);
         channel.sin_port=htons(SERVER_PORT);

         s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
         if(s<0) fatal("socket failed");
         setsockopt(s,SOL_SOCKET,SO_REUSEADDR, (char*) &on , sizeof(on));

         b=bind(s,(struct sockaddr *) &channel , sizeof(channel));
         if(b<0) fatal("bind failed");

         l=listen(s, QUEUE_SIZE);
         if(l<0) fatal("listen failed");
         printf("\nServer in ascolto sulla porta %d.\n", SERVER_PORT);

         return(s);

}


/*void leggiint(int *buff , int sa){ 
                  
                 read(sa,buff,sizeof(int));
                            
}*/

void leggistr(char *buf , int sa){ 
                 int bytes;

                 bytes=read(sa,buf,BUF_SIZE);
                 buf[bytes]='\0';           
}


/*void stampastr(char *buf , int sa , int len , int enablehead){
                 char httphd[400];

                 if(enablehead){
                                sprintf(httphd,"HTTP/1.1 200 OK\nServer: det/1.0\nDate: Sat, 25 Ago 2007 20:42:44 GMT\nContent-Type: text/html\nLast-Modified: 25 Ago 2007 20:42:44 GMT\nContent-Length: %d\n\n", len);
                                write(sa , httphd , strlen(httphd));
                                }
                 write(sa , buf , strlen(buf));
}

void stampastr(char *buf , int sa){
                 write(sa , buf , BUF_SIZE);
}*/

void stampastr(char *buf , int sa , int conn){
                 char app[BUF_SIZE];
                 int bytes=0,k=0,term=0;

                 if (conn) sprintf(app,"HTTP/1.1 200 OK\nConnection: keep-alive\nServer: det/1.0\nContent-Type: text/html\nLast-Modified: 25 Ago 2007 20:42:44 GMT\nContent-Length: %d\n\n", strlen(buf));
                 else sprintf(app,"HTTP/1.1 200 OK\nConnection: close\nServer: det/1.0\nContent-Type: text/html\nLast-Modified: 25 Ago 2007 20:42:44 GMT\nContent-Length: %d\n\n", strlen(buf));
                 bytes=write(sa , app , strlen(app));

                 while(!term){ 
                           term=strpiece(buf , k*BUF_SIZE , BUF_SIZE , app);
                           k++;
                           bytes=bytes+write(sa , app , strlen(app));
                         }
                 printf("\nTrasmessi: %d bytes.\n\n\n", bytes);
}

/*void stampaperc(char *buf , int sa){
                 write(sa , buf , BUF_SIZE);
}*/


void server(void *x){
    
    int sa = (int) x;
           
    determinante(sa);
    close(sa);   
    printf("\nConnessione terminata (Server socket accepted: %d).\n\n", sa);
        
    pthread_mutex_lock(&mutex);
    count--;
    pthread_cond_broadcast(&full);
    pthread_mutex_unlock(&mutex);
    
    
    pthread_exit(0);
    
}

int main(){
        int s,sa;  
        pthread_t th;
        
        pthread_mutex_init(&mutex , NULL);
        pthread_cond_init(&full , NULL);
        s=avviaserver();
                
        while(1){
             pthread_mutex_lock(&mutex);
             if(count++==MAX_CONN){
                          printf("\nRaggiunto il numero massimo di connessioni...\n\n");                  
                          pthread_cond_wait(&full , &mutex);
             }
             pthread_mutex_unlock(&mutex);
             
             sa=accept(s,0,0);
                                       
             if(sa<0) fatal("accept failed");
             
             printf("\nConnessione stabilita (Server socket accepted: %d).\n\n", sa);
             pthread_create((void*) &th , NULL , (void*) &server , (void*) sa);
             
        }
          
}
