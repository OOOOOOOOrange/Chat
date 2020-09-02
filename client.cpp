#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>

#include <iostream>

using namespace::std;

#define BUFFER_SIZE 1024

int main (int argc, const char * argv[])
{
    bool correct=false;
    while(!correct){
        cout<<"欢迎使用简易聊天室，请登录！"<<endl;
        cout<<"用户名:";
        string username,password;
        cin>>username;
        cout<<"密码:";
        cin>>password;
        
        if(username=="a"){
            if(password=="111"){
                correct=true;
            }else{
                cout<<"密码错误！"<<endl;
            }
        }else if(username=="b"){
            if(password=="222"){
                correct=true;
            }else{
                cout<<"密码错误！"<<endl;
            }
        }else{
            cout<<"用户不存在！"<<endl;
        }
    }
    //设置socket
    struct sockaddr_in server_addr;
    server_addr.sin_len = sizeof(struct sockaddr_in);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(11332);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(server_addr.sin_zero),8);

    int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1) {
        perror("socket error");
        return 1;
    }
    char recv_msg[BUFFER_SIZE];//收信
    char input_msg[BUFFER_SIZE];//发信
    //尝试连接
    if (connect(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in))==0) {
        //.创建一个fd_set变量（fd_set实为包含了一个整数数组的结构体），用来存放所有的待检查的文件描述符
       fd_set client_fd_set;
        //等待时间
       struct timeval tv;
       tv.tv_sec = 20;
       tv.tv_usec = 0;


       while (1) {
           //清空fd_set变量，并将需要检查的所有文件描述符加入fd_set
           FD_ZERO(&client_fd_set);
           FD_SET(STDIN_FILENO, &client_fd_set);
           FD_SET(server_sock_fd, &client_fd_set);

           int ret = select(server_sock_fd + 1, &client_fd_set, NULL, NULL, &tv);//调用select。若返回-1，则说明出错;返回0,则说明超时，返回正数，则为发生状态变化的文件描述符的个数
           if (ret < 0 ) {//出错
               continue;
           }else if(ret ==0){//超时
               continue;
           }else{
               //发送消息
               if (FD_ISSET(STDIN_FILENO, &client_fd_set)) {
                   bzero(input_msg, BUFFER_SIZE);
                   fgets(input_msg, BUFFER_SIZE, stdin);
                   std::string s=input_msg;
                   if(s!=""){
                       if (send(server_sock_fd, s.c_str(), BUFFER_SIZE, 0) == -1) {
                           perror("发送消息出错!\n");
                       }
                   }
               }
               //接收消息
               if (FD_ISSET(server_sock_fd, &client_fd_set)) {
                   bzero(recv_msg, BUFFER_SIZE);
                   long byte_num = recv(server_sock_fd,recv_msg,BUFFER_SIZE,0);
                   if (byte_num > 0) {
                       if (byte_num > BUFFER_SIZE) {
                           byte_num = BUFFER_SIZE;
                       }
                       recv_msg[byte_num] = '\0';
                       string str=recv_msg;
                       cout<<str<<endl;
                   }else if(byte_num < 0){
                       printf("接受消息出错!\n");
                   }else{
                       printf("服务器端退出!\n");
                       exit(0);
                   }

               }
           }
       }

   }

   return 0;
}


