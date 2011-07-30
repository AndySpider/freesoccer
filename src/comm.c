#include "global.h"
#include "comm.h"

struct sockaddr_in sa;
socklen_t sa_len;

int init_server_sock()
{
    int sockfd;
     sockfd = socket(PF_INET, SOCK_DGRAM,0);
     if (sockfd == -1)
     {
         perror("can not create socket");
         exit(-1);
     }
     memset(&sa, 0, sizeof(sa));
     sa.sin_family = AF_INET;
     sa.sin_port = htons(PORT);
     sa.sin_addr.s_addr = INADDR_ANY;
     sa_len = sizeof(sa);

     if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1)
     {
         perror("error bind failed");
         close(sockfd);
         exit(-1);
     }
     return sockfd;
}

int destroy_server_sock(int sockfd)
{
    return close(sockfd);
}
ssize_t send_init_data(int sock, struct Init_data *idata)
{
    printf("begin send idata\n");
    return sendto(sock, idata, sizeof(struct Init_data), 0, (struct sockaddr *)&sa, sa_len);
    printf("end send idata\n");
}

ssize_t recv_init_data(int sock, struct Init_data *idata)
{
    printf("begin recv idata\n");
    return recvfrom(sock, idata, sizeof(struct Init_data), 0, (struct sockaddr *)&sa, &sa_len);
    printf("end recv idata\n");
}

ssize_t send_cmd(int sock, struct Cmd *c)
{
    printf("begin send cmd\n");
    return sendto(sock, c, sizeof(struct Cmd), 0, (struct sockaddr *)&sa, sa_len);
    printf("end send cmd\n");
}

ssize_t recv_cmd(int sock, struct Cmd *c)
{
    int re;
    printf("begin recv cmd\n");
    re = recvfrom(sock ,c, sizeof(struct Cmd), 0, (struct sockaddr *)&sa, &sa_len);
    printf("recv cmd:%s\n", c->name);
    return re;
}

ssize_t send_status(int sock, struct Status *sts)
{
    printf("begin send status\n");
    return sendto(sock, sts, sizeof(struct Status), 0, (struct sockaddr *)&sa, sa_len);
    printf("end send status\n");
}

ssize_t recv_status(int sock, struct Status *sts)
{
    printf("begin recv status\n");
    return recvfrom(sock, sts, sizeof(struct Status), 0, (struct sockaddr *)&sa, &sa_len);
    printf("end recv status\n");
}

// for cmd

char *up = "UP";
char *down = "DOWN";
char *left = "LEFT";
char *right = "RIGHT";
char *lpass = "LPASS";
char *shotting = "SHOT";
char *none = "NONE";
char *mtoball = "MTOBALL";

char *restart = "RESTART";
char *start = "START";
char *half = "HALF";
char *exch = "EXCH";
char *end = "END";

int com_move_up(struct Cmd c)
{
    return strcmp(c.name, up) == 0;
}

int com_move_down(struct Cmd c)
{
    return strcmp(c.name, down) == 0;
}

int com_move_left(struct Cmd c)
{
    return strcmp(c.name, left) == 0;
}

int com_move_right(struct Cmd c)
{
    return strcmp(c.name, right) == 0;
}

int com_long_pass(struct Cmd c)
{
    return strcmp(c.name, lpass) == 0;
}

int com_shot(struct Cmd c)
{
    return strcmp(c.name, shotting) == 0;
}

int com_move_to_ball(struct Cmd c)
{
    return strcmp(c.name, mtoball) == 0;
}

int com_none(struct Cmd c)
{
    return strcmp(c.name, none) == 0;
}

int com_restart(struct Cmd c)
{
    return strcmp(c.name, restart) == 0;
}

int com_start(struct Cmd c)
{
    return strcmp(c.name, start) == 0;
}

int com_half(struct Cmd c)
{
    return strcmp(c.name, half) == 0;
}

int com_exch(struct Cmd c)
{
    return strcmp(c.name, exch) == 0;
}

int com_end(struct Cmd c)
{
    return strcmp(c.name, end) == 0;
}
