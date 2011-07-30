#include "global.h"
#include "engine.h"
#include "coordinate.h"
#include "actions.h"
#include "utilities.h"
#include "librobot.h"

#define ACTION_TYPE(a) (a.ac_type)
#define ACTION_DIRT(a) (a.direct)
#define ACTION_SPEED(a) (a.spd)
#define ACTION_POWER(a) (generate_power(a.spd, speed_per_power))
#define ACTION_FRAME(a) (a.frame)

static Action generate_action(int ac_type, Direction dirt, Speed spd, int new_frame)
{
    Action ac;
    ac.ac_type = ac_type;
    ac.direct = dirt;
    ac.spd = spd;
    ac.frame = new_frame;
    return ac;
}

static int equal_action(Action ac1, Action ac2)
{
    if (ACTION_TYPE(ac1) == ACTION_TYPE(ac2) 
            && equal_vector(ACTION_DIRT(ac1), ACTION_DIRT(ac2))
            && equal_vector(ACTION_SPEED(ac1), ACTION_SPEED(ac2)))
        return 1;
    else
        return 0;
}

static void adjust_ball_pos(struct Player *pp, Direction dirt)
{
    Direction d;
    d = normalizing(dirt);
    d.x *= 1.0/(HOLD_DISTANCE *2.0);
    d.y *= 1.0/(HOLD_DISTANCE *2.0);
    d.z *= 1.0/(HOLD_DISTANCE *2.0);

    match.ball.pos.x = pp->pos.x + d.x;
    match.ball.pos.y = pp->pos.y + d.y;
    match.ball.pos.z = pp->pos.z + d.z;
}

static Action previous_action(struct Player *pp)
{
    return pp->action;
}

static int next_frame(struct Player *pp, Action ac, int frame)
{
    int nframe;
    static float inner_timers[22];
    int ac_type = ACTION_TYPE(ac);
    float *my_timer = &inner_timers[pp->id];
    switch (ac_type)
    {
        case TY_RUN:
            if (timer - *my_timer >= FT_RUN)
            {
                nframe = frame - 1;
                if (nframe < 0)
                    nframe = FR_RUN;
            }
            else
                nframe = frame;
            break;
        case TY_KICK:
            if (timer - *my_timer >= FT_KICK)
            {
                nframe = frame - 1;
                if (nframe < 0)
                    nframe = FR_KICK;
            }
            else
                nframe = frame;
            break;
        case TY_STAY:
            if (timer - *my_timer >= FT_STAY)
            {
                nframe = frame - 1;
                if (nframe < 0)
                    nframe = FR_STAY;
            }
            else
                nframe = frame;
            break;
        case TY_HOLD:
            if (timer - *my_timer >= FT_HOLD)
            {
                nframe = frame - 1;
                if (nframe < 0)
                    nframe = FR_HOLD;
            }
            else
                nframe = frame;
            break;
        case TY_POUNCE:
            if (timer - *my_timer >= FT_POUNCE)
            {
                nframe = frame - 1;
                if (nframe < 0)
                    nframe = FR_POUNCE;
            }
            else
                nframe = frame;
            break;
        case TY_KEEP:
            if (timer - *my_timer >= FT_KEEP)
            {
                nframe = frame - 1;
                if (nframe < 0)
                    nframe = FR_KEEP;
            }
            else
                nframe = frame;
            break;
        case TY_SHOVEL:
            if (timer - *my_timer >= FT_SHOVEL)
            {
                nframe = frame - 1;
                if (nframe < 0)
                    nframe = FR_SHOVEL;
            }
            else
                nframe = frame;
            break;
        case TY_DRIBBLE:
            if (timer - *my_timer >= FT_DRIBBLE)
            {
                nframe = frame - 1;
                if (nframe < 0)
                    nframe = FR_DRIBBLE;
            }
            else
                nframe = frame;
            break;
        case TY_TUMBLE:
            if (timer - *my_timer >= FT_TUMBLE)
            {
                nframe = frame - 1;
                if (nframe < 0)
                    nframe = FR_TUMBLE;
            }
            else
                nframe = frame;
            break;

    }
    if (nframe != frame)
        *my_timer = timer;
    return nframe;
}

static void set_frame(struct Player *pp, int frame)
{
    pp->action.frame = frame;
}

static void perform_action(struct Player *pp, Action ac)
{
    Speed spd;
    Direction dirt;
    Speed still = {0.0, 0.0, 0.0};
    float dis;
    int frame = ACTION_FRAME(ac);

    switch (ACTION_TYPE(ac))
    {
        case TY_RUN:
            spd = ACTION_SPEED(ac);
            dirt = ACTION_DIRT(ac);
            pp->direct = dirt;
            pp->spd = spd;
            break;
        case TY_KICK:
            spd = ACTION_SPEED(ac);
            dirt = ACTION_DIRT(ac);
            if (frame == 5)
            {
                dis = distance(pp->pos, match.ball.pos);
                if (dis <= HOLD_DISTANCE)
                {
                    Speed bspd = {spd.x * 1.1, spd.y*1.1, spd.z*1.1};
                    match.ball.spd = bspd;
                    match.ball.kicked = 1;
                    match.ball.last_toucher = pp;
                }
            }
            pp->spd = still;
            pp->direct = dirt;
            break;
        case TY_SHOVEL:
            spd = ACTION_SPEED(ac);
            dirt = ACTION_DIRT(ac);
            dis = distance(pp->pos, match.ball.pos);
            if (dis <= HOLD_DISTANCE)
            {
                Speed bspd = {spd.x * 1.3, spd.y * 1.3, spd.z *1.3};
                match.ball.spd = bspd;
                match.ball.kicked = 1;
                match.ball.last_toucher = pp;
            }
            pp->spd = spd;
            pp->direct = dirt;
            break;
        case TY_STAY:
            dirt = ACTION_DIRT(ac);
            pp->spd = still;
            pp->direct = dirt;
            break;
        case TY_HOLD:
            pp->spd = still;
            dirt = ACTION_DIRT(ac);
            pp->direct = dirt;
            dis = distance(pp->pos, match.ball.pos);
            if (dis <= HOLD_DISTANCE)
            {
                    adjust_ball_pos(pp, pp->direct);
                    match.ball.spd = still;
                    match.ball.last_toucher = pp;
            }
            break;
        case TY_KEEP:
            pp->spd = still;
            dirt = ACTION_DIRT(ac);
            pp->direct = dirt;
            dis = distance(pp->pos, match.ball.pos);
            if (dis <= HOLD_DISTANCE)
            {
                match.ball.spd = still;
                match.ball.keeped = 1;
                match.ball.last_toucher = pp;
            }
            break;
        case TY_POUNCE:
            dis = distance(pp->pos, match.ball.pos);
            if (dis <= HOLD_DISTANCE)    // set spd of ball before that of pp!
            {
                match.ball.spd = pp->spd;
                match.ball.keeped = 1;
                match.ball.last_toucher = pp;
            }
            pp->spd = ACTION_SPEED(ac);
            pp->direct = ACTION_DIRT(ac);
            break;
        case TY_TUMBLE:
            pp->spd = ACTION_SPEED(ac);
            pp->direct = ACTION_DIRT(ac);
            break;
        case TY_DRIBBLE:
            pp->spd = ACTION_SPEED(ac);
            pp->direct = ACTION_DIRT(ac);
            dis = distance(pp->pos, match.ball.pos);
            if (dis <= HOLD_DISTANCE)
            {
                Speed bspd = {pp->spd.x * 1.3, pp->spd.y * 1.3, pp->spd.z *1.3};
                match.ball.spd = bspd;
                match.ball.kicked = 1;
                match.ball.last_toucher = pp;
            }
            break;
        default:
            return;
    }
}

static int action_interruptable(Action ac)
{
    int ac_type = ACTION_TYPE(ac);
    if (ac_type == TY_POUNCE || ac_type == TY_TUMBLE || ac_type == TY_KICK
            || ac_type == TY_SHOVEL)
        return 0;
    else
        return 1;
}

static void set_action(struct Player *pp, Action ac)
{
    pp->action = ac;
}


static void do_action(struct Player *pp, Action ac)
{
    Action pre_action;
    pre_action = previous_action(pp);
    int pre_frame = pre_action.frame;
    int nframe;

    if (equal_action(pre_action, ac))
    {
        nframe = next_frame(pp, ac, pre_frame);
        set_frame(pp, nframe);
        perform_action(pp, pre_action);
    }
    else if (pre_frame != 0)    // previous action unfinished
    {
        if (action_interruptable(pre_action))
        {
            set_action(pp, ac);
            perform_action(pp, ac); //pp->spd = spd;
        }
        else
            do_action(pp, pre_action);
    }
    else          // previous action finished, set new action
    {
        set_action(pp, ac);
        perform_action(pp, ac);
    }
}

/* basic actions with independent frames */
int act_run(struct Player *pp, Direction dirt, Speed spd)
{
    Action action;
    action = generate_action(TY_RUN, dirt, spd, FR_RUN);
    do_action(pp, action);
    return 0;
}

int act_kick(struct Player *pp, Direction dirt, Speed spd)
{
    float dis;
    dis = distance(pp->pos, match.ball.pos);
    Action action = generate_action(TY_KICK, dirt, spd, FR_KICK);

    if (dis <= HOLD_DISTANCE)
    {
        do_action(pp, action);
        return 0;
    }
    else
        return -1;
}

int act_shovel(struct Player *pp, Direction dirt, Speed spd)
{
    float dis;
    dis = distance(pp->pos, match.ball.pos);
    Action action = generate_action(TY_SHOVEL, dirt, spd, FR_SHOVEL);
    do_action(pp, action);
    return 0;
}

int act_hold(struct Player *pp, Direction dirt)
{
    float dis = distance(pp->pos, match.ball.pos);
    Speed still = {0.0, 0.0, 0.0};
    Action action = generate_action(TY_HOLD, dirt, still, FR_HOLD);

    int re = 0;
    if (dis > HOLD_DISTANCE && dis < 4.0*HOLD_DISTANCE)
        return act_runto(pp, match.ball.pos, 10);
    else if (dis <= HOLD_DISTANCE)
        do_action(pp, action);
    else
        re = -1;
    return re;
}

int act_stay(struct Player *pp, Direction dirt)
{
    Speed still = {0.0, 0.0, 0.0};
    Action action = generate_action(TY_STAY, dirt, still, FR_STAY);

    if (rbt_i_have_ball(pp) && ACTION_TYPE(previous_action(pp)) != TY_KICK)
        return act_hold(pp, dirt);
    else
        do_action(pp, action);
    return 0;
}

int act_pounce(struct Player *pp, Direction dirt, Speed spd)
{
    Action action = generate_action(TY_POUNCE, dirt, spd, FR_POUNCE);
    do_action(pp, action);
    return 0;
}

int act_keep(struct Player *pp, Direction dirt)
{
    Speed still = {0.0, 0.0, 0.0};
    Action action = generate_action(TY_KEEP, dirt, still, FR_KEEP);
    float dis = distance(pp->pos, match.ball.pos);

    if (dis <= HOLD_DISTANCE)
    {
        do_action(pp, action);
        return 0;
    }
    else
        return -1;
}

/*
int act_turn(struct Player *pp)
{
}

int act_tumble(struct Player *pp)
{
}

int act_head(struct Player *pp)
{
}

int act_chest(struct Player *pp)
{
}

int act_leg(struct Player *pp)
{
}

int act_reverse(struct Player *pp)
{
}
// basic actions end 
*/

int act_short_pass(struct Player *pp)
{
    Direction dir;
    Speed spd;
    int power;
    struct Player *topasser;
    topasser = find_to_passer(pp);
    if (topasser == NULL)
    {
        power = 6;
        dir = pp->direct;
    }
    else
    {
        float dis = distance(topasser->pos, pp->pos) + 2.0*Meter;
        power =  2*sqrt(dis);
        dir.x = topasser->pos.x - pp->pos.x;
        dir.y = topasser->pos.y - pp->pos.y;
        dir.z = topasser->pos.z - pp->pos.z;
    }
    spd = generate_speed(dir, power, speed_per_power);
    return act_kick(pp, dir, spd);
}


int act_shot(struct Player *pp, Direction dirt, Speed spd)
{
    return act_kick(pp, dirt, spd);
}

int act_runto(struct Player *pp, Position pos, int power)
{
    Direction dir;
    Speed spd;
    dir.x = pos.x - pp->pos.x;
    dir.y = pos.y - pp->pos.y;
    dir.z = pos.z - pp->pos.z;
    spd = generate_speed(dir, power, speed_per_power); 
    return act_run(pp, dir, spd);
}


int act_dribble(struct Player *pp, Direction dirt, Speed spd)
{
    float dis = distance(pp->pos, match.ball.pos);
    Action ac = generate_action(TY_DRIBBLE, dirt, spd, FR_DRIBBLE);

    if (!rbt_i_have_ball(pp))
        return -1;
    else if (angle(dirt, pp->direct) > 5*PI/180)
        return act_hold(pp, spd);
    else if (dis <= HOLD_DISTANCE)
    {
        do_action(pp, ac);
        return 0;
    }
    else
        return -1;

    return 0;
    /*if (dis <= HOLD_DISTANCE)
    {
        if (angle(dirt, pp->direct) > 5*PI/180)
            act_hold(pp, spd, ball);
        else
        {
            act_kick(pp, spd, spd, ball);
            act_run(pp, spd, spd);
        }
    }
    else
        act_run(pp, spd, spd);*/
}

int act_tumble(struct Player *pp, Speed spd, Direction dirt)
{
    Action action = generate_action(TY_TUMBLE, spd, dirt, FR_TUMBLE);
    do_action(pp, action);
    return 0;
}
