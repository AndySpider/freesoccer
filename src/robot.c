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
    Direction dirt;
    t = rbt_my_team(pp);
    Position bpos = rbt_where_ball();
    Position bdest_pos = rbt_judge_ball_dest(pp);
    Scope my_volley_area = volley_area[pp->id];


    if (which_team_hold_ball() != t->id)
    {
        if (in_scope(my_volley_area, bpos) && !rbt_ball_flying_to_gate(bdest_pos, t->id))
        {
            if (distance(pp->pos, bpos) <= HOLD_DISTANCE)
                act_keep(pp, pp->direct);
            else
                act_runto(pp, bpos, 2);
        }
        else if (match.ball.pos.x > 60*Meter || match.ball.pos.x < 45*Meter)
        {
            if (rbt_ball_flying_to_gate(bdest_pos, t->id))
            {
                dirt = pounce_direction(bpos, bdest_pos, pp->pos);
                spd = generate_speed(dirt, 5, speed_per_power); 
                dirt = direct_to_ball(pp);
                act_pounce(pp, dirt, spd);
            }
        }
        else
        {
            stay_in_defence(pp);
        }
    }

    if (rbt_i_have_ball(pp))
    {
        dirt.x = -1.0;
        dirt.y = 0.0;
        dirt.z = 0.0;
        act_stay(pp, dirt);
        dirt.z = 1.0;
        act_shot(pp, pp->direct, generate_speed(dirt, 15, speed_per_power));
    }
}

static void rbt_back_robot(struct Player *pp, struct Match *mat)
{
    struct Team *t = rbt_my_team(pp);
    Position bpos = rbt_where_ball();
    Position topos;
    if (holder != NULL)
    {
        topos.x =  bpos.x + 10*holder->spd.x;
        topos.y =  bpos.y + 10*holder->spd.y;
        topos.z =  0.0;
    }
    else
    {
        topos.x = bpos.x + 10*mat->ball.spd.x;
        topos.y = bpos.y + 10*mat->ball.spd.y;
        topos.z = 0.0;
    }
    Direction dirt = {bpos.x - pp->pos.x, bpos.y - pp->pos.y, bpos.z - pp->pos.z};
    Scope my_volley_area = volley_area[pp->id];
    
    if (which_team_hold_ball() != t->id)
    {
        if (!in_scope(my_volley_area, bpos))
        {
            stay_in_defence(pp);
        }
        else
        {
            act_runto(pp, topos, 3);
        }
    }

    if (rbt_i_have_ball(pp))
    {
            dirt.x = -1.0;
            dirt.y = 0.0;
            dirt.z = 0.0;
            act_stay(pp, dirt);
            dirt.z = 1.0;
            act_shot(pp, pp->direct, generate_speed(dirt, 10, speed_per_power));
    }
}

static void rbt_middle_robot(struct Player *pp, struct Match *mat)
{
    struct Team *t = rbt_my_team(pp);
    Position bpos = rbt_where_ball();
    Direction dirt = direct_to_ball(pp);
    float dis = distance(pp->pos, mat->ball.pos);
    Position topos;
    if (holder != NULL)
    {
        topos.x =  bpos.x + 10*holder->spd.x;
        topos.y =  bpos.y + 10*holder->spd.y;
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
            Speed spd = generate_speed(dirt, 5, speed_per_power);
            act_shovel(pp, dirt, spd); 
        }
        else
            act_runto(pp, topos, 3);
    }

    if (rbt_i_have_ball(pp))
    {
            dirt.x = -1.0;
            dirt.y = 0.0;
            dirt.z = 0.0;
            act_stay(pp, dirt);
            dirt.z = 1.0;
            act_shot(pp, pp->direct, generate_speed(dirt, 10, speed_per_power));
    }
}

static void rbt_front_robot(struct Player *pp, struct Match *mat)
{
    struct Team *t = rbt_my_team(pp);
    Position bpos = rbt_where_ball();
    Position topos;
    if (holder != NULL)
    {
        topos.x =  bpos.x + 10*holder->spd.x;
        topos.y =  bpos.y + 10*holder->spd.y;
        topos.z =  0.0;
    }
    else
    {
        topos.x = bpos.x + 10*mat->ball.spd.x;
        topos.y = bpos.y + 10*mat->ball.spd.y;
        topos.z = 0.0;
    }
    Direction dirt;
    Scope my_volley_area = volley_area[pp->id];
    
   if (which_team_hold_ball() != t->id)
   {
        if (!in_scope(my_volley_area, bpos))
        {
            stay_in_defence(pp);
        }
        else
        {
            act_runto(pp, topos, 3);
        }
    }

    if (rbt_i_have_ball(pp))
    {
        if (pp->pos.x <= 32*Meter)
        {
            dirt = pp->direct;
            dirt.z = sqrt(dirt.x*dirt.x + dirt.y*dirt.y);
            Speed spd = generate_speed(dirt , 10, speed_per_power);
            act_shot(pp, dirt, spd);
        }
        else
        {
            dirt.x = -1.0;
            dirt.y = 0.0;
            dirt.z = 0.0;
            Speed spd = generate_speed(dirt , 3, speed_per_power);
            act_dribble(pp, dirt, spd);
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
