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
#include "comm.h"
#include "coordinate.h"
#include "librobot.h"
#include "utilities.h"
#include "actions.h"
#include "robot.h"

int sock;
struct Cmd c;
struct Status sts;
struct Exchange exch;

struct Player *focuser;
struct Player *holder;
struct Player *topasser;

float timer = 0.0;  // seconds
int ticks = 0;

static void update_timer(void)
{
    ticks += 1;
    timer = ticks*1.0 / match.nTick;
}

static int find_holder(void)
{
    float dis, min = Lfield+10;
    int i, id;
    //int old_holder = match.holder_id;
    for (i=0; i<11; i++)
    {
        dis = distance(match.team1.players[i].pos, match.ball.pos);
        if (dis < min)
        {
            min = dis;
            id = i;
        }
    }
    for (i=0; i<11; i++)
    {
        dis = distance(match.team2.players[i].pos, match.ball.pos);
        if (dis < min)
        {
            min = dis;
            id = 11 + i;
        }
    }
    if ( min <= HOLD_DISTANCE )
        return id;
    else
        return -1;
}

static int find_focuser(void)
{
   float dis, min = Lfield+10;
   int i, id = -1;

   if (match.human == 1)
   {
        for (i=0; i<11; i++)
        {
            dis = distance(match.team1.players[i].pos, match.ball.pos);
            if (dis < min)
            {
                min = dis;
                id = i;
            }
        }
   }
   else if (match.human == 2)
   {
        for (i=0; i<11; i++)
        {
            dis = distance(match.team2.players[i].pos, match.ball.pos);
            if (dis < min)
            {
                min = dis;
                id = 11 + i;
            }
        }
   }
   return id;
}

static void init_players_pos(struct Match *mat)
{
    int i;
    for (i=0; i< 11; i++)
    {
        if (mat->first_half == 1)
        {
            mat->team1.players[i].pos = formations[mat->team1.form][i];
            mat->team2.players[i].pos.x = Lfield - formations[mat->team2.form][i].x;
            mat->team2.players[i].pos.y = Wfield - formations[mat->team2.form][i].y;
        }
        else if (mat->first_half == 0)
        {
            mat->team2.players[i].pos = formations[mat->team2.form][i];
            mat->team1.players[i].pos.x = Lfield - formations[mat->team1.form][i].x;
            mat->team1.players[i].pos.y = formations[mat->team1.form][i].y;
        }

        mat->team1.players[i].direct = direct_to_ball(&mat->team1.players[i]);
        mat->team2.players[i].direct = direct_to_ball(&mat->team2.players[i]);
    }
}

/* Generate positions of players and ball according to specfic events */
static void generate_pos(Event_ID eid)
{
    struct Event evt = match.evt;
    struct Player *kick_player;
    if (eid == PENALTY)
    {
        if ( evt.which_team == 1)
        {
            if (match.first_half == 1)
            {
                match.ball.pos.x = penalty_point_1.x;
                match.ball.pos.y = penalty_point_1.y;
                match.ball.pos.z = penalty_point_1.z;
                match.ball.spd.x = 0.0;
                match.ball.spd.x = 0.0;
                match.ball.spd.z = 0.0;
                kick_player = &match.team2.players[10];
                init_players_pos(&match);
                kick_player->pos.x = match.ball.pos.x + 0.5*Meter;
                kick_player->pos.y = match.ball.pos.y;
                kick_player->pos.z = match.ball.pos.z;
            }
            else if (match.first_half == 0)
            {
                match.ball.pos.x = penalty_point_2.x;
                match.ball.pos.y = penalty_point_2.y;
                match.ball.pos.z = penalty_point_2.z;
                match.ball.spd.x = 0.0;
                match.ball.spd.x = 0.0;
                match.ball.spd.z = 0.0;
                kick_player = &match.team2.players[10];
                init_players_pos(&match);
                kick_player->pos.x = match.ball.pos.x - 0.5*Meter;
                kick_player->pos.y = match.ball.pos.y;
                kick_player->pos.z = match.ball.pos.z;
            }
        }
        else if ( evt.which_team == 2)
        {
            if (match.first_half == 1)
            {
                match.ball.pos.x = penalty_point_2.x;
                match.ball.pos.y = penalty_point_2.y;
                match.ball.pos.z = penalty_point_2.z;
                match.ball.spd.x = 0.0;
                match.ball.spd.x = 0.0;
                match.ball.spd.z = 0.0;
                kick_player = &match.team1.players[10];
                init_players_pos(&match);
                kick_player->pos.x = match.ball.pos.x - 0.5*Meter;
                kick_player->pos.y = match.ball.pos.y;
                kick_player->pos.z = match.ball.pos.z;
            }
            else if (match.first_half == 0)
            {
                match.ball.pos.x = penalty_point_1.x;
                match.ball.pos.y = penalty_point_1.y;
                match.ball.pos.z = penalty_point_1.z;
                match.ball.spd.x = 0.0;
                match.ball.spd.x = 0.0;
                match.ball.spd.z = 0.0;
                kick_player = &match.team1.players[10];
                init_players_pos(&match);
                kick_player->pos.x = match.ball.pos.x + 0.5*Meter;
                kick_player->pos.y = match.ball.pos.y;
                kick_player->pos.z = match.ball.pos.z;
            }
        }
    }
    else if (eid == FREEKICK)
    {
        if ( evt.which_team == 1)
        {
            match.ball.pos = evt.epos;
            match.ball.spd.x = 0.0;
            match.ball.spd.x = 0.0;
            match.ball.spd.z = 0.0;
            kick_player = &match.team2.players[10];
            init_players_pos(&match);
            kick_player->pos.x = match.ball.pos.x + 0.5*Meter;
            kick_player->pos.y = match.ball.pos.y;
            kick_player->pos.z = match.ball.pos.z;
        }
        else if ( evt.which_team == 2)
        {
            match.ball.pos = evt.epos;
            match.ball.spd.x = 0.0;
            match.ball.spd.x = 0.0;
            match.ball.spd.z = 0.0;
            kick_player = &match.team1.players[10];
            init_players_pos(&match);
            kick_player->pos.x = match.ball.pos.x - 0.5*Meter;
            kick_player->pos.y = match.ball.pos.y;
            kick_player->pos.z = match.ball.pos.z;
        }
        //printf("FREEKICK positions unimplemented\n");
    }
    else if (eid == CORNER)
    {
        printf("CORNER positions unimplemented\n");
    }
    else if (eid == GOAL)
    {
        match.ball.pos.x = center_point.x;
        match.ball.pos.y = center_point.y;
        match.ball.pos.z = center_point.z;
        match.ball.spd.x = 0.0;
        match.ball.spd.x = 0.0;
        match.ball.spd.z = 0.0;
        init_players_pos(&match);
        if (evt.which_team == 1)
        {
            match.team2.players[10].pos.x = match.ball.pos.x;
            match.team2.players[10].pos.y = match.ball.pos.y + 0.2*Meter;
            match.team2.players[9].pos.x  = match.ball.pos.x;
            match.team2.players[9].pos.y  = match.ball.pos.y - 1*Meter;
        }
        else if (evt.which_team == 2)
        {
            match.team1.players[10].pos.x = match.ball.pos.x;
            match.team1.players[10].pos.y = match.ball.pos.y + 0.2*Meter;
            match.team1.players[9].pos.x  = match.ball.pos.x;
            match.team1.players[9].pos.y  = match.ball.pos.y - 1*Meter;
        }
    }
    else if (eid == OFFSIDE)
    {
        printf("OFFSIDE positions unimplemented\n");
    }
    else if (eid == THROWIN)
    {
        match.ball.pos.x = evt.epos.x;
        match.ball.pos.y = evt.epos.y;
        match.ball.pos.z = 0.0;
        match.ball.spd.x = 0.0;
        match.ball.spd.x = 0.0;
        match.ball.spd.z = 0.0;
        if (evt.which_team == 1)
        {
            match.team2.players[10].pos.x = match.ball.pos.x;
            match.team2.players[10].pos.y = match.ball.pos.y -0.2*Meter;
            match.team2.players[10].pos.z = match.ball.pos.z;
        }
        else if (evt.which_team == 2)
        {
            match.team1.players[10].pos.x = match.ball.pos.x;
            match.team1.players[10].pos.y = match.ball.pos.y - 0.2*Meter;
            match.team1.players[10].pos.z = match.ball.pos.z;
        }
    }
    else if (eid == TEEBALL)
    {
        printf("TEEBALL positions unimplemented\n");
    }
}

static void clear_event(void)
{
    match.evt.eid = NONE;
}

/* New event detected, do call this function to set */
static void set_event(struct Event new_evt)
{
    struct Event c_evt = match.evt;
    if (c_evt.disabled == 1)
        return;
    else
    {
        match.evt = new_evt;
        match.evt.disabled = 1;
    }
}

static int process_evt(void)
{
    struct Event evt = match.evt;
    switch (evt.eid)
    {
        case PENALTY:
            printf("Penalty\n");
            generate_pos(PENALTY);
            break;
        case FREEKICK:
            printf("Freekick\n");
            //generate_pos(FREEKICK);
            break;
        case CORNER:
            printf("Corner\n");
            generate_pos(CORNER);
            break;
        case GOAL:
            printf("Goal\n");
            generate_pos(GOAL);
            break;
        case OFFSIDE:
            printf("Offside\n");
            generate_pos(OFFSIDE);
            break;
        case THROWIN:
            printf("Throwin\n");
            generate_pos(THROWIN);
            break;
        case TEEBALL:
            printf("Teeball\n");
            generate_pos(TEEBALL);
            break;
        default:
            return -1;
    }
    clear_event();
    return 0;
}

static void set_status(struct Status *sts)
{
    int i;
    sts->ball = match.ball;
    for (i=0; i<11; i++)
    {
        sts->players[i] = match.team1.players[i];
        sts->players[11+i] = match.team2.players[i];
    }
    sts->holder_id = match.holder_id;
    sts->focus_id = match.focus_id;
    sts->evt = match.evt;
}

int diff_team(struct Player p1, struct Player p2)
{
    if (((float)p1.id-10.5)*((float)p2.id-10.5) < 0)
        return 1;
    else
        return 0;
}

struct Player *id2player(int id)
{
    if (id == -1)
        return NULL;
    if (id <= 10)
        return &match.team1.players[id];
    else if (id > 10)
        return &match.team2.players[id-11];
    return NULL;
}

/* Put players' speed, dirspd into action */
static int players_act(void)
{
    int i;
    for (i=0; i<11; i++)
    {
        // position
        if (match.team1.players[i].action.spd.z > 0.0) 
            match.team1.players[i].action.spd.z -= (9.8*Meter/(match.nTick*match.nTick));

        if (match.team2.players[i].action.spd.z > 0.0) 
            match.team2.players[i].action.spd.z -= (9.8*Meter/(match.nTick*match.nTick));

        match.team1.players[i].pos = add_vector(match.team1.players[i].pos, match.team1.players[i].action.spd);
        if (match.team1.players[i].pos.z < 0.0)
            match.team1.players[i].pos.z = 0.0;

        match.team2.players[i].pos = add_vector(match.team2.players[i].pos, match.team2.players[i].action.spd);
        if (match.team2.players[i].pos.z < 0.0)
            match.team2.players[i].pos.z = 0.0;

        // direction
        match.team1.players[i].direct += match.team1.players[i].action.dspd;
        match.team1.players[i].direct = relocate(match.team1.players[i].direct); 

        match.team2.players[i].direct += match.team2.players[i].action.dspd;
        match.team2.players[i].direct = relocate(match.team2.players[i].direct); 
    }
    return 0;
}

/* Detect collisions between players */
static int detect_collision(void)
{
    int i, j;
    float dis;
    for (i=0; i<11; i++)
    {
        for (j=0; j<11; j++)
        {
            dis = distance(match.team1.players[i].pos, match.team2.players[j].pos);
            if (dis <= HOLD_DISTANCE)
            {
                if (match.team1.players[i].action.ac_type == TY_SHOVEL)
                {
                    if (fabsf(direct_diff(match.team1.players[i].direct, match.team2.players[j].direct)) < PI/2)
                    {
                        struct Event nevt = {FREEKICK, 1, match.team2.players[j].pos};
                        set_event(nevt);
                    }
                    struct Vector vec = direct2vector(match.team2.players[j].direct, INFINITE);
                    Speed spd = generate_speed(vec, 1, speed_per_power);
                    act_tumble(&match.team2.players[j], 0.0, spd);
                }
                else if (match.team2.players[j].action.ac_type == TY_SHOVEL)
                {
                    if (fabsf(direct_diff(match.team2.players[j].direct, match.team1.players[i].direct)) < PI/2)
                    {
                        struct Event nevt = {FREEKICK, 2, match.team1.players[i].pos};
                        set_event(nevt);
                    }
                    struct Vector vec = direct2vector(match.team1.players[i].direct, INFINITE);
                    Speed spd = generate_speed(vec, 1, speed_per_power);
                    act_tumble(&match.team1.players[i], 0.0, spd);
                }
                else if (modular(match.team1.players[i].action.spd) != 0 && modular(match.team2.players[i].action.spd) != 0.0)
                {
                    if (match.team1.players[i].attr.body < match.team2.players[j].attr.body)
                    {
                        struct Vector vec = direct2vector(match.team1.players[i].direct, INFINITE);
                        Speed spd = generate_speed(vec, 0.7, speed_per_power);
                        act_tumble(&match.team1.players[i], 0.0, spd);
                    }
                    if (match.team1.players[i].attr.body > match.team2.players[j].attr.body)
                    {
                        struct Vector vec = direct2vector(match.team2.players[j].direct, INFINITE);
                        Speed spd = generate_speed(vec, 0.7, speed_per_power);
                        act_tumble(&match.team2.players[j], 0.0, spd);
                    }
                }
            }
        }
    }
    return 0;
}

static int prepare4starting(struct Match *mat)
{
    mat->ball.pos.x = 0.5*Lfield;
    mat->ball.pos.y = 0.5*Wfield;
    mat->ball.pos.z = 0.0;
    mat->ball.spd.x = 0.0;
    mat->ball.spd.y = 0.0;
    mat->ball.spd.z = 0.0;
    init_players_pos(mat);
    //2. 初始化完毕，报告GUI
    strcpy(c.name,"READY");
    send_cmd(sock, &c);
    //3. 等待GUI的启动TICK……
    recv_cmd(sock, &c);
    //4. 收到启动TICK，启动开球画面，等待开球……
    if (com_start(c))
    {
        set_status(&sts);
        send_status(sock, &sts);
    }
    return 0;
}

static void none_action(struct Player *pp)
{
    if (i_own_ball(pp))
        act_hold(pp, 0.0);
    else
        act_stay(pp, 0.0);
}

static void move_to_ball(struct Player *pp, int spower)
{
    float dis = distance(pp->pos, match.ball.pos);
    if (dis <= HOLD_DISTANCE)
        act_short_pass(pp);
    else
        act_runto(pp, direct_to_ball(pp), match.ball.pos, 180, spower);
}

static void long_pass(struct Player *pp, int spower)
{
    struct Vector vec = direct2vector(pp->direct, INFINITE);
    vec.z = sqrt(vec.x*vec.x + vec.y*vec.y)/2.0;
    Speed bspd;
    bspd = generate_speed(vec, spower, speed_per_power);
    if (act_runto(pp, direct_to_ball(pp), match.ball.pos, 180, 10) == DONE)
        act_kick(pp, 0.0, generate_speed(match.ball.spd, 5, speed_per_power), bspd);

}

static void run_up(struct Player *pp, int spower)
{
    Speed spd;
    Dirspeed dirspd;
    Direction up = PI/2.0;
    Angle agl = direct_diff(pp->direct, up);
    dirspd = generate_dirspd(agl, 180, dirspd_per_power);
    struct Vector vec_up = {0, INFINITE, 0};
    spd = generate_speed(vec_up, spower, speed_per_power);
    Speed ballspd = multiply(spd, 1.2);
    if (act_dribble(pp, dirspd, spd, ballspd) == IMPOSSIBLE)
        act_run(pp, dirspd, spd);
}

static void run_down(struct Player *pp, int spower)
{
    Speed spd;
    Direction down = -PI/2.0;
    Angle agl = direct_diff(pp->direct, down);
    Dirspeed dirspd = generate_dirspd(agl, 180, dirspd_per_power);
    struct Vector vec_down = {0, -INFINITE, 0};
    spd = generate_speed(vec_down, spower, speed_per_power);
    Speed ballspd = multiply(spd, 1.2);
    if (act_dribble(pp, dirspd, spd, ballspd) == IMPOSSIBLE)
        act_run(pp, dirspd, spd);
}

static void run_left(struct Player *pp, int spower)
{
    Speed spd;
    Direction left = PI;
    Angle agl = direct_diff(pp->direct, left);
    Dirspeed dirspd = generate_dirspd(agl, 180, dirspd_per_power);
    struct Vector vec_left = {-INFINITE, 0, 0};
    spd = generate_speed(vec_left, spower, speed_per_power);
    Speed ballspd = multiply(spd, 1.2);
    if (act_dribble(pp, dirspd, spd, ballspd) == IMPOSSIBLE)
        act_run(pp, dirspd, spd);
}

static void run_right(struct Player *pp, int spower)
{
    Speed spd;
    Direction right = 0.0;
    Angle agl = direct_diff(pp->direct, right);
    Dirspeed dirspd = generate_dirspd(agl, 180, dirspd_per_power);
    struct Vector vec_right = {INFINITE, 0, 0};
    spd = generate_speed(vec_right, spower, speed_per_power);
    Speed ballspd = multiply(spd, 1.2);
    if (act_dribble(pp, dirspd, spd, ballspd) == IMPOSSIBLE)
        act_run(pp, dirspd, spd);
}

static int action_on_cmd(struct Cmd command)
{
    if (focuser == NULL)
        return 0;

    if ( com_move_up(command) )   //strcmp(command.name, "W") == 0)
        run_up(focuser, command.power );   //command.power);
    else if (com_move_down(command)) // strcmp(command.name, "S") == 0)
        run_down(focuser, command.power);
    else if (com_move_left(command))//strcmp(command.name, "A") == 0)
        run_left(focuser, command.power);
    else if (com_move_right(command)) //strcmp(command.name, "D") == 0)
        run_right(focuser, command.power);
    else if (com_shot(command)) //strcmp(command.name, "SPACE") == 0)
    {
        if (i_own_ball(focuser))
        {
            struct Vector vec = {Lfield - 2*Meter - focuser->pos.x, Wfield/2.0 - focuser->pos.y, 0.0};
            vec.z = sqrt(vec.x*vec.x + vec.y*vec.y)/2.0;
            Speed bspd = generate_speed(vec, command.power, speed_per_power);
            Angle agl = direct_diff(focuser->direct, vector2direct(vec));
            Dirspeed dspd = generate_dirspd(agl, 180, dirspd_per_power);
            if (act_runto(focuser, direct_to_ball(focuser), match.ball.pos, 180, 10) == DONE)
                act_shot(focuser, dspd, generate_speed(match.ball.spd, 5, speed_per_power), bspd);
        }
        else
        {
            // shovel
            Angle agl = direct_diff(focuser->direct, direct_to_ball(focuser));
            Dirspeed dspd = generate_dirspd(agl, 180, dirspd_per_power);
            struct Vector vec = vector(focuser->pos, match.ball.pos);
            Speed spd = generate_speed(vec, 8, speed_per_power);
            Speed bspd = multiply(spd, 1.4);
            act_shovel(focuser, dspd, spd, bspd);
        }
    }
    else if (com_long_pass(command)) //strcmp(command.name, "L") == 0)
        long_pass(focuser, command.power);
    else if (com_none(command)) //strcmp(command.name, "NONE") == 0)
        none_action(focuser);
    else if (com_move_to_ball(command)) //strcmp(command.name, "K") == 0)
        move_to_ball(focuser, command.power);
    else 
        act_stay(focuser, focuser->direct);

    return 0;
}
        
/* For uncontrolable players, using inside robots to decide next actions */
static int call_robot(void)
{
    int i;
    for (i=0; i<11; i++)
    {
        if (focuser == NULL || match.team1.players[i].id != focuser->id)
            rbt_robot(&match.team1.players[i], &match);
        if (focuser == NULL || match.team2.players[i].id != focuser->id)
            rbt_robot(&match.team2.players[i], &match);
    }
    return 0;
}

static int team_id(struct Player *pp)
{
    if (pp == NULL)
        return -1;

    if (match.human == 1)
    {
        if (pp->id <= 10)
            return 1;
        else if (pp->id > 10)
            return 2;
    }
    else if (match.human == 2)
    {
        if (pp->id <= 10)
            return 2;
        else if (pp->id > 10)
            return 1;
    }
    return -1;
}

static void enable_event(void)
{
    match.evt.disabled = 0;
    return;
}

/* TODO: this function didn't take first_half into consideration */
static Event_ID detect_ball_event(void)
{
    Position bpos;
    bpos = match.ball.pos;
    int tid;
    tid = team_id(match.ball.last_toucher);
    struct Event new_evt;
    new_evt.eid = NONE;

    if (in_scope(full_field, bpos))
        return match.evt.eid;

    else if (up_down_scope(full_field, bpos))
    {
        printf("THROWIN!\n");
        new_evt.eid = THROWIN;
        new_evt.which_team = team_id(match.ball.last_toucher);
        new_evt.epos = bpos;
        new_evt.epos.z = 0;
    }
    else if (left_scope(full_field, bpos))
    {
        if (left_scope(up_gate_field, bpos) || left_scope(down_gate_field, bpos) || above_scope(goal_field_1, bpos))
        {
            if (tid == 1)
            {
                printf("CORNER!\n");
                new_evt.eid = CORNER;
                new_evt.which_team = 1;
                new_evt.epos = bpos;
                new_evt.epos.z = 0;
            }
            else if (tid == 2)
            {
                printf("Missing shot!\n");
                new_evt.eid = TEEBALL;
                new_evt.which_team = 1;
                new_evt.epos = bpos;
                new_evt.epos.z = 0;
            }
        }
        else
        {
            printf("GOAL2!\n");
            match.team2.score++;
            new_evt.eid = GOAL;
            new_evt.which_team = 2;
            new_evt.epos = bpos;
            new_evt.epos.z = 0;
        }
    }
    else if (right_scope(full_field, bpos))
    {
        if (right_scope(up_gate_field, bpos) || right_scope(down_gate_field, bpos) || above_scope(goal_field_2, bpos))
        {
            if (tid == 2)
            {
                printf("CORNER!\n");
                new_evt.eid = CORNER;
                new_evt.which_team = 2;
                new_evt.epos = bpos;
                new_evt.epos.z = 0;
            }
            else if (tid == 1)
            {
                printf("Missing shot!\n");
                new_evt.eid = TEEBALL;
                new_evt.which_team = 2;
                new_evt.epos = bpos;
                new_evt.epos.z = 0;
            }
        }
        else
        {
            printf("GOAL1!\n");
            match.team1.score++;
            new_evt.eid = GOAL;
            new_evt.which_team = 1;
            new_evt.epos = bpos;
            new_evt.epos.z = 0;
        }
    }
    if (new_evt.eid != NONE)
        set_event(new_evt);
    return new_evt.eid;
}

static void init_match(struct Match *mat, struct Init_data *idata)
{
    mat->dif = idata->dif;
    mat->weath = idata->weath;
    mat->nTick = idata->nTick;
    mat->team1 = idata->team1;
    mat->team2 = idata->team2;
    mat->team1.players = malloc(11*sizeof(struct Player));
    mat->team2.players = malloc(11*sizeof(struct Player));
    memcpy(mat->team1.players, idata->player1, 11*sizeof(struct Player));
    memcpy(mat->team2.players, idata->player2, 11*sizeof(struct Player));
    mat->human = idata->human;
    mat->first_half = 1;

    mat->ball.pos.x = 0.5*Lfield;
    mat->ball.pos.y = 0.5*Wfield;
    mat->holder_id = -1;
    //mat->holder_id = find_holder(); 
    holder = NULL; //id2player(mat->holder_id);
    mat->focus_id = -1;
    //mat->focus_id = find_focuser();
    focuser = NULL; //id2player(mat->focus_id);
    mat->evt.eid = NONE;
    mat->evt.disabled = 0;

    return;
}

static void clean_match(struct Match *mat)
{
    free(mat->team1.players);
    free(mat->team2.players);
    return;
}


static void clean_up(void)
{
    destroy_server_sock(sock);
    clean_match(&match);
}

static float maxin2(float a, float b)
{
    if (fabsf(a) > fabsf(b))
        return a;
    else
        return b;
}

static Speed add_speed(Speed spd1, Speed spd2)
{
    float x;
    if (spd1.x * spd2.x <= 0)
        x = spd1.x + spd2.x;
    else
        x = maxin2(spd1.x, spd2.x);
    float y;
    if (spd1.y * spd2.y <= 0)
        y = spd1.y + spd2.y;
    else
        y = maxin2(spd1.y, spd2.y);
    float z;
    if (spd1.z * spd2.z <= 0)
        z = spd1.z + spd2.z;
    else
        z = maxin2(spd1.z, spd2.z);
    Speed re = {x, y, z};
    return re;
}

void change_ball_speed(Speed added_spd, struct Vector norm, float ref_factor)
{
    if (angle(match.ball.spd, norm) <= PI/2.0)
    {
        match.ball.spd = add_speed(match.ball.spd, added_spd);
    }
    else
    {
        struct Vector ref_vec = reflecting(match.ball.spd, norm);
        struct Vector left_spd = multiply(ref_vec, -ref_factor);
        match.ball.spd = add_speed(left_spd, added_spd);
    }
}

Angle direct_to_ball(struct Player *pp)
{
    struct Vector vec = vector(pp->pos, match.ball.pos);
    return vector2direct(vec);
}

int main(void)
{
    //1. 等待GUI传来的初始化数据，初始化内部数据结构
    sock = init_server_sock();
//newmatch:
    struct Init_data *idata = malloc(sizeof(struct Init_data));
    recv_init_data(sock, idata);
    init_match(&match, idata);
    free(idata);

restart:
    init_robot(&match);
    //2. 初始化完毕，报告GUI
    //3. 等待GUI的启动TICK……
    //4. 收到启动TICK，启动开球画面，等待开球……

    prepare4starting(&match);
    //5. 等待GUI传来的数据
    
    while(1)
    {
        recv_cmd(sock, &c);
        update_timer();
        if (com_end(c))
            break;
        else if (com_restart(c))
        {
            match.first_half = 1;
            match.human = 1;
            goto restart;
        }
        else if (com_half(c))
        {
            printf("Half time\n");
            struct Team tmp;
            tmp = match.team1;
            match.team1 = match.team2;
            match.team2 = tmp;
            match.human = (match.human == 1) ? 2 : 1;
            match.first_half = 0;
            init_robot(&match);
            prepare4starting(&match);
            continue;
        }
        else if (com_exch(c))
        {
            exch.id1 = c.id1;
            exch.id2 = c.id2;
        }
        else
            action_on_cmd(c);
        //6. 检查有无球场事件，有则：
        //          向GUI发送事件对应的球、人位置
        //          发送事件类型
        //          持球、聚焦队员
        //   回到5.
        //
        if (match.evt.eid != NONE && match.evt.disabled == 1)
        {
            process_evt();

            set_status(&sts);
            send_status(sock, &sts);
            continue;
        }
        //6. 无则： 调用Robot计算所有人的下一步反应动作
        //7. 根据反应动作，计算成各球员位置
        //8. 将计算完成的各球员位置、球的位置、持球人、聚焦人发送给GUI
        //9. 在GUI渲染的同时，完成：
        //          检测球员冲撞，设置冲撞动作
        //          判断球场事件
        //          设置持球、聚焦球员
        //          球迹计算
        //   回到5.

        /* See if it's time to enable event */
        if (match.evt.disabled == 1 
                && in_scope(full_field, match.ball.pos) 
                && (match.ball.spd.x+match.ball.spd.y+match.ball.spd.z != 0))
        {
            enable_event();
        }

        call_robot();
        players_act();

        set_status(&sts);
        send_status(sock, &sts);

        detect_collision();
        detect_ball_event();

        match.holder_id = find_holder();
        holder = id2player(match.holder_id);
        match.focus_id = find_focuser();
        focuser = id2player(match.focus_id);

        // collision of ball
        /*float dis = distance(holder->pos, match.ball.pos);
        if ( dis <= COLLISION_DISTANCE && holder != NULL)
        {
            Direction dirt = direct_to_ball(holder, &match);
            Speed ref_spd = reflecting(match.ball.spd, dirt);
            printf("bspd: %f, %f, %f; dtob: %f, %f, %f; ref: %f, %f, %f\n", match.ball.spd.x, match.ball.spd.y, match.ball.spd.z, dirt.x, dirt.y, dirt.z, ref_spd.x, ref_spd.y, ref_spd.z);
            Speed col_spd = {holder->spd.x * 1.3, holder->spd.y*1.3, holder->spd.z*1.3};
            match.ball.spd = add_vector(ref_spd, col_spd);
            match.ball.last_toucher = holder;
            match.ball.kicked = 0;
            match.ball.keeped = 0;
        }*/

        match.ball.pos.x += match.ball.spd.x;
        match.ball.pos.y += match.ball.spd.y;
        match.ball.pos.z += match.ball.spd.z;
        if(match.ball.pos.z < 0.0)
            match.ball.pos.z = 0.0;

        /* 球迹计算 */

        float acce_factor;
        Speed acceleration;
        if (match.ball.kicked == 1)
        {
            match.ball.kicked = 0;
        }
        else
        {
            if (match.ball.pos.z > 0.0)
            {
                match.ball.spd.z = match.ball.spd.z - (9.8*Meter/(match.nTick*match.nTick));
                acce_factor = 0.5;
            }
            else 
            {
                if (match.ball.spd.z*match.ball.spd.z >= 0.5*Meter/match.nTick)
                    match.ball.spd.z = - match.ball.spd.z * 0.5;
                else
                    match.ball.spd.z = 0.0;
                acce_factor = 3.0;
            }


            acceleration = generate_speed(counter_vector(match.ball.spd), acce_factor, speed_per_power);
            match.ball.spd.x += (acceleration.x/match.nTick);
            match.ball.spd.y += (acceleration.y/match.nTick);
        }

    }
    printf("====================\n");
    printf("Score: %d : %d\n", match.team1.score, match.team2.score);
    printf("====================\n");
    clean_up();
    return 0;
}

