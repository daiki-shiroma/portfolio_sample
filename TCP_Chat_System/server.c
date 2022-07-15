#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#define TIMEOUT 10
#define MAXCLIENTS 5
#define _UNIX03_THREADS
int csock[MAXCLIENTS+1]={-1,-1,-1,-1,-1,-1};
char rbuf[MAXCLIENTS+1][1024];
char user_name_list[MAXCLIENTS+1][1024];
int k=0;
int connect_check(int);
pthread_t handle[MAXCLIENTS];
int flag1=0;
int flag2=0;
int flag3=0;
int flag4=0;
int flag5=0;
pthread_mutex_t mutex;

void *client_socket(void *arg) {
    int client_number;
    
    if (flag1!=1) {
        flag1=1;
        client_number=1;
    }
    
    else if (flag2!=1) {
        flag2=1;
        client_number=2;
    }
    else if (flag3!=1) {
        flag3=1;
        client_number=3;
    }
    else if (flag4!=1) {
        flag4=1;
        client_number=4;
    }
    else {
        flag5=1;
        client_number=5;
    }
    
    do {
        fd_set rfds;
        struct timeval tv;
        FD_ZERO(&rfds);
        FD_SET(0,&rfds);
        FD_SET(csock[client_number],&rfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        if(select(csock[client_number]+1,&rfds,NULL,NULL,&tv)>0 ){
            
            memset(rbuf[client_number], '\0', sizeof(rbuf[client_number]));
            if(FD_ISSET(csock[client_number],&rfds)) {
                
                size_t readlen=read(csock[client_number],rbuf[client_number],sizeof(rbuf[client_number]));
                if ( readlen < 0) {
                    perror("read");
                    exit(1);
                }
                else if (readlen == 0) { //ctr+D
                    //s7
                    for (int j=1; j<=MAXCLIENTS; j++){
                        write(csock[j],user_name_list[client_number],sizeof(user_name_list[client_number]));
                        /*write(csock[j],"==>",sizeof("==>"));*/
                        write(csock[j],"connection ended\n",sizeof("connection ended\n"));
                    }
                    
                    //  printf("%s: exit\n",user_name_list[client_number]);
                    close(csock[client_number]);
                    
                    memset(user_name_list[client_number], '\0', sizeof(user_name_list[client_number]));
                    csock[client_number]=-1;
                    pthread_mutex_lock(&mutex);
                    k--;
                    pthread_mutex_unlock(&mutex);
                    switch (client_number){
                        case 1:
                            flag1=0;
                            break;
                            
                        case 2:
                            flag2=0;
                            break;
                            
                        case 3:
                            flag3=0;
                            break;
                            
                        case 4:
                            flag4=0;
                            break;
                            
                        case 5:
                            flag5=0;
                            break;
                            
                    }
                }
                
                else {
                    write(1,user_name_list[client_number],sizeof(user_name_list[client_number]));
                    write(1,"==>",sizeof("==>"));
                    write(1,rbuf[client_number],sizeof(rbuf[client_number]));
                    
                    //writeで他のsocketにも出力
                    for (int j=1; j<=MAXCLIENTS; j++){
                        if(j!=client_number && csock[j]!=-1){
                            write(csock[j],user_name_list[client_number],sizeof(user_name_list[client_number]));
                            /* write(csock[j],"==>",sizeof("==>"));*/
                            write(csock[j],rbuf[client_number],/*sizeof*/strlen(rbuf[client_number]));
                        }
                    }
                    
                }
            }
            
        }
    } while (1);
    return NULL;
}


int main(int argc,char **argv) {
    int sock;
    int temp;
    int reuse;
    struct sockaddr_in svr;
    pthread_t handle1;
    pthread_t handle2;
    pthread_t handle3;
    pthread_t handle4;
    pthread_t handle5;
    pthread_mutex_init(&mutex, NULL);
    
    if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
        perror("socket");
        exit(1);
    }
    
    reuse=1;
    if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0) {
        perror("setsockopt");
        exit(1);
    }
    
    bzero(&svr,sizeof(svr));
    svr.sin_family=AF_INET;
    svr.sin_addr.s_addr=htonl(INADDR_ANY);
    svr.sin_port=htons(10140);
    
    if(bind(sock,(struct sockaddr *)&svr,sizeof(svr))<0) {
        perror("bind");
        exit(1);
    }
    
    if (listen(sock,5)<0) {
        perror("listen");
        exit(1);
    }
    
    while(1){
        pthread_mutex_lock(&mutex);
        if(k>0){
            
            if (flag1!=1)  pthread_create(&handle1, NULL,client_socket, NULL);
            else if (flag2!=1) pthread_create(&handle2, NULL,client_socket, NULL);
            else if (flag3!=1) pthread_create(&handle3, NULL,client_socket, NULL);
            else if (flag4!=1) pthread_create(&handle4, NULL,client_socket, NULL);
            else if (flag5!=1) pthread_create(&handle5, NULL,client_socket, NULL);
            
        }
        pthread_mutex_unlock(&mutex);
        
        temp=connect_check(sock);
        if(temp!=-1) {
            pthread_mutex_lock(&mutex);
            k++;
            pthread_mutex_unlock(&mutex);
            for (int i=1;i<MAXCLIENTS+1;i++){
                if ( csock[i]==-1){
                    csock[i]=temp;
                    break;
                }
            }
        }
        else close(csock[0]);
        
        for (int i=1;i<MAXCLIENTS+1;i++){
            printf("user_name: %s\n",user_name_list[i]);
            printf("addr: %d\n",csock[i]);
        }
    }
    
    pthread_mutex_destroy(&mutex);
}

int connect_check(int sock){
    struct sockaddr_in clt;
    struct hostent *cp;
    unsigned int clen;
    int client_sock=-1;
    int i,flag=0;
    
    clen = sizeof(clt);
    if ( ( csock[0] = accept(sock,(struct sockaddr *)&clt,&clen) ) <0 ) {
        perror("accept");
        exit(2);
    }
    
    pthread_mutex_lock(&mutex);
    if(k<MAXCLIENTS) {
        write(csock[0], "REQUEST ACCEPTED\n", strlen("REQUEST ACCEPTED\n"));
        
        cp = gethostbyaddr((char *)&clt.sin_addr,sizeof(struct in_addr),AF_INET);
        printf("[%s]\n",cp->h_name);
        
        memset(rbuf[0], '\0', sizeof(rbuf[0]));
        if ( ( read(csock[0],rbuf[0],sizeof(rbuf[0])) ) < 0) {
            perror("read");
            exit(1);
        }
        
        else{
            size_t length = strlen(rbuf[0]);
            if (length > 0 && rbuf[0][length - 1] == '\n') {
                rbuf[0][--length] = '\0';
            }
            for (i=1;i<MAXCLIENTS+1;i++){
                if(strcmp(rbuf[0],user_name_list[i])==0){
                    write(csock[0], "USERNAME REJECTED\n", strlen("USERNAME REJECTED\n"));
                    // return client_sock;
                    flag=1;
                    break;
                }
            }
            
            if (flag!=1){
                for (i=1;i<MAXCLIENTS+1;i++){
                    if (user_name_list[i][0]=='\0'){
                        write(csock[0], "USERNAME REGISTERED\n", strlen("USERNAME REGISTERED\n"));
                        memset(user_name_list[i], '\0', sizeof(user_name_list[i]));
                        strcpy(user_name_list[i],rbuf[0]); //ユーザー名登録
                        client_sock=csock[0]; //ソケットの登録
                        break;
                    }
                }
            }
        }
        
    }
    
    else{
        write(csock[0], "REQUEST REJECTED\n", strlen("REQUEST REJECTED\n"));
    }
    
    pthread_mutex_unlock(&mutex);
    
    return client_sock;
}

