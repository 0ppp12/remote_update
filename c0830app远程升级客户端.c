#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define SIZE 10240
int remote_update_s(){
  //1.创建套接字
  int sockfd=socket(AF_INET,SOCK_STREAM,0);
  if(sockfd<0){
    perror("socket");
    return -1;
  }
  //2.连接服务器
  struct sockaddr_in seraddr;
  memset(&seraddr,0,sizeof(seraddr));
  seraddr.sin_family=AF_INET;//地址族
  seraddr.sin_port=htons(6666);//端口号
  seraddr.sin_addr.s_addr=inet_addr("192.168.22.3");//ip地址
  int ret=connect(sockfd,(struct sockaddr*)&seraddr,sizeof(seraddr));
  if(ret<0){
    perror("connect");
    return -1;
  }
  //3.发送数据
  FILE *file0=fopen("./c.txt","r");
  char vbuf[32]={0};//存储服务器上的app版本
  fgets(vbuf,32,file0);
  vbuf[strlen(vbuf)-1]='\0';//把最后一个字符（换行符）用\0替换
  fclose(file0);//关闭文件

  char buffer[64]={0};
  sprintf(buffer,"{version:%s}",vbuf);
  int size=send(sockfd,buffer,sizeof(buffer),0);
  printf("send version:%d\n",size);

  size=recv(sockfd,buffer,sizeof(buffer),0);
  printf("45 recv version:%d\n",size);
  if(size<=0){
    //等待一定时间后继续检查新版本
    sleep(10);
    remote_update_s();
    return 0;
  }
  printf("version:%s\n",buffer);
  sscanf(buffer,"{version:%[^}]}",buffer);
  char cmd[128];
  sprintf(cmd,"echo %s>c.txt",buffer);
  printf("cmd: %s\n",cmd);
  // system(cmd);
  //接收文件
  size_t nsize=0;//接收网络字节序头大小
  uint32_t headersize=0;//保存头大小
  char filename[128]={0};//保存文件名称
  int filesize=0;//保存文件大小
  int recvsize=0;//已接收大小
  FILE *file=NULL;

  while(1){
    //接收头大小数据
    if(headersize==0){//准备工作
      //recv(from, buf, len, 0);buf=htonl(strlen("{filename:%s,filesize:%d}"))
      printf("65 recv header nsize:%ld\n",recv(sockfd,&nsize,sizeof(nsize),0)); //读取头大小
      headersize=ntohl(nsize);//strlen("{filename:%s,filesize:%d}")
      printf("headersize:%u----nsize:%lu\n",headersize,nsize);
      //读取文件头
      char header[headersize+1];
      printf("71 recv header :%ld\n",recv(sockfd,header,headersize,0));
      printf("header:%s\n",header);
      //解析文件名称，文件大小
      sscanf(header,"{filename:%[^,],filesize:%d}",filename,&filesize);
      printf("%s,%d\n",filename,filesize);
      //创建文件
      strcpy(filename,"demo.tmp");//不要在与正在运行的程序文件上写
      file=fopen(filename,"w");
    }

    char buffer[SIZE]={0};//文件内容
    //读取文件内容
    int size=recv(sockfd,buffer,SIZE,0);
    // printf("84 recv content size:%d\n",size);
    fwrite(buffer,1,size,file);
    recvsize+=size;
    // printf("%d----%d\n",recvsize,filesize);
    // printf("%lf\n",(double)recvsize/filesize);
    
    if(recvsize==filesize){//结束
      fclose(file);
      printf("88 recvsize:%d,filesize:%d\n",recvsize,filesize);
      break;
    }
  }
  //重启
  printf("99 echo end-----------\n");
  sleep(1);
  system(cmd);//更新c.txt
  system("mv demo.tmp demo");//写好了再替换正在运行的文件

  char cmd2[128];
  sleep(1);
  sprintf(cmd2,"killall -9 %s","demo");
  printf("%s\n",cmd2);
  sleep(1);
  system(cmd2);
  system("./demo");
  printf("112 --------\n");
  remote_update_s();
  return 0;
}
int main(void){
  remote_update_s();
  return 0;
}