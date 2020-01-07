#include<stdio.h>
#include<string.h>
#include <stdlib.h>

#include<netdb.h>
#include<sys/types.h> /* 「注意」参照 */
#include<sys/socket.h>
#include<arpa/inet.h>
#include <fcntl.h>
#include<unistd.h>

#define MAX_LINE_LEN 1024
FILE *fp;
struct hostent *hp;
struct sockaddr_in sa;

int sockfd;
void parse_line(char *);

int check1(char *param){
    if((*param>='a'&& *param<='z') || (*param>='A' && *param<='Z') || 
       (*param>='0' && *param<='9')) {
        return 1;
    }
}

 int subst(char *str, char c1, char c2)
 {
   int n = 0;
 
   while (*str) {
     if (*str == c1) {
       *str = c2;
       n++;
    }
     str++;
   }
   return n;
 }

 int split(char *str, char *ret[], char sep, int max)
 {
   int n = 0;
   ret[n++] = str; 
   while (*str && n < max) {
     if (*str == sep){
      *str = '\0';
      if(*(str+1) != sep){
       ret[n++] = str + 1;
      }
     }
     str++;
   }
  return n;
 }

int get_line(FILE *fp,char *line)
 {
   if (fgets(line, MAX_LINE_LEN + 1, fp) == NULL){
     return 0;
 } else{
   subst(line, '\n', '\0');
   return 1;
   }
 }

//コマンドQ
void cmd_quit(char *line)
{
    char buf[1024];
    send(sockfd,line,MAX_LINE_LEN,0);
    recv(sockfd,buf,MAX_LINE_LEN,0);
    close(sockfd);
    exit(0);
}

//コマンドP
void cmd_print(char *line){
    int i,nitems;
    char num[1024],buf[MAX_LINE_LEN];
    memset(buf,0,MAX_LINE_LEN);
    memset(num,0,1024);

    send(sockfd,line,MAX_LINE_LEN,0);
    recv(sockfd,num,1024,0);

    if(strcmp(num,"miss")==0){
        printf("Please write number\n");
    }else{
        nitems=atoi(num);
        
        for(i=0;i<nitems;i++){
            send(sockfd,"do",10,0);
            recv(sockfd,buf,MAX_LINE_LEN,0);
            printf("%s",buf);
        }
    }
}

//コマンドR
void cmd_read(char *filename){
    char read_buf[MAX_LINE_LEN],recv_buf[MAX_LINE_LEN];
    int i,buf_len,x=1;

    if((fp = fopen(filename,"r")) == NULL) {
        fprintf(stderr, "%%R: file open error %s.\n", filename);
        return;
    }

    memset(read_buf,0,MAX_LINE_LEN);
    while(fgets(read_buf,MAX_LINE_LEN,fp) != NULL){
        //printf("%s",read_buf);  //確認用
        send(sockfd,read_buf,MAX_LINE_LEN,0);
        recv(sockfd,recv_buf,MAX_LINE_LEN,0);
        memset(read_buf,0,MAX_LINE_LEN);
    }
   
    fclose(fp);
}

//コマンドW
void cmd_write(char *line,char *filename){
    int i,nitems;
    char num[10],buf[1024];

     if((fp = fopen(filename,"w")) == NULL){
      fprintf(stderr,"error");
      return ;
    }

     memset(buf,0,MAX_LINE_LEN);
     memset(num,0,10);

     send(sockfd,line,MAX_LINE_LEN,0);
     recv(sockfd,num,10,0);
     nitems=atoi(num);

     for (i = 0; i < nitems; i++) {
         send(sockfd,"do",10,0);
         recv(sockfd,buf,MAX_LINE_LEN,0);
         fprintf(fp,"%s",buf);
  }
     fclose(fp);
}

//コマンドF
void cmd_find(char *line,char *param){
    char recv_buf[MAX_LINE_LEN],num[10];
    int i,k,times;
    memset(recv_buf,0,MAX_LINE_LEN);

    for(i=0; line[i]!= '\0'; i++);
    send(sockfd,line,i,0);
    recv(sockfd,num,10,0);
    times=atoi(num);
    if(times==0) {
        send(sockfd,"miss",10,0);
        recv(sockfd,recv_buf,1024,0);
        printf("%s",recv_buf);
    }
    for(i=0;i<times;i++){
        send(sockfd,"do",10,0);
        recv(sockfd,recv_buf,1024,0);
        printf("%s",recv_buf);
    }
}

//コマンド　デフォルト(%S,%D,%V,%H)
void cmd_default(char *line){
    char recv_buf[MAX_LINE_LEN];
    int buf_len,i;
    memset(recv_buf,0,MAX_LINE_LEN);
    //for(i=0; line[i]!= '\0'; i++);
    send(sockfd,line,1024,0);
    recv(sockfd,recv_buf,1024,0);
    if(line[0]=='%'){
        printf("%s",recv_buf);
    }
}

void exec_command(char cmd, char *param,char *line){
    int i,buf_len;
    char buf[1024];

    switch (cmd){
    case 'Q': cmd_quit(line);                   break;
    case 'P': cmd_print(line);              break;
    case 'R': cmd_read(param);              break;
    case 'W': cmd_write(line,param);        break;
    case 'F': cmd_find(line,param);         break;
    default:
        cmd_default(line);                  break;
    }
}

void parse_line(char *line)
{
    int i,buf_len;
    char buf[MAX_LINE_LEN];
    if(line[0] == '%'){
        if(check1(&line[2])==1) {
            printf("Please space after %c%c\n",line[0],line[1]);
        }
        else {
            exec_command(line[1],&line[3],line);
        }
    }
    else{
        cmd_default(line);
    }
}

void send_input(){
    char buf[MAX_LINE_LEN];

    int i;
    memset(buf,0,MAX_LINE_LEN);
    read(0,buf,MAX_LINE_LEN);
    //printf("%s",buf);      //入力確認
    subst(buf, '\n', '\0');
    parse_line(buf);
}

int make_sockfd(){
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr,"socket error\n");
        return -1;
    }

    //memset(&sa,0,sizeof(sa));
    sa.sin_family      = AF_INET; // host address type
    sa.sin_port        = htons(3001); // port number
    bzero((char*)&sa.sin_addr,sizeof(sa.sin_addr));
    memcpy((char*)&sa.sin_addr,(char*)hp->h_addr,hp->h_length);
    
    if(connect(fd, (struct sockaddr*)&sa, sizeof(sa)) < 0){
        fprintf(stderr,"connect error\n");
        return -1;
    }
    return fd; 
}

int main (){
    hp = gethostbyname("localhost");

    if (hp==NULL){
        fprintf(stderr,"ホスト取得失敗\n");
        return 1;
    }

    if((sockfd=make_sockfd()) < 0) return;
           
    while(1){
        send_input();
    }
}

