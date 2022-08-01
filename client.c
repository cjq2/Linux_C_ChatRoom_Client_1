#include "client.h"

//套接字初始化
void Socket_init(int *arg)
{
    int sockfd; //分别创建套接字文件描述符变量
    struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);
    //第一步：创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        ERRLOG("socket");
        return;
    }

    //第二步：填充网络信息结构体
    serveraddr.sin_addr.s_addr = inet_addr("192.168.91.78");
    serveraddr.sin_family = AF_INET; // IPV4协议族
    serveraddr.sin_port = htons(atoi("8866"));

    //第三步：给服务器发送客户端连接请求
    if (connect(sockfd, (struct sockaddr *)&serveraddr, addrlen))
    {
        ERRLOG("connect");
        return;
    }
    printf("客户端连接上服务器了\n");
    sleep(2);
    *arg = sockfd;
}

//聊天室
void Chat_Room(int sockfd)
{
    while (1)
    {

        int num = Show_cover();
        switch (num)
        {
        case 1:
            Reg_Usr(sockfd);
            break;
        case 2:
            Log_Usr(sockfd);
            break;
        case 3:
            system("clear");
            return;
        }
    }
}

//聊天室开头封面
int Show_cover()
{
    system("clear");
    printf("**************************************************************************\n");
    printf("\n\n\n\n\n");
    printf("                               欢迎使用聊天室                             \n");
    printf("\n\n\n\n\n");
    printf("**************************************************************************\n");
    printf("\n\n\n\n\n");
    printf("                                1、注册账号\n");
    putchar(10);
    printf("                                2、登录账号\n");
    putchar(10);
    printf("                                3.退出\n");
    printf("\n\n\n\n\n");
    printf("**************************************************************************\n");
    printf("\n\n");
    int num, status = 0;
    printf("请选择选项：\n");
    status = scanf("%d", &num);
    while (status != 1 && num < 1 || num > 3)
    {
        if (status != 1)
        {
            scanf("%*s");
            printf("请输入1-3的整数:\n");
        }
        status = scanf("%d", &num);
    }
    return num;
}

//注册界面
void Register_menu(int sockfd,struct Msg *msg)
{
    system("clear");
    printf("**************************************************************************\n");
    printf("\n\n\n");
    printf("******************************注册新用户**********************************\n");
    printf("\n\n\n");
    printf("**************************************************************************\n");
    printf("\n\n\n\n\n");
    printf("                                1、用户名\n");
    putchar(10);
    printf("                                2、登录密码\n");
    printf("\n\n\n\n\n");
    printf("**************************************************************************\n");
    printf("请设置用户名\n");
    scanf("%s", msg->name);
    getchar();                    //处理垃圾字符
    printf("请设置登录密码\n");
    ImplicitCode(msg);
    msg->cmd = 1;
    sleep(1);
}

//登录界面
void Log_menu(int sockfd,struct Msg *msg)
{
    system("clear"); 
    printf("**************************************************************************\n");
    printf("\n\n\n");
    printf("******************************用户登录************************************\n");
    printf("\n\n\n");
    printf("**************************************************************************\n");
    printf("\n\n\n\n\n");
    printf("                                1、用户名\n");
    putchar(10);
    printf("                                2、登录密码\n");
    printf("\n\n\n\n\n");
    printf("**************************************************************************\n");
    printf("请输入用户名\n");
    scanf("%s", msg->name);
    getchar();                //处理垃圾字符
    printf("请输入登录密码\n");
    ImplicitCode(msg);
    msg->cmd = 2;
}

void ImplicitCode(struct Msg *msg)
{
    int i=0;
    char ch=0;
    while(1)
    {
        system("stty -echo");
        ch=getchar();  //获取一个字符
        system("stty echo");
        if(ch=='\n')   //遇到换行符，就结束
        {
            break;
        }
        else if(ch=='\b')  //遇到退格
        {
            printf("\b \b");
            --i;
        }
        else
        {
            msg->passwd[i++]=ch;
        }
    }
    putchar(10);
    msg->passwd[i]='\0';  
}

//用户注册
void Reg_Usr(int sockfd)
{
    struct Msg msg;
    memset(&msg, 0, sizeof(struct Msg));
    Register_menu(sockfd,&msg);
    send(sockfd, &msg, sizeof(struct Msg), 0);              //向服务器发送注册信息
    recv(sockfd, &msg, sizeof(struct Msg), 0); //接收服务器的注册反馈
    if (msg.cmd == 0)
    {
        printf("注册失败,该用户已注册\n");
        sleep(2);
        return;
    }
    if (msg.cmd == 1)
    {
        printf("注册成功\n");
        sleep(2);
        return;
    }
}

//用户登录
void Log_Usr(int sockfd)
{
    struct Msg msg;
    Log_menu(sockfd,&msg); 
    send(sockfd, &msg, sizeof(struct Msg),0);                         //向服务器发送登录信息
    printf("等待服务器发送消息......\n");
    recv(sockfd, &msg, sizeof(struct Msg), 0); //接收服务器的登录反馈
    switch (msg.cmd)
    {
        case 0:
            UsrIsRoot(sockfd, &msg);
            break;
        case 1:
            printf("用户密码输入错误\n");
            sleep(2);
            break;
        case 2:
            printf("该用户还未注册\n");
            sleep(2);
            return;
        case 3:
            printf("该用户已在线\n");
            sleep(2);
            break;
    }
    
}

//判断用户是否为管理员
void UsrIsRoot(int sockfd, struct Msg *msg)
{
    //offline = 0;   //未被踢出
    pthread_t tid;
    pthread_create(&tid, NULL, RecvMsg, &sockfd); //创建一个线程来读取收到的信息
    pthread_detach(tid);
    switch (msg->flags)
    {
        case 0:
            Common_Usr(sockfd,msg);
            break;
        case 1:
            Root_Usr(sockfd, msg);
            break;
    }
}

void Root_Usr(int sockfd,struct Msg *msg)
{
    int num;
    while(1)
    {
        num=Root_menu(msg);
        switch(num)
        {
            case 1:
                    see_online(sockfd,msg);  //查看在线人数
                    break;
            case 2:
                    chat_group(sockfd,msg);   //群聊
                    break;
            case 3:
                    chat_private(sockfd,msg);   //私聊
                    break;
            case 4:
                    see_record(msg);   //查看聊天记录
                    break;
            case 5:
                    dele_record(msg);   //删除聊天记录
                    break;       
            case 6:
                    forbid_speak(sockfd,msg);    //禁言
                    break;
            case 7:
                    releve_speak(sockfd,msg);   //解除禁言
                    break;
            case 8:
                    off_line(sockfd,msg);         //下线
                    exit(0);
                    return;
            case 9:
                    kickout_group(sockfd,msg);         //踢出聊天室
                    break;
        }
    }
}


void Common_Usr(int sockfd,struct Msg *msg)
{
    int num;
    while(1)
    {
         
        num=Common_menu(msg);
        if(offline==1)
        {
            printf("你已被踢出聊天室\n");
            sleep(4);
            return;
        }
        switch(num)
        {
           case 1:
                    see_online(sockfd,msg);   //查看在线人数
                    break;
            case 2:
                    chat_group(sockfd,msg);    //群聊
                    break;
            case 3:
                    chat_private(sockfd,msg);    //私聊
                    break;        
            case 4:
                    see_record(msg);       //查看聊天记录
                    break;
            case 5:
                    dele_record(msg);       //删除聊天记录
                    break;
            case 6:
                    send_file(sockfd,msg);       //发送文件
                    break;
            case 7:
                    logout(sockfd,msg);           //注销用户
                    exit(0);
                    break;;
            case 8:
                    off_line(sockfd,msg);         //下线
                    exit(0);
                    break;
            case 9:
                    download_file(sockfd,msg);     //下载文件
                    break;
            default :
                    printf("请输入1-9的整数:\n");
                    break;
        }

       
    }

}

//查看在线人数
void see_online(int sockfd,struct Msg *msg)
{
    msg->cmd=1;
    send(sockfd,msg,sizeof(struct Msg),0);
    return ;
}

//群聊
void chat_group(int sockfd,struct Msg *msg)
{
    msg->cmd=2;
    printf("请输入要发送的内容:\n");
    fgets(msg->msg,1024,stdin);
    msg->msg[strlen(msg->msg)-1]='\0';
    strcpy(msg->fronname,msg->name);
    strcpy(msg->toname,"all");
    save_record(0,msg);         //保存群聊聊天记录
    send(sockfd,msg,sizeof(struct Msg),0);
}

//私聊
void chat_private(int sockfd,struct Msg *msg)
{
    msg->cmd=3;

    printf("请输入私聊的用户名：\n");
    fgets(msg->toname,sizeof(msg->toname),stdin);
    msg->toname[strlen(msg->toname)-1]='\0';

    printf("请输入要发送的内容:\n");
    fgets(msg->msg,sizeof(msg->msg),stdin);
    msg->msg[strlen(msg->msg)-1]='\0';

    strcpy(msg->fronname,msg->name);
    save_record(1,msg);
    send(sockfd,msg,sizeof(struct Msg),0);
}

//查看聊天记录
void see_record(struct Msg *msg)
{
    printf("请选择查看记录选项\n");
    printf("\n\n     1、群聊记录");
    printf("\n\n     2、私聊记录\n");
    int num;
    scanf("%d",&num);
    switch(num)
    {
        case 1:
                see_grouprecord(msg);
                break;
        case 2:
                see_privaterecord(msg);
                break;
        default:
                printf("输入错误,请输入1-2的整数\n");
                break;
    }
    return;
}

//删除聊天记录
void dele_record(struct Msg *msg)
{
    printf("请选择删除记录选项\n");
    printf("\n\n     1、群聊记录");
    printf("\n\n     2、私聊记录\n\n");
    int num;
    scanf("%d",&num);
    switch(num)
    {
        case 1:
                dele_grouprecord(msg);
                break;
        case 2:
                dele_privaterecord(msg);
                break;
        default:
                printf("输入错误,请输入1-2的整数\n");
                break;
    }
    return;
}

//注销当前用户
void logout(int sockfd,struct Msg *msg)
{
    msg->cmd=7;
    send(sockfd,msg,sizeof(struct Msg),0);
    printf("该用户正在注销中......\n");
  
}

//退出客户端
void off_line(int sockfd,struct Msg *msg)
{
    msg->cmd=8;
    send(sockfd,msg,sizeof(struct Msg),0);
    printf("该用户正在下线中......\n");  
}

//禁言
void forbid_speak(int sockfd,struct Msg *msg)
{
    msg->cmd=6;
    printf("请输入你要禁言的对象：\n");
    fgets(msg->toname,sizeof(msg->toname),stdin);
    msg->toname[strlen(msg->toname)-1]='\0';
    strcpy(msg->fronname,"Admin");
    send(sockfd,msg,sizeof(struct Msg),0);
}

//解除禁言
void releve_speak(int sockfd,struct Msg *msg)
{
    msg->cmd=7;
    printf("请输入你要解除禁言的对象:\n");
    fgets(msg->toname,sizeof(msg->toname),stdin);
    msg->toname[strlen(msg->toname)-1]='\0';
    strcpy(msg->fronname,"Admin");
    send(sockfd,msg,sizeof(struct Msg),0);
}

//踢出群聊
void kickout_group(int sockfd,struct Msg *msg)
{
    msg->cmd=9;
    printf("请输入你要踢出聊天室的成员：\n");
    fgets(msg->toname,20,stdin);
    msg->toname[strlen(msg->toname)-1]='\0';
    send(sockfd,msg,sizeof(struct Msg),0);  
}

//发送文件
void send_file(int sockfd,struct Msg *msg)
{
    msg->cmd=6;
    strcpy(msg->fronname,msg->name);
    printf("\t\n请输入要发送的文件名:\n");
    fgets(msg->msg,1024,stdin);
    msg->msg[strlen(msg->msg)-1]='\0';
    
    int size=file_size(msg->msg);
    msg->filesize=size;
    send(sockfd,msg,sizeof(struct Msg),0);   //发送一个客户端要上传文件的信息

    int fd=open(msg->msg,O_RDONLY|O_EXCL,0777);  //客户端读取文件内容，如果文件不存在则报错
    if(fd==-1)
    {
        perror("open");
        printf("发送文件失败，该文件不存在\n");
        return ;
    }

    char buf[65535]={0};
    memset(buf,0,65535);

    int ret=read(fd,buf,65535);
    if(ret==-1)
    {
        perror("read");
        printf("发送文件失败\n");
        return ;
    }
    send(sockfd,buf,ret,0);
    close(fd);
}

//文件下载
void download_file(int sockfd,struct Msg *msg)
{
    msg->cmd=9;
    printf("\n\n请输入你要下载的文件名:\n");
    fgets(msg->msg,1024,stdin);
    msg->msg[strlen(msg->msg)-1]='\0';
    send(sockfd,msg,sizeof(struct Msg),0);
}

//计算文件大小
int file_size(char *s)
{
    int fd=open(s,O_RDONLY,0777);
    if(fd==-1)
    {
        perror("open");
        printf("计算文件大小失败\n");
        return -1;
    }

    int size=lseek(fd,0,SEEK_END);

    close(fd);

    return size;
}

//查看群聊记录
void see_grouprecord(struct Msg *msg)
{
    int fd=open("./public.txt",O_RDONLY);  //群聊文件
    if(fd==-1)
    {
        perror("open");
        printf("查看群聊记录失败\n");
        return ;
    }

    struct gp_record gp1;

    while(1)
    {
        int ret=read(fd,&gp1,sizeof(gp1));
        if(ret ==-1)
        {
            perror("read");
            printf("查看群聊记录失败\n");
            return ;
        }
        else if(ret ==0)
        {
            printf("\n\n读到文件末尾了\n");
            break;
        }
        printf("%s %s群发了一条消息:%s\n",gp1.tim,gp1.fronname,gp1.word);
        memset(&gp1,0,sizeof(gp1));
    }
    close(fd);
}

//查看私聊记录
void see_privaterecord(struct Msg *msg)
{
    char ch[128]={0};
    sprintf(ch,"%s_private.txt",msg->name);
    int fd=open(ch,O_RDONLY|O_CREAT,0777);
    if(fd==-1)
    {
        perror("open");
        printf("查看私聊记录失败\n");
        return ;
    }

    struct pr_record pr1;
    while(1)
    {
        int ret=read(fd,&pr1,sizeof(pr1));
        if(ret ==-1)
        {
            perror("read");
            printf("查看私聊记录失败\n");
            return ;
        }
        else if(ret ==0)
        {
            printf("\n\n读到文件末尾了\n");
            break;
        }
        printf("%s %s向%s发了一条消息:%s\n",pr1.tim,pr1.fronname,pr1.toname,pr1.word);
        memset(&pr1,0,sizeof(pr1));
    }
    close(fd);
}

//删除群聊记录
void dele_grouprecord(struct Msg *msg)
{
    int fd=open("./public.txt",O_WRONLY|O_TRUNC);
    if(fd==-1)
    {
        perror("open");
        printf("删除群聊记录失败\n");
        return ;
    }
    close(fd);
}

//删除私聊记录
void dele_privaterecord(struct Msg *msg)
{
    char ch[128]={0};
    sprintf(ch,"%s_private.txt",msg->name);
    int fd=open(ch,O_WRONLY|O_TRUNC|O_CREAT,0777);
    if(fd==-1)
    {
        perror("open");
        printf("删除群聊记录失败\n");
        return ;
    }
    close(fd);
}

//保存聊天记录
void save_record(int flag2,struct Msg *msg)
{
    time_t curtime;
    time(&curtime);
    char s[30];
    strcpy(s,ctime(&curtime));
    s[strlen(s)-1]='\0';
    switch(msg->cmd+flag2)
    {
        case 2:
                group_record(s,msg);  //群聊的时候:msg->cmd=2,再令flag2=0；flag2+cmd=2
                break;
        case 4:
                private_Srecord(s,msg);  //私聊的时候msg->cmd=3,flag2=1;flag2+cmd=4;保存客户端发送的私聊
                break;
        case 5:
                private_Rrecord(s,msg);  //私聊的时候msg->cmd=3,flag2=2; flag2+cmd=5,保存客户端接收的私聊
    }               
}

//保存群聊聊天记录在本地文件
void group_record(char *s,struct Msg *msg)
{
    int fd=open("./public.txt",O_WRONLY|O_APPEND|O_CREAT,0777);
    if(fd==-1)
    {
        perror("open");
        printf("打开群聊文件失败\n");
        return;
    }

    struct gp_record gp1;
    strcpy(gp1.fronname,msg->fronname);
    strcpy(gp1.word,msg->msg);
    strcpy(gp1.tim,s);

    int ret=write(fd,&gp1,sizeof(gp1));
    if(ret==-1)
    {
        perror("write");
        printf("写入群聊记录失败\n");
    }

    close(fd);
}

//保存发送的私聊记录在本地文件
void private_Srecord(char *s,struct Msg *msg)
{
    char ch[128]={0};
    sprintf(ch,"%s_private.txt",msg->fronname);  //发送消息时，msg->fronname为客户端用户名
    int fd=open(ch,O_WRONLY|O_APPEND|O_CREAT,0777);
    if(fd==-1)
    {
        perror("open");
        printf("写入私聊记录失败\n");
        return ;
    }

    struct pr_record pr1;
    strcpy(pr1.fronname,msg->fronname);
    strcpy(pr1.toname,msg->toname);
    strcpy(pr1.word,msg->msg);
    strcpy(pr1.tim,s);

    int ret=write(fd,&pr1,sizeof(pr1));
    if(ret ==-1)
    {
        perror("write");
        printf("写入私聊记录失败\n");
        return;
    }

    close(fd);
}

//保存接收的私聊记录在本地文件
void private_Rrecord(char *s,struct Msg *msg)
{
    char ch[128]={0};
    sprintf(ch,"%s_private.txt",msg->toname);   //msg->toname 才是客户端的用户名
    int fd=open(ch,O_WRONLY|O_APPEND|O_CREAT,0777);
    if(fd==-1)
    {
        perror("open");
        printf("写入私聊记录失败\n");
        return ;
    }

    struct pr_record pr1;
    strcpy(pr1.fronname,msg->fronname);
    strcpy(pr1.toname,msg->toname);
    strcpy(pr1.word,msg->msg);
    strcpy(pr1.tim,s);

    int ret=write(fd,&pr1,sizeof(pr1));
    if(ret ==-1)
    {
        perror("write");
        printf("写入私聊记录失败\n");
        return;
    }

    close(fd);
}

int Common_menu(struct Msg *msg)
{
    system("clear");
    printf("**************************************************************************\n");
    printf("\n\n\n");
    printf("****************************用户%s************************************\n",msg->name);
    printf("\n\n\n");
    printf("**************************************************************************\n");
    printf("\n\n\n\n\n");
    printf("                                1、查看其他在线成员\n");
    putchar(10);
    printf("                                2、群聊\n");
    putchar(10);
    printf("                                3、悄悄话\n");
    putchar(10);
    printf("                                4、查看聊天记录\n");
    putchar(10);
    printf("                                5、删除聊天记录\n");
    putchar(10);
    printf("                                6、上传文件\n");
    putchar(10);
    printf("                                7、注销用户\n");
    putchar(10);
    printf("                                8、退出当前账号\n");
    putchar(10);
    printf("                                9、下载文件\n");
    printf("\n\n\n\n\n");
    printf("**************************************************************************\n"); 

    int num;
    printf("请输入菜单选项\n");
    scanf("%d",&num);
    getchar();
    return num;
}

int Root_menu(struct Msg *msg)
{
    
    system("clear");
    printf("**************************************************************************\n");
    printf("\n\n\n");
    printf("******************************用户%s************************************\n",msg->name);
    printf("\n\n\n");
    printf("**************************************************************************\n");
    printf("\n\n\n\n\n");
    printf("                                1、查看其他在线成员\n");
    putchar(10);
    printf("                                2、群发消息\n");
    putchar(10);
    printf("                                3、悄悄话\n");
    putchar(10);
    printf("                                4、查看聊天记录\n");
    putchar(10);
    printf("                                5、删除聊天记录\n");
    putchar(10);
    printf("                                6、设置禁言\n");
    putchar(10);
    printf("                                7、解除禁言\n");
    putchar(10);
    printf("                                8、退出当前聊天室\n");
    putchar(10);
    printf("                                9、踢出成员\n");
    printf("\n\n\n\n\n");
    printf("**************************************************************************\n"); 

    int num;
    printf("请输入菜单选项\n");
    scanf("%d",&num);
    getchar();
    return num;
}

void *RecvMsg(void *arg)
{
    struct Msg msg;
    int sockfd = *(int *)arg;

    while (1)
    {
        memset(&msg,0,sizeof(struct Msg));
        recv(sockfd, &msg, sizeof(struct Msg), 0);
        switch (msg.cmd)
        {
        case 1: //收到在线用户
            dispy_online(&msg);
            sleep(5);
            break;
        case 2:
            printf("\n用户%s 给大家发了一条消息:%s\n", msg.name, msg.msg); //收到的一条群聊消息
            sleep(5);
            break;
        case 3:
            printf("\n用户%s 给你发了一条消息:%s\n", msg.fronname, msg.msg); //收到一条私信
            save_record(2,&msg);       //保存接收的私聊聊天记录
            sleep(5);
            break;
        case 6:
            printf("\n用户%s 上传了一份文件:%s\n", msg.fronname, msg.msg);
            sleep(5);
            break;
        case 7:
            return NULL;
        case 8:
            pthread_exit(0);
            return NULL;
        case 9:
            fun(sockfd,&msg);
            break;
        case 1002:
            printf("发送失败，该用户未上线\n");
            sleep(5);
            break;
        case 1003:
            printf("\n\n您已被管理员禁言\n\n");
            break;
        case 1004:
            printf("\n您已被管理员解除禁言\n");
            sleep(5);
            break;
        case 1005:
            printf("\n用户%s 被管理员踢出聊天室\n", msg.msg);
            sleep(5);
            break;
        case 1006:
            printf("\n\n 您已被管理员踢出聊天室\n\n");
            printf("\n\n正在退出聊天室......\n\n");
            sleep(5);
            offline = 1; //被踢标志位
            pthread_exit(NULL);  //结束通话线程
            break;
        }
    }
}

void dispy_online(struct Msg *msg)
{
    int i;
    printf("在线人员如下：\n\n");
    for (i = 0; i < N; i++)
    {
        if(msg->usr[i].socket !=0)
        {
            printf("%-5s ", msg->usr[i].name);
        }
        else
        {
            break;
        }
        
    }
    putchar(10);
    return;
}

//下载文件
void fun(int sockfd,struct Msg *msg)
{
    printf("\n\t正在下载中....\n");
    int fd=open(msg->msg,O_WRONLY|O_CREAT,0777);
    if(fd==-1)
    {
        perror("open");
        printf("下载文件失败\n");
        return;
    }

    int size =msg->filesize;
    printf("下载的文件大小为%d个字节\n",size);

    char buf[65535]={0};
    memset(buf,0,65535);
    int ret=recv(sockfd,buf,size,0);
    if(ret==-1)
    {
        perror("read");
        printf("\n下载文件失败\n");
        return ;
    }
    int ret1=write(fd,buf,ret);
    if(ret1==-1)
    {
        perror("write");
        printf("写入文件失败");
    }
    close(fd);
    sleep(1);
    printf("\n文件下载完成\n");
    
}

