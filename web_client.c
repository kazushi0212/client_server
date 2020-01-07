#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h> /* 「注意」参照 */
#include<sys/socket.h>
#include<arpa/inet.h>

void send_input(int sockfd){
    char send_buf[128]={"\0"},recv_buf[128];
    int i,buf_len;
        for(i=0;i<128;i++) send_buf[i]=0;
        buf_len=read(0,send_buf,128);
        for(i=0; send_buf[i]!= '\0'; i++);
        send(sockfd,send_buf,i,0);
        //recv(sockfd,recv_buff, sizeof(recv_buff), 0);
        recv(sockfd,recv_buf, buf_len, 0);
        printf("output word :%s\n",recv_buf);
}

int main (){
    struct sockaddr_in sa;
    struct hostent *hp;
    char buf[1024]={"\0"};
    int sockfd;
    hp = gethostbyname("localhost");

    if (hp==NULL){
        fprintf(stderr,"ホスト取得失敗\n");
        return 1;
    }
    
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            fprintf(stderr,"socket error\n");
            return 1;
        }

        //memset(&sa,0,sizeof(sa));
        sa.sin_family      = AF_INET; // host address type
        sa.sin_port        = htons(3001); // port number
        bzero((char*)&sa.sin_addr,sizeof(sa.sin_addr));
        memcpy((char*)&sa.sin_addr,(char*)hp->h_addr,hp->h_length);

        if(connect(sockfd, (struct sockaddr*)&sa, sizeof(sa)) < 0){
            fprintf(stderr,"connect error\n");
            return 1;
        }
        send_input(sockfd);
        close(sockfd);
}

