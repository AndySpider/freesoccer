#ifndef COMM_H_
#define COMM_H_
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 4444
#define ADDR 172.0.0.1

struct Init_data {
    Difficulty dif;
    Weather weath;
    int nTick;
    int human;
    struct Team team1;
    struct Player player1[11];
    struct Team team2;
    struct Player player2[11];
};

ssize_t send_init_data(int, struct Init_data *);
ssize_t recv_init_data(int, struct Init_data *);

struct Cmd {
    char name[10];
    Speed spd;  // for use of shot
    int power;
    int id1;
    int id2;
};
//typedef enum {NONE, W, S, A, D, SHIFT, SPACE, L, H, P} Keys;
/* char *:
 * keys, EXCHANGE, START, READY, HALF, END 
 */
ssize_t send_cmd(int, struct Cmd *);  // GUI --> engine. if EXCHANGE, waiting for struct Exchange in next package. 
ssize_t recv_cmd(int, struct Cmd *);

struct Status {
    struct Ball ball;
    struct Player players[22];
    int holder_id;
    int focus_id;
    struct Event evt;
};

ssize_t send_status(int, struct Status *);                   // engine --> GUI
ssize_t recv_status(int, struct Status *);

int init_server_sock();
int destroy_server_sock(int);

// for cmd

int com_move_up(struct Cmd);
int com_move_down(struct Cmd);
int com_move_left(struct Cmd);
int com_move_right(struct Cmd);
int com_short_pass(struct Cmd);
int com_shot(struct Cmd);
int com_long_pass(struct Cmd);
int com_move_to_ball(struct Cmd);
int com_none(struct Cmd);

int com_restart(struct Cmd);
int com_start(struct Cmd);
int com_half(struct Cmd);
int com_exch(struct Cmd);
int com_end(struct Cmd);

#endif        //COMM_H_
