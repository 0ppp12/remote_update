#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#define SIZE 1024*10
void *client_run(void *arg);
int main(void){
    //1.创建套接字
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        perror("socket");
        return -1;
    }
    //2.绑定
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(6666);
    addr.sin_addr.s_addr=INADDR_ANY;
    int ret=bind(sockfd,(struct sockaddr*)(&addr),sizeof(addr));
    if(ret<0){
        perror("bind");
        return -1;
    }
    //3.监听
    ret=listen(sockfd,5);
    if(ret<0){
        perror("listen");
        return -1;
    }
    while(1){
        //4.接受连接（默认是阻塞）
        struct sockaddr_in clientaddr;
        socklen_t len=sizeof(clientaddr);
        int clientfd=accept(sockfd,(struct sockaddr *)(&clientaddr),&len);
        if(clientfd<0){
            perror("accept");
            break;
        }
        printf("客户端IP:%s\n",inet_ntoa(clientaddr.sin_addr));
        //遍历--把ip地址和套接字存储到客户端信息数组中
        //启动线程
        pthread_t id=0;
        int ret=pthread_create(&id,NULL,client_run,(void*)&clientfd);
        if(ret<0){
            close(clientfd);
        }
        //分离线程
        pthread_detach(id);
    }
    pause();
    close(sockfd);
    return 0;
}
void *client_run(void *arg){
    int clientfd=*((int*)arg);
    //接收数据
    char recvbuffer[256]={0}; //{version:v1.0}
    int ret=recv(clientfd,recvbuffer,256,0);//接收客户端版本号
    printf("recv version:%d\n",ret);
    if(ret<=0){
        perror("read/客户端掉线");
    }
    char vbuf[32]={0};
    //解析版本号
    sscanf(recvbuffer,"{version:%[^}]}",vbuf);
    //与服务器存储的版本号比较
    FILE *file=fopen("./s.txt","r");
    char svbuf[32]={0};//存储服务器上的app版本
    fgets(svbuf,32,file);
    svbuf[strlen(svbuf)-1]='\0';//把最后一个字符（换行符）用\0替换
    fclose(file);//关闭文件
    //比较相同
    printf("%s--%s----\n",vbuf,svbuf);
    if(strcmp(vbuf,svbuf)==0){
        close(clientfd);
        return NULL;
    }
    //发送新的版本号
    char sendbuf[64]={0};
    sprintf(sendbuf,"{version:%s}",svbuf);//
    printf("87 send version:%ld\n",send(clientfd,sendbuf,strlen(sendbuf),0));//对方还要recv version
    //发送文件

        //计算文件大小
    unsigned  int filesize=0;
    char aout[64]="demo";
    FILE *file2=fopen(aout,"r");
    if(file2==NULL){
        perror("fopen");
        return (void*)-1;
    }
    fseek(file2,0,SEEK_END);//偏移
    filesize=ftell(file2);//获取偏移量
    rewind(file2);//偏移到文件头

    //打包文件头（文件名称和文件大小）{filename:main.c,filesize:12345}
    char header[128]={0};
    sprintf(header,"{filename:%s,filesize:%d}",aout,filesize);
    printf("header:%s\n",header);
    
    //3.发送头大小
    uint32_t headersize=strlen(header);
    unsigned nsize=htonl(headersize);
    printf("headersize:%d----nsize:%lu\n",headersize,nsize);
    
    printf("111 send header nsize:%ld\n",send(clientfd,&nsize,sizeof(nsize),0));//!
    //4.发送头信息
    printf("115 send header:%ld\n",send(clientfd,header,strlen(header),0));
    //5.发送文件内容
    while(1){
        char buffer[SIZE]={0};
        //读文件
        int readsize=fread(buffer,1,SIZE,file);
        if(readsize==0){
            printf("发送完毕---------\n");
            break;
        }
        //发送文件内容
        send(clientfd,buffer,readsize,0);
        // printf("126 send content:%ld\n",send(clientfd,buffer,readsize,0));
    }
}
