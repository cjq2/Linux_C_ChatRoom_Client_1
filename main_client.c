#include "client.h"


int main(int argc, char const *argv[])
{
    int sockfd;
    Socket_init(&sockfd);    //客户端初始化
    Chat_Room(sockfd);     //聊天室
    return 0;
}
