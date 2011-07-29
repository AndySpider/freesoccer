#include "global.h"
#include "coordinate.h"
#include "engine.h"
#include "utilities.h"
#include "actions.h"
#include "librobot.h"

Scope defend_area[22];
Scope volley_area[22];

struct Player *find_to_passer(struct Player *pp)
{
    Direction dirt, pdir = pp->direct;
    float ang, min_ang = 10.0;
    int i, id;
    struct Team *t = rbt_my_team(pp);
    float dis;
    if (t->id == 1)
    {
        for (i=0; i<11; i++)
        {
           dirt.x = match.team1.players[i].pos.x - pp->pos.x;
           dirt.y = match.team1.players[i].pos.y - pp->pos.y;
           dirt.z = match.team1.players[i].pos.z - pp->pos.z;

           ang = angle(dirt, pdir);
           if (ang == -1)        // pp himself
               continue;
           else if (ang < min_ang)
           {
               min_ang = ang;
               id = i;
               dis = distance(pp->pos, match.team1.players[i].pos);
           }
        }
        if (dis <= 30*Meter)
            return &match.team1.players[id];
    }
    if (t->id == 2)
    {
        for (i=0; i<11; i++)
        {
           dirt.x = match.team2.players[i].pos.x - pp->pos.x;
           dirt.y = match.team2.players[i].pos.y - pp->pos.y;
           dirt.z = match.team2.players[i].pos.z - pp->pos.z;

           ang = angle(dirt, pdir);
           if (ang == -1)
               continue;
           else if (ang < min_ang)
           {
               min_ang = ang;
               id = i;
               dis = distance(pp->pos, match.team2.players[i].pos);
           }
        }
        if (dis <= 30*Meter)
            return &match.team2.players[id];
    }
    return NULL;
}

int counter_team(int tid)
{
    if (tid == 0)
        return 0;
    else if (tid == 1)
        return 2;
    else if (tid == 2)
        return 1;
    return -1;
}

int which_team_hold_ball(void)  // 0|1|2
{
    struct Player *holder;
    holder = id2player(match.holder_id);
    if (holder == NULL)
    {
        return 0;
    }
    struct Team *holder_team = rbt_my_team(holder);
    return holder_team->id;
}

static void set_player_area(struct Player *pp, struct Match *mat)
{
    int pid = pp->id;
    int first_half = mat->first_half;
    Relative_area drarea, vrarea;
    Duty dy = rbt_my_duty(pp);
    
    struct Team *tm = rbt_my_team(pp);
    Tactics tc = tm->tact;
    Formation fm = tm->form;
    Position org_pos;

    if (fm == A442 && tc == BALANCED)
    {
        if (dy == FRONT)
        {
            drarea = darea_balanced_keeper_442A;
            vrarea = varea_balanced_front_442A;
        }
        if (dy == MIDDLE)
        {
            drarea = darea_balanced_middle_442A;
            vrarea = varea_balanced_middle_442A;
        }
        if (dy == BACK)
        {
            drarea = darea_balanced_back_442A;
            vrarea = varea_balanced_back_442A;
        }
        if (dy == KEEPER)
        {
            drarea = darea_balanced_keeper_442A;
            vrarea = varea_balanced_keeper_442A;
        }
    }

    if (first_half == 1)
    {
        if (pid <= 10)
        {
            org_pos = formations[tm->form][pid];
            defend_area[pid].lb.x = drarea.rlb.x + org_pos.x;
            defend_area[pid].lb.y = drarea.rlb.y + org_pos.y;
            defend_area[pid].ru.x = drarea.rru.x + org_pos.x;
            defend_area[pid].ru.y = drarea.rru.y + org_pos.y;
            volley_area[pid].lb.x = vrarea.rlb.x + org_pos.x;
            volley_area[pid].lb.y = vrarea.rlb.y + org_pos.y;
            volley_area[pid].ru.x = vrarea.rru.x + org_pos.x;
            volley_area[pid].ru.y = vrarea.rru.y + org_pos.y;
        }
        else   // 11-21
        {
            org_pos.x = Lfield - formations[tm->form][pid-11].x;
            org_pos.y = Wfield - formations[tm->form][pid-11].y;
            defend_area[pid].lb.x = org_pos.x - drarea.rru.x;
            defend_area[pid].lb.y = org_pos.y - drarea.rru.y;
            defend_area[pid].ru.x = org_pos.x - drarea.rlb.x;
            defend_area[pid].ru.y = org_pos.y - drarea.rlb.y;
            volley_area[pid].lb.x = org_pos.x - vrarea.rru.x;
            volley_area[pid].lb.y = org_pos.y - vrarea.rru.y;
            volley_area[pid].ru.x = org_pos.x - vrarea.rlb.x;
            volley_area[pid].ru.y = org_pos.y - vrarea.rlb.y;
        }
    }
    else
    {
        if (pid <= 10)
        {
            org_pos.x = Lfield - formations[tm->form][pid].x;
            org_pos.y = Wfield - formations[tm->form][pid].y;
            defend_area[pid].lb.x = org_pos.x - drarea.rru.x;
            defend_area[pid].lb.y = org_pos.y - drarea.rru.y;
            defend_area[pid].ru.x = org_pos.x - drarea.rlb.x;
            defend_area[pid].ru.y = org_pos.y - drarea.rlb.y;
            volley_area[pid].lb.x = org_pos.x - vrarea.rru.x;
            volley_area[pid].lb.y = org_pos.y - vrarea.rru.y;
            volley_area[pid].ru.x = org_pos.x - vrarea.rlb.x;
            volley_area[pid].ru.y = org_pos.y - vrarea.rlb.y;
        }
        else
        {
            org_pos = formations[tm->form][pid-11];
            defend_area[pid].lb.x = drarea.rlb.x + org_pos.x;
            defend_area[pid].lb.y = drarea.rlb.y + org_pos.y;
            defend_area[pid].ru.x = drarea.rru.x + org_pos.x;
            defend_area[pid].ru.y = drarea.rru.y + org_pos.y;
            volley_area[pid].lb.x = vrarea.rlb.x + org_pos.x;
            volley_area[pid].lb.y = vrarea.rlb.y + org_pos.y;
            volley_area[pid].ru.x = vrarea.rru.x + org_pos.x;
            volley_area[pid].ru.y = vrarea.rru.y + org_pos.y;
        }
    }
}

static void init_defend_volley_area(struct Match *mat)
{
    int i;
    struct Player p1, p2;
    struct Team t1 = mat->team1;
    struct Team t2 = mat->team2;
    for (i=0; i<11; i++)
    {
        p1 = t1.players[i];
        p2 = t2.players[i];
        set_player_area(&p1, mat);
        set_player_area(&p2, mat);
    }
}

int init_robot(struct Match *mat)
{
    init_defend_volley_area(mat);
    
    // test
    int i;
    for (i=0; i<22; i++)
    {
        printf("player[%d]:%f, %f; %f, %f\n",i, defend_area[i].lb.x, defend_area[i].lb.y, defend_area[i].ru.x, defend_area[i].ru.y);
    }
    return 0;
}
    
struct Team *rbt_my_team(struct Player *p)
{
    if (p->id <= 10)
        return &match.team1;
    else 
        return &match.team2;
}

int rbt_my_duty(struct Player *pp)
{
    Formation form;
    struct Team *t;
    t = rbt_my_team(pp);
    form = t->form;
    if (form == A442 || form == B442)
    {
        if (pp->id == 0 || pp->id == 11)
            return 0;
        else if (pp->id <= 4 || (pp->id <= 15 && pp->id > 10))
            return 1;
        else if (pp->id <= 8 || (15 < pp->id && pp->id <= 19))
            return 2;
        else if (pp->id <= 10 || (pp->id == 20 || pp->id == 21))
            return 3;
    }
    return -1;
}

Position rbt_where_ball(void)
{
    return match.ball.pos;
}

int rbt_i_have_ball(struct Player *p)
{
    if (match.holder_id == p->id)
        return 1;
    else
        return 0;
}

struct Player *rbt_who_hold_ball(void)
{
    return id2player(match.holder_id);
}

Scope rbt_my_precinct(struct Player *pp)
{
    Position org_pos;
    struct Team *t;
    Scope a;
    t = rbt_my_team(pp);
    if (t->id == 1)
        org_pos = formations[match.team1.form][pp->id];
    else if (t->id == 2)
    {
        org_pos.x = Lfield - formations[match.team2.form][pp->id-11].x;
        org_pos.y = Wfield - formations[match.team2.form][pp->id-11].y;
    }
    a.lb.x = org_pos.x - 0.17*0.5*Lfield;   // 1/2*1/3*1/2
    a.lb.y = org_pos.y - 0.17*Wfield;       // 1/2*1/3
    a.lb.z = 0;
    a.ru.x = org_pos.x + 0.17*0.5*Lfield;
    a.ru.y = org_pos.y + 0.17*Wfield;
    a.ru.z = 0;
    return a;
}

Tactics rbt_team_tactics(struct Player *pp)
{
    struct Team *t;
    t = rbt_my_team(pp);
    return t->tact;
}

float rbt_direct_difficulty(struct Player *pp, Direction dir)
{
    struct Team *my_t, *oth_t;
    int i;
    struct Player plr;
    float dif = 0;
    Direction d;
    float ang, dis;
    my_t = rbt_my_team(pp);
    if (my_t->id == 1)
        oth_t = &match.team2;
    else 
        oth_t = &match.team1;
    for (i=0; i<11; i++)
    {
        plr = oth_t->players[i];
        d = direction(pp->pos, plr.pos);
        ang = angle(d, dir);
        dis = distance(pp->pos, plr.pos);
        dif += 0.5*(1/dis)*(sin(ang) + 1);
    }
    return dif;
}

float rbt_rbt_pass_factor(struct Player *pp)
{
    Direction up = {0, 1, 0}, down = {0, -1, 0}, left = {-1, 0, 0}, right = {0, 1, 0};

    float dif_up, dif_down, dif_left, dif_right, pf;
    dif_up = rbt_direct_difficulty(pp, up);
    dif_down = rbt_direct_difficulty(pp, down);
    dif_left = rbt_direct_difficulty(pp, left);
    dif_right = rbt_direct_difficulty(pp, right);
    
    pf = 2*dif_up + dif_left + dif_right + 0.5*dif_down;
    return pf;
}

Position rbt_judge_ball_dest(struct Player *pp)
{
    float spd_value;
    Speed bspd = match.ball.spd;
    Position bpos = rbt_where_ball();
    do 
    {
        bpos.x += bspd.x;
        bpos.y += bspd.y;
        bpos.z += bspd.z;

        bspd.x *= 0.8;
        bspd.y *= 0.8;
        bspd.z *= 0.8;

        spd_value = modular(bspd);
    } while (spd_value > (0.1 * 1.0*Meter / match.nTick)); // > 0.1m/s
    return bpos;
}

int rbt_ball_flying_to_gate(Position ball_dest, int gate_num)
{
    if (gate_num == 1)
        return (left_scope(full_field, ball_dest) 
                && !left_scope(up_gate_field, ball_dest)
                && !left_scope(down_gate_field, ball_dest)
                && !above_scope(goal_field_1, ball_dest));
    else if (gate_num == 2)
        return (right_scope(full_field, ball_dest) 
                && !right_scope(up_gate_field, ball_dest)
                && !right_scope(down_gate_field, ball_dest)
                && !above_scope(goal_field_2, ball_dest));
    return -1;
}

static Position get_player_pos(Scope which_area, Position bpos, int tid, int first_half)
{
    float infinite = 10000.0;
    Position defend_pos;
    Position lb = which_area.lb;
    Position rb;
    rb.x = which_area.ru.x;
    rb.y = which_area.lb.y;
    rb.z = 0.0;
    Position ru = which_area.ru;
    Position lu;
    lu.x = which_area.lb.x;
    lu.y = which_area.ru.y;
    lu.z = 0.0;
    
    Position refer_point;
    if (tid == 2)
    {
        if (first_half)
        {
            refer_point.x = rb.x -0.2*Meter;   // avoid infinite slope
            refer_point.y = (rb.y + ru.y)/2.0;
            refer_point.z = 0.0;
        }
        else
        {
            refer_point.x = lb.x + 0.2*Meter;   // avoid infinite slope
            refer_point.y = (lb.y + lu.y)/2.0;
            refer_point.z = 0.0;
        }
    }
    else if (tid == 1)
    {
        if (first_half)
        {
            refer_point.x = lb.x + 0.2*Meter;   // avoid infinite slope
            refer_point.y = (lb.y + lu.y)/2.0;
            refer_point.z = 0.0;
        }
        else
        {
            refer_point.x = rb.x -0.2*Meter;   // avoid infinite slope
            refer_point.y = (rb.y + ru.y)/2.0;
            refer_point.z = 0.0;
        }
    }
    
    float a_l_line, b_l_line;
    float a_d_line, b_d_line;
    float a_r_line, b_r_line;
    float a_u_line, b_u_line;

    float a_refer_line, b_refer_line;

    b_l_line = infinite;                     //  |
    a_l_line = -b_l_line / lb.x;             //  |

    a_d_line = 0.0;                         //  ____
    b_d_line = lb.y;                        //  

    b_r_line = infinite;
    a_r_line = -b_r_line / rb.x;

    a_u_line = 0.0;
    b_u_line = lu.y;

    if (bpos.x != refer_point.x)
    {
        a_refer_line = (bpos.y - refer_point.y) / (bpos.x - refer_point.x);
        b_refer_line = (bpos.x*refer_point.y - refer_point.x*bpos.y) / (bpos.x - refer_point.x);
    }
    else if (tid == 1)
    {
        bpos.x -= 0.1*Meter;    // avoid infinite slope
        a_refer_line = (bpos.y - refer_point.y) / (bpos.x - refer_point.x);
        b_refer_line = (bpos.x*refer_point.y - refer_point.x*bpos.y) / (bpos.x - refer_point.x);
        /*
        a_refer_line = a_l_line;
        b_refer_line = b_l_line;
        */
    }
    else if (tid == 2)
    {
        bpos.x += 0.1*Meter;    // avoid infinite slope
        a_refer_line = (bpos.y - refer_point.y) / (bpos.x - refer_point.x);
        b_refer_line = (bpos.x*refer_point.y - refer_point.x*bpos.y) / (bpos.x - refer_point.x);
        
        /*
        a_refer_line = a_r_line;
        b_refer_line = b_r_line;
        */
    }
    
    float a_lbref_line, b_lbref_line;
    float a_rbref_line, b_rbref_line;
    float a_luref_line, b_luref_line;
    float a_ruref_line, b_ruref_line;

    if (lb.x != refer_point.x)   // should always be true
    {
        a_lbref_line = (lb.y - refer_point.y) / (lb.x - refer_point.x);
        b_lbref_line = (lb.x*refer_point.y - refer_point.x*lb.y) / (lb.x - refer_point.x);
    }
    else
    {
        a_lbref_line = a_l_line;
        b_lbref_line = b_l_line;
    }

    if (rb.x != refer_point.x)  // should always be true
    {
        a_rbref_line = (rb.y - refer_point.y) / (rb.x - refer_point.x);
        b_rbref_line = (rb.x*refer_point.y - refer_point.x*rb.y) / (rb.x - refer_point.x);
    }
    else
    {
        a_rbref_line = a_r_line;
        b_rbref_line = b_r_line;
    }

    if (ru.x != refer_point.x)  // try to make this be true by make infinite huge enough!
    {
        a_ruref_line = (ru.y - refer_point.y) / (ru.x - refer_point.x);
        b_ruref_line = (ru.x*refer_point.y - refer_point.x*ru.y) / (ru.x - refer_point.x);
    }
    else
    {
        a_ruref_line = a_r_line;
        b_ruref_line = b_r_line;
    }

    if (lu.x != refer_point.x) // try to make this be true!
    {
        a_luref_line = (lu.y - refer_point.y) / (lu.x - refer_point.x);
        b_luref_line = (lu.x*refer_point.y - refer_point.x*lu.y) / (lu.x - refer_point.x);
    }
    else
    {
        a_luref_line = a_l_line;
        b_luref_line = b_l_line;
    }
    
    if ((bpos.y <= a_lbref_line*bpos.x + b_lbref_line) && (bpos.y < a_rbref_line*bpos.x + b_rbref_line))
    {
        defend_pos.x = (b_refer_line - b_d_line) / (a_d_line - a_refer_line);
        defend_pos.y = (a_d_line*b_refer_line - a_refer_line*b_d_line) / (a_d_line - a_refer_line);
        defend_pos.z = 0.0;
    }
    else if ((bpos.y >= a_rbref_line*bpos.x + b_rbref_line) && (bpos.y < a_ruref_line*bpos.x + b_ruref_line))
    {
        defend_pos.x = (b_refer_line - b_r_line) / (a_r_line - a_refer_line);
        defend_pos.y = (a_r_line*b_refer_line - a_refer_line*b_r_line) / (a_r_line - a_refer_line);
        defend_pos.z = 0.0;
    }
    else if ((bpos.y >= a_ruref_line*bpos.x + b_ruref_line) && (bpos.y > a_luref_line*bpos.x + b_luref_line))
    {
        defend_pos.x = (b_refer_line - b_u_line) / (a_u_line - a_refer_line);
        defend_pos.y = (a_u_line*b_refer_line - a_refer_line*b_u_line) / (a_u_line - a_refer_line);
        defend_pos.z = 0.0;
    }
    else
    {
        defend_pos.x = (b_refer_line - b_l_line) / (a_l_line - a_refer_line);
        defend_pos.y = (a_l_line*b_refer_line - a_refer_line*b_l_line) / (a_l_line - a_refer_line);
        defend_pos.z = 0.0;
    }
    return defend_pos;
}

Direction direct_to_ball(struct Player *pp)
{
    Position bpos = rbt_where_ball();
    Direction dirt;
    dirt.x = bpos.x - pp->pos.x;
    dirt.y = bpos.y - pp->pos.y;
    dirt.z = bpos.z - pp->pos.z;

    return dirt;
}

void stay_in_defence(struct Player *pp)
{
    Position bpos;
    Direction dirt;
    Scope my_defend_area = defend_area[pp->id];
    bpos = rbt_where_ball();
    int tid = rbt_my_team(pp)->id;

    Position defend_pos = get_player_pos(my_defend_area, bpos, tid, match.first_half);
    dirt = direct_to_ball(pp);
    if (distance(pp->pos, defend_pos) > HOLD_DISTANCE)
        act_runto(pp, defend_pos, 2);
    else
        act_stay(pp, dirt);
}


Direction pounce_direction(Position bpos, Position bdest_pos, Position keeper_pos)
{
    Position p;
    p = intersection(bpos, bdest_pos, keeper_pos);

    printf("bpos: %f, %f, %f; bdest_pos: %f, %f, %f; keeppos: %f, %f, %f; inter: %f, %f, %f;;\n", bpos.x, bpos.y, bpos.z, bdest_pos.x, bdest_pos.y, bdest_pos.z, keeper_pos.x, keeper_pos.y, keeper_pos.z, p.x, p.y, p.z);
    Direction dirt = {p.x - keeper_pos.x, p.y - keeper_pos.y, p.z - keeper_pos.z};
    return dirt;
}

