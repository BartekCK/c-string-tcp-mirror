#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define PENDING_CONNECTION_BACKLOG 50
#define BUFFER_MAX 2042

int err(char *s);
struct sockaddr_in create_serv_addr(int port);
char *read_message(int sock_fd);
char *change_str_order(char *msg);
void send_message(int sock_fd, char *msg);

int main(int argc, char const *argv[])
{
    int conn_fd, listen_fd;
    struct sockaddr_in cli_addr;
    socklen_t cli_len;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        err("socket");
    }

    struct sockaddr_in serv_addr = create_serv_addr(3001);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        err("bind");
    }

    if (listen(listen_fd, PENDING_CONNECTION_BACKLOG) == -1)
    {
        err("listen");
    }
    printf("The server is ready\n");

    while (1)
    {
        cli_len = sizeof(cli_addr);
        printf("Waiting on accept()\n");
        if ((conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len)) == -1)
        {
            err("accept");
        }
        printf("New client connection completed successfully\n");

        char *message = read_message(conn_fd);
        printf("Reveived message: %s\n", message);
        char *new_msg = change_str_order(message);
        send_message(conn_fd, new_msg);


        message = NULL;
        new_msg = NULL;
        free(message);
        free(new_msg);

        close(conn_fd);
    }

    close(listen_fd);
    return 0;
}

char *change_str_order(char *msg)
{
    int size = strlen(msg);
    int end = size - 1;
    char *new_msg = (char *)malloc(size);

    for (int i = 0; i < size; i++)
    {
        new_msg[i] = msg[end];
        end--;
    }
    new_msg[size] = '\0';
    
    return new_msg;
}

void send_message(int sock_fd, char *msg)
{
    write(sock_fd, msg, strlen(msg));
}

char *read_message(int sock_fd)
{
    char *msg = (char *)malloc(BUFFER_MAX - 1);
    int n = read(sock_fd, msg, BUFFER_MAX - 1);
    if (n == -1)
    {
        err("read");
    }
    return msg;
}

struct sockaddr_in create_serv_addr(int port)
{
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    return serv_addr;
}

int err(char *s)
{
    printf("%s\n", s);
    printf("Errno:%d\n", errno);
    fprintf(stderr, "%s\n", strerror(errno));
    exit(-1);
}
