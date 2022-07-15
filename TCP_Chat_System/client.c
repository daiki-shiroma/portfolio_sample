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
#define TIMEOUT 10
#define PORT 10000
void c6_exception(int);
int main(int argc,char **argv)
{
    //s1
    int sock;
    int c2=0,c3=0,c4=0,c5=0;
    int result=0;
    char buf[1024];
    struct sockaddr_in host;
    struct hostent *hp;
    fd_set rfds;
    struct timeval tv;
    
    if (argc != 3) {
        fprintf(stderr,"Usage: %s hostname message\n",argv[0]);
        exit(1); }
    
    if ( ( sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP) ) < 0) {
        perror("client: socket");
        exit(1); }
    
    bzero(&host,sizeof(host));
    host.sin_family=AF_INET;
    host.sin_port=htons(10140);
    
    if ( ( hp = gethostbyname(argv[1]) ) == NULL ) {
        fprintf(stderr,"unknown host %s\n",argv[1]);
        exit(1);
    }
    
    bcopy(hp->h_addr,&host.sin_addr,hp->h_length);
    
    result=connect(sock,(struct sockaddr *)&host,sizeof(host));
    if ((result)==-1){
        perror("cannot \n");
        exit(1);
    }
    else{
        printf("connected\n");
        c2=1;
    }
    
    if(c2==1){
        //s2
        memset(buf, '\0', sizeof(buf));
            if ( ( read(sock,buf,sizeof(buf)) ) < 0) {
                perror("read");
            }
            else {
                printf("receive: %s\n",buf);
                
                if( !strncmp( "REQUEST ACCEPTED\n", buf, strlen("REQUEST ACCEPTED\n"))){
                //    printf("REQUEST ACCEPTED\n");
                    c3=1;
                }
                //  もしメッセージが接続受理 ("REQUEST ACCEPTED\n") ならば状態 c3 へ
                else c6_exception(sock);
                //さもなければ (接続拒否または何らかの例外) 状態 c6 へ移る.
            }
        
        
    }
    
    //c3
    if (c3==1){
        if(write(sock, argv[2], /*sizeof*/strlen(argv[2]))==-1){
            perror("writeforsend\n");
            exit(1);
        }
        
        memset(buf, '\0', sizeof(buf));
        
            if ( ( read(sock,buf,sizeof(buf)) ) < 0) {
                perror("read");
            }
            else {
                printf("receive: %s\n",buf);
                
                if( !strncmp( "USERNAME REGISTERED\n", buf, strlen("REQUEST ACCEPTED\n"))) {
                 //   printf("USERNAME REGISTERED\n");
                    c4=1;
                }
                //  もしメッセージが接続受理 ("USERNAME REGISTERED\n") ならば状態 c4 へ
                else c6_exception(sock);
                //さもなければ (接続拒否または何らかの例外) 状態 c6 へ移る.
            }
    }
    
    
    //c4
    
    if (c4==1){
        int i=1;
        do {
         
            FD_ZERO(&rfds);
            FD_SET(0,&rfds);
            FD_SET(sock,&rfds);
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            memset(buf, '\0', sizeof(buf));
            if(select(sock+1,&rfds,NULL,NULL,&tv)>0 ){
                
                if(FD_ISSET(sock,&rfds)){
                   
                    size_t readlen=read(sock,buf,sizeof(buf));
                    if ( readlen < 0) {
                        perror("read");
                        exit(1);
                    }
                    else if (readlen == 0)  {
                        c5=1;
                        break;
                    }
                    else {
                        size_t length=strlen(buf);
                         if (length > 0 && buf[length - 1] == '\n') {
                             buf[--length] = '\0';
                         }
                        printf("%s",buf);
                        if(i==1)  {
                            printf("==>");
                            i++;
                        }
                        else if(i==2){
                            printf("\n");
                            i=1;
                        }
                        
                        
                    }
                }
                
                if(FD_ISSET(0,&rfds)) {
                    if (fgets(buf, sizeof(buf), stdin) == NULL) {
                        // 標準入力が EOF なら状態 c5 へ
                        c5=1;
                        break;
                    }
                    else{
                        if(write(sock, buf,strlen(buf))==-1){
                            perror("writeforclient\n");
                            exit(1);
                        }
                    }
                }
            }
        } while (1);
    }
    
    //c5
    if(c5==1){
        printf("finished\n");
        close(sock);
        exit(0);
    }
    printf("exit\n");
    
}


void c6_exception(int sock){
    perror("error");
    close(sock);
    exit(1);
}






