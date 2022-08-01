#ifndef _CLIENT_H_
#define _CLIENT_H_

#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdbool.h>
#include<pthread.h>
#include<time.h>



#define TRUE    1
#define FALSE  -1
#define N      20

#define ERRLOG(errmsg) do{\
                            perror(errmsg);\
                            printf("%s - %s - %d\n",__FILE__,__func__,__LINE__);\
                            exit(1);\
                         }while(0)


//客户端发送的信息
struct USR{             //在线人员的信息
    char name[N];
    int socket;
    int flag;    //用来禁言标志位  0---没有被禁言，1---被禁言
};

//一条群聊记录
struct gp_record{
    char tim[30];      //时间戳
    char fronname[18];   //发送者姓名
    char word[1024];      //发送内容
};

//一条私聊记录
struct pr_record{
    char tim[30];      //时间戳
    char fronname[18];   //发送者姓名
    char toname[18];
    char word[1024];      //发送内容
};


//客户端接收的消息
struct Msg
{
    struct USR usr[N];
    char msg[1024];
    char buf[1024];
    char name[N];
    char fronname[N];
    char toname[N];
    char passwd[N];
    int cmd;
    int filesize;
    int flags;                      //用来判断用户权限 0--代表普通用户， 1---代表超级用户
};

int offline;     //被踢标志位---1---被踢，0----没有被踢


//套接字初始化
void Socket_init(int *arg);

//聊天室
void Chat_Room(int sockfd);

//聊天室开头封面
int Show_cover();

//注册界面
void Register_menu(int sockfd,struct Msg *msg);

//登录界面
void Log_menu(int sockfd,struct Msg *msg);

//密码隐式输入
void ImplicitCode(struct Msg *msg);

//用户注册
void Reg_Usr(int sockfd);

//用户登录
void Log_Usr(int sockfd);


//判断用户是否为管理员
void UsrIsRoot(int sockfd,struct Msg *msg);

//管理员
void Root_Usr(int sockfd,struct Msg *msg);

//普通用户
void Common_Usr(int sockfd,struct Msg *msg);

//普通用户菜单
int Common_menu(struct Msg *msg);

//管理员菜单
int Root_menu(struct Msg *msg);

//查看在线人数
void see_online(int sockfd,struct Msg *msg);

//群聊
void chat_group(int sockfd,struct Msg *msg);

//私聊
void chat_private(int sockfd,struct Msg *msg);

//查看聊天记录
void see_record(struct Msg *msg);

//删除聊天记录
void dele_record(struct Msg *msg);

//查看私聊记录
void see_privaterecord(struct Msg *msg);

//查看群聊记录
void see_grouprecord(struct Msg *msg);

//删除私聊记录
void dele_privaterecord(struct Msg *msg);

//注销当前用户
void logout(int sockfd,struct Msg *msg);

//退出客户端
void off_line(int sockfd,struct Msg *msg);

//禁言
void forbid_speak(int sockfd,struct Msg *msg);

//解除禁言
void releve_speak(int sockfd,struct Msg *msg);

//踢出群聊
void kickout_group(int sockfd,struct Msg *msg);

//发送文件
void send_file(int sockfd,struct Msg *msg);

//文件下载
void download_file(int sockfd,struct Msg *msg);

//计算文件大小
int file_size(char *s);

//删除群聊记录
void dele_grouprecord(struct Msg *msg);

//保存聊天记录
void save_record(int flag2,struct Msg *msg);

//保存群聊聊天记录在本地文件
void group_record(char *s,struct Msg *msg);

//保存发送的私聊记录在本地文件
void private_Srecord(char *s,struct Msg *msg);

//保存接收的私聊记录在本地文件
void private_Rrecord(char *s,struct Msg *msg);


//线程函数
void *RecvMsg(void *arg);

//展示在线人数
void dispy_online(struct Msg *msg);

//下载文件
void fun(int sockfd,struct Msg *msg);


#endif

