#ifndef LIBROBOT_H_
#define LIBROBOT_H_
#include <stdlib.h>
#include <math.h>

typedef enum {KEEPER = 0, BACK, MIDDLE, FRONT} Duty;
// utilities
int init_robot(struct Match *);
// judgements
int counter_team(int);
int which_team_hold_ball(void);
struct Team *rbt_my_team(struct Player *);
int rbt_my_duty(struct Player *);
Position rbt_where_ball(void);
int rbt_i_have_ball(struct Player *);
struct Player *rbt_who_hold_ball(void);
Scope rbt_my_precinct(struct Player *);
Tactics rbt_team_tactics(struct Player *);
float rbt_direct_difficulty(struct Player *, Direction);
float rbt_pass_factor(struct Player *);
Position rbt_judge_ball_dest(struct Player *);
int rbt_ball_flying_to_gate(Position, int);
void stay_in_defence(struct Player *);
struct Player *find_to_passer(struct Player *);
Direction pounce_direction(Position, Position, Position);
Direction direct_to_ball(struct Player *);

extern Scope defend_area[22];
extern Scope volley_area[22];

#endif //LIBROBOT_H_
