#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main(int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8999);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    if (listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    int buff_size = 130;
    char buff[buff_size];

    printf("======waiting for client's request======\n");
    while (1) {
        if ((connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) == -1) {
            printf("accept socket error: %s(errno: %d)", strerror(errno),
                   errno);
            continue;
        }
        //在n = recv()之前，先
        memset(buff, 0, buff_size);
        int nRecv = recv(connfd, buff, buff_size, 0);
        string recev_str = buff;
        while (nRecv == buff_size) {
            printf("recv size exceed limit=[%d],cur=[%d]\n", buff_size, nRecv);
            memset(buff, 0, buff_size);
            nRecv = recv(connfd, buff, buff_size, 0);
            recev_str += buff;
        }

        printf(">>>recv size:%d content:\n%s<<<\n", recev_str.length(),
               recev_str.data());

        string result_str = "HTTP/1.1 200 OK\n";
        result_str += "Content-Type:text/html;charset=utf-8\n\n";
        result_str += "<h5>你刚才发送的请求数据是：<br>";
        result_str += recev_str;
        result_str += "</h5>";
        printf(">>>feedback:\n%s<<<\n", result_str.data());
        //向TCP连接的另一端发送数据。
        send(connfd, result_str.data(), result_str.length(), 0);
        close(connfd);
    }
    close(listenfd);
    return 0;
}