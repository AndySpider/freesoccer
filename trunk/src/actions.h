#ifndef ACTION_H_
#define ACTION_H_
#include <math.h>

typedef enum {IMPOSSIBLE = -1, UNFINISHED, DONE} AC_RESULT;

int act_runto(struct Player *, Direction, Position pos, int apower, int ppower);
int act_dribble(struct Player *, Dirspeed, Speed, Speed);
int act_run(struct Player *, Dirspeed, Speed);
int act_kick(struct Player *, Dirspeed, Speed, Speed);
int act_shovel(struct Player *, Dirspeed, Speed, Speed);
int act_hold(struct Player *, Dirspeed);
int act_stay(struct Player *, Dirspeed);
int act_pounce(struct Player *, Dirspeed, Speed);
int act_keep(struct Player *, Dirspeed);
//void act_turn(struct Player *);
int act_shot(struct Player *, Dirspeed, Speed, Speed);
int act_tumble(struct Player *, Dirspeed, Speed);

int act_short_pass(struct Player *);
// actions
#define TY_RUN      1  
#define TY_KICK     2
#define TY_STAY     3
#define TY_HOLD     4
#define TY_POUNCE   5
#define TY_KEEP     6 
#define TY_TURN     7
#define TY_TUMBLE   8 
#define TY_HEAD     9
#define TY_CHEST    10
#define TY_LEG      11
#define TY_REVERSE  12
#define TY_SHOVEL   13
#define TY_DRIBBLE  14

#define FR_RUN      10
#define FT_RUN      0.02
#define FR_KICK     10
#define FT_KICK     0.05
#define FR_STAY     1
#define FT_STAY     0.1
#define FR_SHOVEL   3
#define FT_SHOVEL   0.02
#define FR_HOLD     10
#define FT_HOLD     0.1
#define FR_KEEP     10
#define FT_KEEP     0.1
#define FR_TURN     10
#define FT_TURN     0.1
#define FR_POUNCE   10
#define FT_POUNCE   0.1
#define FR_TUMBLE   15 
#define FT_TUMBLE   0.1
#define FR_DRIBBLE  10
#define FT_DRIBBLE  0.02

#endif 
