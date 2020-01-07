#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<netdb.h>
#include <netinet/in.h>
#include<sys/stat.h> 
#include<sys/types.h> 
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>

int main (){
    struct sockaddr_in read_sa;
    struct sockaddr_in write_sa;
    struct hostent *host;
    int sockfd,new_sockfd;
    int buf_len,write_len,i;
    char recv_buf[128]={"\0"},send_buf[128];

/*make socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            fprintf(stderr,"socket error\n");
            return 1;
        }

/*socket setting*/
    memset((char*)&read_sa,0,sizeof(read_sa));
    read_sa.sin_family      = AF_INET; // host address type
    read_sa.sin_port        = htons(3001); // port number
    read_sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&read_sa, sizeof(read_sa)) < 0){
        fprintf(stderr,"bind error\n");
            return 1;
    }
    
    if(listen(sockfd,5) < 0 ){
        fprintf(stderr,"listen error\n");
        close(sockfd);
        return 1;
    }
   
    if((new_sockfd = accept(sockfd,(struct sockaddr*)&write_sa,&write_len))== -1) {
        fprintf(stderr,"accept error\n");
        return 1;
    }

    recv(new_sockfd,recv_buf,buf_len,0);

    sprintf(send_buf,"okokokok\n");
    for(i=0;send_buf[i]!='\0';i++);
/*    for(i=0;recv_buf[i]!='\0';i++) {
        send_buf[i]=recv_buf[i]-32;
        printf("input:%c , output:%c\n",recv_buf[i],send_buf[i]);
        }*/

    send(new_sockfd,send_buf,i-1,0);

    //close(sockfd);
    //close(new_sockfd);
}
