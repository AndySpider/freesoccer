#ifndef ENGINE_H_
#define ENGINE_H_
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define speed_per_power ((1.0/105.0)*Lfield/match.nTick)
#define dirspd_per_power (PI/180.0/match.nTick)

struct Match match;
extern float timer;
extern struct Player *focuser;
extern struct Player *holder;

struct Exchange {
    int id1;
    int id2;
};

struct Player *id2player(int);
int diff_team(struct Player, struct Player);
void change_ball_speed(Speed, struct Vector, float);
Angle direct_to_ball(struct Player *);

#endif //ENGINE_H_
