#include "global.h"
#include "engine.h"
#include "coordinate.h"
#include "librobot.h"
#include "actions.h"
#include "utilities.h"

static void rbt_keeper_robot(struct Player *pp, struct Match *mat)
{
    struct Team *t;
    Speed spd;
    Dirspeed dspd;
    Angle agl;
    struct Vector vec;
    t = rbt_my_team(pp);
    Position bpos = rbt_where_ball();
    Position bdest_pos = rbt_judge_ball_dest(pp);
    Scope my_volley_area = volley_area[pp->id];


    if (which_team_hold_ball() != t->id)
    {
        if (in_scope(my_volley_area, bpos) && !rbt_ball_flying_to_gate(bdest_pos, t->id))
        {
            if (distance(pp->pos, bpos) <= HOLD_DISTANCE)
            {
                agl = direct_diff(pp->direct, direct_to_ball(pp)); 
                dspd = generate_dirspd(agl, 180, dirspd_per_power);
                act_keep(pp, dspd);
            }
            else
                act_runto(pp, direct_to_ball(pp), bpos, 180, 6);
        }
        else if (match.ball.pos.x > 60*Meter || match.ball.pos.x < 45*Meter)
        {
            if (rbt_ball_flying_to_gate(bdest_pos, t->id))
            {
                vec = pounce_vector(bpos, bdest_pos, pp->pos);
                spd = generate_speed(vec, 7, speed_per_power); 
                agl = direct_diff(pp->direct, direct_to_ball(pp));
                dspd = generate_dirspd(agl, 180, dirspd_per_power);
                act_pounce(pp, dspd, spd);
            }
        }
        else
        {
            stay_in_defence(pp);
        }
    }
    else
        stay_in_defence(pp);

    if (rbt_i_have_ball(pp))
    {
        vec.x = -1.0 * INFINITE;
        vec.y = 0.0;
        vec.z = 0.0;
        agl = direct_diff(pp->direct, vector2direct(vec));
        dspd = generate_dirspd(agl, 180, dirspd_per_power);
        vec.z = 0.5 * INFINITE;
        act_shot(pp, dspd, generate_speed(match.ball.spd, 5, speed_per_power), generate_speed(vec, 20, speed_per_power));
    }
}

static void rbt_back_robot(struct Player *pp, struct Match *mat)
{
    struct Team *t = rbt_my_team(pp);
    Angle agl;
    Dirspeed dspd;
    Position bpos = rbt_where_ball();
    Position topos;
    if (holder != NULL)
    {
        topos.x =  bpos.x + 10*holder->action.spd.x;
        topos.y =  bpos.y + 10*holder->action.spd.y;
        topos.z =  0.0;
    }
    else
    {
        topos.x = bpos.x + 10*mat->ball.spd.x;
        topos.y = bpos.y + 10*mat->ball.spd.y;
        topos.z = 0.0;
    }
    struct Vector vec = {bpos.x - pp->pos.x, bpos.y - pp->pos.y, bpos.z - pp->pos.z};
    Scope my_volley_area = volley_area[pp->id];
    
    if (which_team_hold_ball() != t->id)
    {
        if (!in_scope(my_volley_area, bpos))
        {
            stay_in_defence(pp);
        }
        else
        {
            act_runto(pp, direct_to_ball(pp), topos, 180, 4);
        }
    }
    else
    {
        agl = direct_diff(pp->direct, direct_to_ball(pp));
        dspd = generate_dirspd(agl, 180, dirspd_per_power);
        act_stay(pp, dspd);
    }

    if (rbt_i_have_ball(pp))
    {
            vec.x = -1.0 * INFINITE;
            vec.y = 0.0;
            vec.z = 0.0;
            agl = direct_diff(pp->direct, vector2direct(vec));
            dspd = generate_dirspd(agl, 180, dirspd_per_power);
            act_hold(pp, dspd);
            vec.z = 1.0 * INFINITE;
            act_shot(pp, dspd, generate_speed(match.ball.spd, 5, speed_per_power), generate_speed(vec, 15, speed_per_power));
    }
}

static void rbt_middle_robot(struct Player *pp, struct Match *mat)
{
    struct Team *t = rbt_my_team(pp);
    Position bpos = rbt_where_ball();
    struct Vector vec = vector(pp->pos, mat->ball.pos);
    Angle agl = direct_diff(pp->direct, direct_to_ball(pp));
    Dirspeed dspd;
    Speed spd;
    float dis = distance(pp->pos, mat->ball.pos);
    Position topos;
    if (holder != NULL)
    {
        topos.x =  bpos.x + 10*holder->action.spd.x;
        topos.y =  bpos.y + 10*holder->action.spd.y;
        topos.z =  0.0;
    }
    else
    {
        topos.x = bpos.x + 10*mat->ball.spd.x;
        topos.y = bpos.y + 10*mat->ball.spd.y;
        topos.z = 0.0;
    }
    Scope my_volley_area = volley_area[pp->id];
    
    if (which_team_hold_ball() != t->id)
    {
        if (!in_scope(my_volley_area, bpos))
        {
            stay_in_defence(pp);
        }
        else if (dis <= 4*Meter)
        {

            dspd = generate_dirspd(agl, 180, dirspd_per_power);
            spd = generate_speed(vec, 5, speed_per_power);
            act_shovel(pp, dspd, spd, spd); 
        }
        else
            act_runto(pp, direct_to_ball(pp), topos, 180, 3);
    }
    else
        act_stay(pp, generate_dirspd(agl, 180, dirspd_per_power));

    if (rbt_i_have_ball(pp))
    {
            vec.x = -1.0 * INFINITE;
            vec.y = 0.0;
            vec.z = 0.0;
            agl = direct_diff(pp->direct, vector2direct(vec));
            dspd = generate_dirspd(agl, 180, dirspd_per_power);
            act_hold(pp, dspd);
            vec.z = 1.0 * INFINITE;
            act_shot(pp, dspd, generate_speed(match.ball.spd, 5, speed_per_power), generate_speed(vec, 13, speed_per_power));
    }
}

static void rbt_front_robot(struct Player *pp, struct Match *mat)
{
    struct Team *t = rbt_my_team(pp);
    Position bpos = rbt_where_ball();
    Position topos;
    Angle agl;
    Dirspeed dspd;
    if (holder != NULL)
    {
        topos.x =  bpos.x + 10*holder->action.spd.x;
        topos.y =  bpos.y + 10*holder->action.spd.y;
        topos.z =  0.0;
    }
    else
    {
        topos.x = bpos.x + 10*mat->ball.spd.x;
        topos.y = bpos.y + 10*mat->ball.spd.y;
        topos.z = 0.0;
    }
    struct Vector vec;
    Scope my_volley_area = volley_area[pp->id];
    
   if (which_team_hold_ball() != t->id)
   {
        if (!in_scope(my_volley_area, bpos))
        {
            stay_in_defence(pp);
        }
        else
        {
            act_runto(pp, direct_to_ball(pp), topos, 180, 3);
        }
    }
    else
    {
        agl = direct_diff(pp->direct, direct_to_ball(pp));
        dspd = generate_dirspd(agl, 180, dirspd_per_power);
        act_stay(pp, dspd);
    }

    if (rbt_i_have_ball(pp))
    {
        if (pp->pos.x <= 36*Meter)
        {
            vec = direct2vector(pp->direct, INFINITE);
            vec.z = sqrt(vec.x*vec.x + vec.y*vec.y);
            Speed spd = generate_speed(vec , 15, speed_per_power);
            agl = direct_diff(pp->direct, vector2direct(vec));
            dspd = generate_dirspd(agl, 180, dirspd_per_power);
            act_shot(pp, dspd, generate_speed(match.ball.spd, 5, speed_per_power), spd);
        }
        else
        {
            vec.x = -1.0 * INFINITE;
            vec.y = 0.0;
            vec.z = 0.0;
            agl = direct_diff(pp->direct, vector2direct(vec));
            dspd = generate_dirspd(agl, 180, dirspd_per_power);
            Speed spd = generate_speed(vec , 3, speed_per_power);
            Speed bspd = multiply(spd, 1.2);
            act_dribble(pp, dspd, spd, bspd);
        }
    }

}
    
void rbt_robot(struct Player *pp, struct Match *mat)
{
    Duty dy;
    dy = rbt_my_duty(pp);
    if (dy == KEEPER)
        rbt_keeper_robot(pp, mat);
    else if (dy == BACK)
        rbt_back_robot(pp, mat);
    else if (dy == MIDDLE)
        rbt_middle_robot(pp, mat);
    else if (dy == FRONT)
        rbt_front_robot(pp, mat);
    return;
}
