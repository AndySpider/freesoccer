/* Copyright (C) 
 * 2011 - hkuieagle
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include "global.h"
#include "engine.h"
#include "coordinate.h"
#include "actions.h"
#include "utilities.h"
#include "librobot.h"

#define ACTION_TYPE(a) (a.ac_type)
#define ACTION_DSPEED(a) (a.dspd)
#define ACTION_SPEED(a) (a.spd)
#define ACTION_BALLSPEED(a) (a.ball_spd)
#define ACTION_PPOWER(a) (generate_ppower(a.spd, speed_per_power))
#define ACTION_DPOWER(a) (generate_dpower(a.dirspd, dirspd_per_power))
#define ACTION_FRAME(a) (a.frame)
Speed arbi_bspd = {-999, -999, -999};

static Action generate_action(int ac_type, Dirspeed dspd, Speed spd, Speed bspd, int new_frame)
{
    Action ac;
    ac.ac_type = ac_type;
    ac.dspd = dspd;
    ac.spd = spd;
    ac.ball_spd = bspd;
    ac.frame = new_frame;
    return ac;
}

static int equal_action(Action ac1, Action ac2)
{
    if (ACTION_TYPE(ac1) == ACTION_TYPE(ac2) 
            && equal_vector(ACTION_SPEED(ac1), ACTION_SPEED(ac2))
            && ACTION_DSPEED(ac1) == ACTION_DSPEED(ac2))
        return 1;
    else
        return 0;
}

static int adjust_ball_pos(struct Player *pp, Direction dirt)
{
    if (distance(pp->pos, match.ball.pos) > HOLD_DISTANCE)
        return IMPOSSIBLE;

    if (fabsf(direct_diff(direct_to_ball(pp), dirt)) <= 5*PI/180)
        return DONE;

    struct Vector d, dballto;
    Position pballto;
    d = direct2vector(dirt, 0.7*HOLD_DISTANCE);

    pballto.x = pp->pos.x + d.x;
    pballto.y = pp->pos.y + d.y;
    pballto.z = pp->pos.z + d.z;

    dballto.x = pballto.x - match.ball.pos.x;
    dballto.y = pballto.y - match.ball.pos.y;
    dballto.z = pballto.z - match.ball.pos.z;

    Speed spd = generate_speed(dballto, 5, speed_per_power);
    change_ball_speed(spd, (struct Vector)spd, 0.0);

    return UNFINISHED;
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

static void complete_action(struct Player *pp)
{
    float dis;
    int frame = ACTION_FRAME(pp->action);
    Speed bspd = ACTION_BALLSPEED(pp->action);

    switch (ACTION_TYPE(pp->action))
    {
        case TY_RUN:
            break;
        case TY_KICK:
            if (frame == 5)
            {
                dis = distance(pp->pos, match.ball.pos);
                if (dis <= HOLD_DISTANCE) 
                {
                    change_ball_speed(bspd, (struct Vector)bspd, -0.2);
                    match.ball.kicked = 1;
                    match.ball.last_toucher = pp;
                }
            }
            break;
        case TY_SHOVEL:
            dis = distance(pp->pos, match.ball.pos);
            if (dis <= HOLD_DISTANCE)
            {
                change_ball_speed(bspd, (struct Vector)bspd, -0.2);
                match.ball.kicked = 1;
                match.ball.last_toucher = pp;
            }
            break;
        case TY_STAY:
            break;
        case TY_HOLD:
            dis = distance(pp->pos, match.ball.pos);
            if (dis <= HOLD_DISTANCE)
            {
                change_ball_speed(counter_vector(match.ball.spd), direct2vector(pp->direct, 1), -0.1);
                match.ball.last_toucher = pp;
            }
            break;
        case TY_KEEP:
            dis = distance(pp->pos, match.ball.pos);
            if (dis <= HOLD_DISTANCE)
            {
                change_ball_speed(counter_vector(match.ball.spd), direct2vector(pp->direct, 1), -0.1);
                match.ball.keeped = 1;
                match.ball.last_toucher = pp;
            }
            break;
        case TY_POUNCE:
            dis = distance(pp->pos, match.ball.pos);
            if (dis <= HOLD_DISTANCE)    // set spd of ball before that of pp!
            {
                change_ball_speed(pp->action.spd, direct2vector(pp->direct, 1), -0.1);
                match.ball.keeped = 1;
                match.ball.last_toucher = pp;
            }
            break;
        case TY_TUMBLE:
            break;
        case TY_DRIBBLE:
            dis = distance(pp->pos, match.ball.pos);
            if (dis <= HOLD_DISTANCE)
            {
                change_ball_speed(bspd, (struct Vector)bspd, -0.1);
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
        complete_action(pp);
    }
    else if (pre_frame != 0)    // previous action unfinished
    {
        if (action_interruptable(pre_action))
        {
            set_action(pp, ac);
            complete_action(pp); //pp->spd = spd;
        }
        else
            do_action(pp, pre_action);
    }
    else          // previous action finished, set new action
    {
        set_action(pp, ac);
       complete_action(pp);
    }
}

/* basic actions with independent frames */
int act_run(struct Player *pp, Dirspeed dspd, Speed spd)
{
    Action action;
    action = generate_action(TY_RUN, dspd, spd, arbi_bspd, FR_RUN);
    do_action(pp, action);
    return DONE;
}

static int i_keep_ball(struct Player *pp)
{
    if (ACTION_TYPE(pp->action) == TY_KEEP 
            && match.ball.keeped == 1 
            && match.ball.last_toucher->id == pp->id)
        return 1;
    else
        return 0;
}

static int i_kick_ball(struct Player *pp)
{
    if (ACTION_TYPE(pp->action) == TY_KICK 
            && match.ball.kicked == 1 
            && match.ball.last_toucher->id == pp->id)
        return 1;
    else
        return 0;
}

int act_kick(struct Player *pp, Dirspeed dspd, Speed spd, Speed bspd)
{
    if (i_kick_ball(pp))
        return DONE;

    if (!i_own_ball(pp))
        return IMPOSSIBLE;

    Action action = generate_action(TY_KICK, dspd, spd, bspd, FR_KICK);
    if (act_runto(pp, direct_to_ball(pp), match.ball.pos, 180, 8) == DONE)
        do_action(pp, action);

    return UNFINISHED;
}

int act_shovel(struct Player *pp, Dirspeed dspd, Speed spd, Speed bspd)
{
    if (i_kick_ball(pp))
        return DONE;

    Action action = generate_action(TY_SHOVEL, dspd, spd, bspd, FR_SHOVEL);

    do_action(pp, action);

    return UNFINISHED;
}

static int i_hold_ball(struct Player *pp)
{
    float dis = distance(pp->pos, match.ball.pos);
    Speed still = {0.0, 0.0, 0.0};
    if (ACTION_TYPE(pp->action) == TY_HOLD 
            && dis <= HOLD_DISTANCE 
            && equal_vector(match.ball.spd, still))
        return 1;
    else
        return 0;
}

int act_hold(struct Player *pp, Dirspeed dspd)
{
    if (i_hold_ball(pp))
        return DONE;

    if (!i_own_ball(pp))
        return IMPOSSIBLE;

    Speed still = {0.0, 0.0, 0.0};
    Action action = generate_action(TY_HOLD, dspd, still, arbi_bspd, FR_HOLD);

    AC_RESULT re = act_runto(pp, direct_to_ball(pp), match.ball.pos, 180, 5);
    if ( re == DONE )
    {
        do_action(pp, action);
        adjust_ball_pos(pp, pp->direct + dspd);
    }

    return UNFINISHED;
}

int act_stay(struct Player *pp, Dirspeed dspd)
{
    Speed still = {0.0, 0.0, 0.0};
    Action action = generate_action(TY_STAY, dspd, still, arbi_bspd, FR_STAY);
    do_action(pp, action);
    return DONE;
}

int act_pounce(struct Player *pp, Dirspeed dspd, Speed spd)
{
    Action action = generate_action(TY_POUNCE, dspd, spd, arbi_bspd, FR_POUNCE);
    do_action(pp, action);
    return DONE;
}

int act_keep(struct Player *pp, Dirspeed dspd)
{
    if (i_keep_ball(pp))
        return DONE;

    Speed still = {0.0, 0.0, 0.0};
    Action action = generate_action(TY_KEEP, dspd, still, arbi_bspd, FR_KEEP);
    float dis = distance(pp->pos, match.ball.pos);

    if (dis <= HOLD_DISTANCE)
    {
        do_action(pp, action);
    }
    else
        return IMPOSSIBLE;

    return UNFINISHED;
}

/*
int act_turn(struct Player *pp)
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
    struct Vector vec;
    Speed bspd;
    Direction dir;
    Dirspeed dspd;
    int spower, dpower = 180;
    struct Player *topasser;
    topasser = find_to_passer(pp);
    if (topasser == NULL)
    {
        spower = 6;
        vec = direct2vector(pp->direct, INFINITE);
        dir = 0.0;
    }
    else
    {
        float dis = distance(topasser->pos, pp->pos) + 3.0*Meter;
        spower =  3*sqrt(dis);
        vec = vector(pp->pos, topasser->pos);
        dir = vector2direct(vec);
    }
    bspd = generate_speed(vec, spower, speed_per_power);
    dspd = generate_dirspd(dir, dpower, dirspd_per_power);
    return act_kick(pp, dspd, generate_speed(match.ball.spd, 5, speed_per_power), bspd);
}


int act_shot(struct Player *pp, Dirspeed dspd, Speed spd, Speed bspd)
{
    return act_kick(pp, dspd, spd, bspd);
}

int act_runto(struct Player *pp, Direction dir, Position pos, int dpower, int spower)
{
    if (distance(pp->pos, pos) <= 0.8 * HOLD_DISTANCE )
        return DONE;

    struct Vector d;
    Speed spd;
    d.x = pos.x - pp->pos.x;
    d.y = pos.y - pp->pos.y;
    d.z = pos.z - pp->pos.z;
    spd = generate_speed(d, spower, speed_per_power); 

    Angle agl;
    Dirspeed dspd;
    agl = direct_diff(pp->direct, dir);
    dspd = generate_dirspd(agl, dpower, dirspd_per_power);

    act_run(pp, dspd, spd);
    return UNFINISHED;
}


int act_dribble(struct Player *pp, Dirspeed dspd, Speed spd, Speed bspd)
{
    Action ac = generate_action(TY_DRIBBLE, dspd, spd, bspd, FR_DRIBBLE);

    if (!i_own_ball(pp))
        return IMPOSSIBLE;

    int spower = generate_spower(spd, speed_per_power);
    int dpower = generate_dpower(dspd, dirspd_per_power);
    AC_RESULT re = act_runto(pp, direct_to_ball(pp), match.ball.pos, dpower, spower);
    if (re == DONE)
    {
        do_action(pp, ac);
        adjust_ball_pos(pp, pp->direct + dspd);
    }

    return UNFINISHED;
}

int act_tumble(struct Player *pp, Dirspeed dspd, Speed spd)
{
    Action action = generate_action(TY_TUMBLE, dspd, spd, arbi_bspd, FR_TUMBLE);
    do_action(pp, action);
    return DONE;
}
