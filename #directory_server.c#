#include<stdio.h>
#include<string.h>
#include <stdlib.h>

#include<fcntl.h>
#include<netdb.h>
#include <netinet/in.h>
#include<sys/stat.h> 
#include<sys/types.h> 
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>

#define MAX_LINE_LEN 1024
#define MAX_STR_LEN  69
#define MAX_PROFILES 10000

void parse_line(char *,int);

 struct date {
   int y;
   int m;
   int d;
 };
 
 struct profile {
   int         id;
   char        name[MAX_STR_LEN+1];
   struct date birthday;
   char        home[MAX_STR_LEN+1];
   char        *comment;
 };

struct profile profile_data_store[MAX_PROFILES];
int profile_data_nitems=0; 
int number;
FILE *fp;
struct profile another[MAX_PROFILES];

//%%文字を別の文字に置換
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

//%%文を分割
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

struct date *new_date(struct date *d, char *str)
  {
   char *ptr[3];
 
   if (split(str, ptr, '-', 3) != 3){
     return NULL;
   } else{
   d->y = atoi(ptr[0]);
   d->m = atoi(ptr[1]);
   d->d = atoi(ptr[2]);
   return d;
   }
 }

struct profile *new_profile(struct profile *p, char *csv,int fd)
 {
     char *qtr[5]; 
     if (split(csv, qtr, ',', 5) != 5) {
         return NULL; 
     }
     p->id = atoi(qtr[0]); 
   
     strncpy(p->name, qtr[1], MAX_STR_LEN); 
     p->name[MAX_STR_LEN] = '\0';

     if (new_date(&p->birthday, qtr[2]) == NULL) {
         return NULL; 
     }
     strncpy(p->home, qtr[3], MAX_STR_LEN);
     p->home[MAX_STR_LEN] = '\0';
     
     p->comment = (char *)malloc(sizeof(char) * (strlen(qtr[4])+1));
     strcpy(p->comment, qtr[4]); 
     return p;  
 }

////%%コマンド処理
////コマンドC
void cmd_check(fd)
{
    char buf[MAX_LINE_LEN];
    sprintf(buf,"%d profile(s)\n",profile_data_nitems);
    send(fd,buf,1024,0);
}


char *date_string(char buf[],struct date *date)
{
  sprintf(buf,"%04d-%02d-%02d",date->y,date->m,date->d);
  return buf;
}


void print_profile(struct profile *p,int fd)
{
    char date[20];
    char print_buf[1024];
    int i;
    sprintf(print_buf,"ID    : %d\n""Name  : %s\n""Birth : %s\n""Addr  : %s\n""Com.  : %s\n", p->id, p->name, date_string(date, &p->birthday), p->home, p->comment);
    //for(i=0; print_buf[i]!='\0'; i++);
    //printf("%s",print_buf);
    send(fd,print_buf,1024,0); 
}

////コマンドP
void cmd_print(int nitems, int fd)
{
    int i, start = 0,end = profile_data_nitems;
    char num[1024];
    char str[1024];

    if (nitems > 0) {
        if(nitems < profile_data_nitems) end = nitems;
        sprintf(num,"%d",nitems);
        send(fd,num,1024,0);
    }
    else if (nitems < 0) {
        if(end + nitems > start) start = end + nitems ;
        sprintf(num,"%d",-nitems);
        send(fd,num,1024,0);      
    } else {
        sprintf(num,"%d",profile_data_nitems);   //全件表示
        send(fd,num,1024,0); 
    }

    for (i = start; i < end; i++) {
        recv(fd,str,1024,0);
        print_profile(&profile_data_store[i],fd);
    }
}

/*
//コマンドR
void cmd_read(char *filename,int fd)
{
  int l;
  char line[MAX_LINE_LEN + 1];

  if((fp = fopen(filename,"r")) == NULL){
    fprintf(stderr, "%%R: file open error %s.\n", filename);
    return;
    }

//////%D用///////
   number=profile_data_nitems;
  for(l=0; l<=number-1; l++){
    another[l] = profile_data_store[l];
  }
  while(get_line(fp,line)){
      parse_line(line,fd);
  }
  fclose(fp);
}
*/

////コマンドC
void fprint_profile_csv(int fd,struct profile *p)
{
  char buf[1024];
  char date[20];
  sprintf(buf,"%d,%s,%s,%s,%s",p->id,p->name,date_string(date,&p->birthday),p->home,p->comment);
  send(fd,buf,1024,0);
}

/////コマンドW
void cmd_write(char *filename,int fd)
{
  int i,l;
  char num[10],str[10];
  if((fp = fopen(filename,"w")) == NULL){
      fprintf(stderr,"file error");
    }
  sprintf(num,"%d",profile_data_nitems);
  send(fd,num,10,0); 

  number=profile_data_nitems;
  for(i=0; i<=number-1; i++){
    another[i] = profile_data_store[i];
  }
  
  for (i = 0; i < profile_data_nitems; i++) {
      recv(fd,str,10,0);
      fprint_profile_csv(fd,&profile_data_store[i]);
  }
  fclose(fp);
}


int strcmp_word(struct profile *p,char *word)
{
  char id[20];
  char date[20];
 sprintf(id,"%d",p->id);
 if(strcmp(id, word) == 0||strcmp(p->name, word) == 0||strcmp(date_string(date, &p->birthday), word) == 0||strcmp(p->home, word) == 0||strcmp(p->comment, word) == 0){
   return 0;
 } 
}

////コマンドF
void cmd_find(char *word,int fd)
{
    int i,times=0;
    struct profile *p,*q;
    char num[10],buf[1024];
    char str[10];

    memset(buf,0,MAX_LINE_LEN);

    for (i=0; i < profile_data_nitems; i++) {
        p=&profile_data_store[i];
        if(strcmp_word(p,word)==0){ 
            times++;
        }
    }
    sprintf(num,"%d",times);
    send(fd,num,10,0);
    if(times>0){
        for (i=0; i < profile_data_nitems; i++) {
            q=&profile_data_store[i];
            if(strcmp_word(q,word)==0){
                recv(fd,str,10,0);
                print_profile(q,fd);
            }
        }
    }else{                    //times==0 : %F の引数を含むデータがないとき
        recv(fd,str,10,0);
        sprintf(buf,"no data with that phrase!\n");
        send(fd,buf,1024,0); 
    }
}


void swap(struct profile *p1, struct profile *p2)
{
  struct profile tmp;

  tmp = *p1;
  *p1 = *p2;
  *p2 = tmp;
}

int compare_date(struct date *d1, struct date *d2)
{
  if (d1->y != d2->y) return (d1->y) - (d2->y);
  if (d1->m != d2->m) return (d1->m) - (d2->m);
  return (d1->d) - (d2->d);
}

//引数でソートを場合分け
int compare_profile(struct profile *p1, struct profile *p2, int column)
{
  switch (column) {
    case 1:
      return p1->id - p2->id; break; 
    case 2:
      return strcmp(p1->name,p2->name); break;  
    case 3:
      return compare_date(&p1->birthday,&p2->birthday); break;  
    case 4:
      return strcmp(p1->home,p2->home); break;  
    case 5:
      return strcmp(p1->comment,p2->comment); break; 
    }
}

////コマンドS
void cmd_sort(int column,int fd)                   //エラー処理不十分
{
  int length =profile_data_nitems;
  int i,j,l,s;
  struct profile *p;
  char buf[1024],num[10];
  memset(buf,0,MAX_LINE_LEN);
  s = length-1;

  if(column != 1 && column != 2 && column != 3 && column != 4 && column !=5){
      sprintf(buf,"%d is not adaptted\n",column);
      send(fd,buf,1024,0);
  } 
  if(column == 1 || column == 2 || column == 3 || column == 4 || column ==5){
      number=profile_data_nitems;
      for(l=0; l<=number-1; l++){
          another[l] = profile_data_store[l];           //コマンドD用
      }
      for(i = 0; i <= s; i++) {
          for (j = 0; j <= s - 1; j++) {
              p=&profile_data_store[j];
              if (compare_profile(p, (p+1), column) > 0)
                  swap(p, (p+1));  
          }
      }
      sprintf(buf,"%%S was executed\n");
      send(fd,buf,1024,0);
  }
}

////コマンドD
void cmd_delete(int nitems,int fd)
{
  int i, l, end = profile_data_nitems-1;
  char buf[1024];
  memset(buf,1024,0);
  if(nitems < 0){
      sprintf(buf,"%d is smaller than 0\n",nitems);
      send(fd,buf,1024,0); 
  } 
  else if(nitems > end+1){
      sprintf(buf,"%d is bigger than data number\n",nitems);
      send(fd,buf,1024,0); 
  }
  else{
      number=profile_data_nitems;
      for(l=0; l<=number-1; l++){               
          another[l] = profile_data_store[l];         //コマンドD用
      }
      if(nitems > 0 && nitems < end+1){
          for(i=nitems-1;i<end;i++){
              profile_data_store[i]=profile_data_store[i+1];  
          }
          profile_data_nitems--;
      }
      else if(nitems == end+1){
          profile_data_nitems--;
      }
      else if(nitems == 0){         //ALL DELETE
          profile_data_nitems=0;
      }
      sprintf(buf,"%%D was executed\n");
      send(fd,buf,1024,0);
  }
}

////コマンドV
void cmd_vanish(char *word,int fd)
{
    int i,k,l,times=0;
    char date[20];
    char id[20];
    char buf[1024];
    memset(buf,1024,0);
    struct profile *p;
    for (i=0; i < profile_data_nitems; i++) {
        p=&profile_data_store[i];
        if(strcmp_word(p,word)==0){
            times++;
        }
    }
    if(times>0){
        number=profile_data_nitems;
        for(l=0; l<=number-1; l++){
            another[l] = profile_data_store[l];             //コマンドD用
        }
        for (i=0; i < profile_data_nitems; i++) {
            p=&profile_data_store[i];
            if(strcmp_word(p,word)==0){
                for(k=i;k<profile_data_nitems-1;k++){
                    profile_data_store[k]=profile_data_store[k+1];  
                }
                profile_data_nitems--;
            }
        }
        sprintf(buf,"%%V was executed\n");
        send(fd,buf,1024,0);
    } else {
        sprintf(buf,"no data with that phrase!\n");
        send(fd,buf,1024,0);
    }
}


void cmd_help(int fd)
{
    char help_buf[1024]={"\0"};
    int i;
    char *str1  = "%%Q      : プログラムを終了\n";   
    char *str2  = "%%C      : CSVデータの登録件数などを表示 \n";
    char *str3  = "%%P n    : CSVデータの先頭からn件表示 (n: 0 - 99 ... n = 0，n > 100:全件表示，n < 0:後ろから-n件表示)\n";
    char *str4  = "%%R file : fileから読み込み　\n";
    char *str5  = "%%W file : fileへ書き出し \n";
    char *str6  = "%%F word : wordを含むデータを検索\n";     
    char *str7  = "%%S n    : データをn番目の項目で整列\n";   
    char *str8  = "%%D n    : n番目のデータを削除 (n: 0 - 99 ... n=0:全件削除，n < 0，n > 100:警告文の表示)\n"; 
    char *str9  = "%%V word :  wordを含むデータを削除\n";
    char *str10 ="%%B      :  データ群の形を1つ前の状態に戻す\n";
    sprintf(help_buf,"%s%s%s%s%s%s%s%s%s%s\n",str1,str2,str3,str4,str5,str6,str7,str8,str9,str10);
   
    for(i=0; help_buf[i]!='\0'; i++);
    send(fd,help_buf,i,0);
}


void cmd_back(int fd){
    char buf[1024];
  int i,s=profile_data_nitems;
  profile_data_nitems=number;
  for(i=0; i<=profile_data_nitems-1; i++){
    profile_data_store[i] = another[i];
  }
        sprintf(buf,"%B was executed\n");
        send(fd,buf,1024,0);
}


int check1(char *param){
if((*param>='a'&& *param<='z') || (*param>='A' && *param<='Z') || (*param>='0' && *param<='9')) {
  return 1;
 }
}

int check2(char *param){
if(*param>='0' && *param<='9') {
  return 1;
 }
}

int check3(char *param){
    int l,i,j=0;
    for(l=0;param[l]!='\0';l++);

    for(i=0;i<l;i++){
        if((param[i]>='a'&& param[i]<='z') || (param[i]>='A' && param[i]<='Z')){
            j++;
        }
    }
    return j;
}

/*
int check3(char *param){
  if((*param>='a'&& *param<='z') || (*param>='A' && *param<='Z')) {
    return 1;
  }
}
*/
void exec_command(char cmd, char *param, int fd)
{
    char buf[1024];
    memset(buf,0,MAX_LINE_LEN);
    switch (cmd){
    case 'C': 
        if(check1(param)==1) {
            sprintf(buf,"Don't write word\n");
            send(fd,buf,1024,0);
            break;
        }else{ 
            cmd_check(fd); 
            break;
        }
    case 'P': 
        if(check3(param)!=0) {
            sprintf(buf,"miss");
            send(fd,buf,1024,0);
            break;
        }else{
            cmd_print(atoi(param),fd);
            break;
        }
    case 'W': cmd_write(param,fd);       break;
    case 'F': cmd_find(param,fd);     break;
    case 'S': 
        if(check3(param)!=0) {
            sprintf(buf,"Please write number\n");
            send(fd,buf,1024,0);
            break;
        }else{
        cmd_sort(atoi(param),fd);  break;
        }
    case 'D': 
        if(check3(param)!=0) {
            sprintf(buf,"Please write number\n");
            send(fd,buf,1024,0);
            break;
        }else{
            cmd_delete(atoi(param),fd);
            break;
        }
    case 'V': cmd_vanish(param,fd);     break;
        
    case 'H': 
        if(check1(param)==1) {
            sprintf(buf,"Don't write word\n");
            send(fd,buf,1024,0);
            break;
        }else{ 
            cmd_help(fd); 
            break;
        }
    case 'B':
        if(check1(param)==1) {
            sprintf(buf,"Don't write word\n");
            send(fd,buf,1024,0);
            break;
        }else{ 
            cmd_back(fd); 
            break;
        }
    default:
        sprintf(buf,"command %c is ignored.\n",cmd);
        send(fd,buf,1024,0);
        break;
    }
}

////%%parse_line
void parse_line(char *line,int fd)
{
    int x=0;
    char buf[3];
    memset(buf,0,3);
    if(line[0] == '%'){
        exec_command(line[1],&line[3],fd);
    }
    else{
/*
        if(new_profile(&profile_data_store[profile_data_nitems], line,fd)==NULL){
            send(fd,"error",10,0);
        } else{
*/
        new_profile(&profile_data_store[profile_data_nitems], line,fd);
        send(fd,"ok",3,0);
        profile_data_nitems++;
        //}
    }
}

int main (){
    struct sockaddr_in read_sa;
    struct sockaddr_in write_sa;
    struct hostent *host;
    int sockfd,new_sockfd;
    int buf_len,write_len,i;
    char recv_buf[1024],send_buf[1024];
    int x=1;
    memset(recv_buf,0,MAX_LINE_LEN);
    memset(send_buf,0,MAX_LINE_LEN);

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
   
     while(1){
        if((new_sockfd = accept(sockfd,(struct sockaddr*)&write_sa,&write_len))== -1) {
            fprintf(stderr,"accept error\n");
            return 1;
        }
        while(1){
            recv(new_sockfd,recv_buf,1024,0);

            //コマンドQの処理
            if(recv_buf[0]=='%' && recv_buf[1]=='Q'){
                sprintf(send_buf,"exit!");
                send(new_sockfd,send_buf,1024,0);
                close(new_sockfd);
                break;
            }
            parse_line(recv_buf,new_sockfd);  
        }
     }
}
