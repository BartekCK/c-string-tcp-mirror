#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUFFER_MAX 2042

int err(char *s)
{
    printf("%s\n", s);
    printf("Errno:%d\n", errno);
    fprintf(stderr, "%s\n", strerror(errno));
    exit(-1);
}

struct sockaddr_in create_serv_addr(int port)
{
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    return serv_addr;
}

int main(int argc, char const *argv[])
{
    int sock_fd;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        err("socket");
    }
    struct sockaddr_in serv_addr = create_serv_addr(3001);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    if ((connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1)
    {
        err("connect");
    }

    char message[BUFFER_MAX];
    printf("Enter your message\n");
    scanf("%[^\n]%*c", message);

    char res_mess[BUFFER_MAX] = "";

    if (write(sock_fd, message, strlen(message)) == -1)
    {
        err("write");
    }
    if (read(sock_fd, res_mess, BUFFER_MAX - 1) == -1)
    {
        err("read");
    }

    printf("Response is: %s\n", res_mess);

    return 0;
}
