#ifndef ENGINE_H_
#define ENGINE_H_
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define speed_per_power ((1.0/105.0)*Lfield/match.nTick)

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

void set_ball_spd(struct Ball *, Speed);
#endif //ENGINE_H_
