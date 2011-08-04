#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <stdio.h>
struct Vector {
    float x;
    float y;
    float z;
};

typedef struct Vector Position;
typedef struct Vector Speed;

typedef float Angle;
typedef Angle Direction;
typedef Angle Dirspeed;

typedef struct {
    int ac_type;
    Dirspeed dspd;
    Speed spd;
    Speed ball_spd;
    int frame;
} Action;

struct Information {
    char name[10];
    char team[10];
    int age;
    int height;
    int weight;
};

struct Attributes {
    int attack;
    int defend;
    int technique;
    int dribble;
    int pass;
    int shoot;
    int head;
    int speed;
    int speed_up;
    int reaction;
    int power;
    int body;
    int stamina;
    int teamwork;
};                  // attributes of player

struct Award {
    int ycard;
    int rcard;
    int goal;
};                 // award of player

struct Player {
    int id;
    Position pos;
    Direction direct;
    Action action;
    struct Information info;
    struct Attributes attr;
    struct Award awd;
    struct Player *next;
};

struct Ball {
    Position pos;
    Speed spd;
    struct Player *last_toucher;
    int keeped; // 0/1
    int kicked; // 0/1
};

typedef enum {A442, B442} Formation;

typedef enum {ALL_ATTACK, ATTACK, BALANCED, DEFENCE, ALL_DEFENCE} Tactics;

typedef enum {NONE, PENALTY, FREEKICK, CORNER, GOAL, OFFSIDE, THROWIN, TEEBALL} Event_ID;
struct Event {
    Event_ID eid;
    int which_team;  // 1 or 2
    Position epos;
    int disabled;
};

typedef enum {EASY, NORMAL, HARD} Difficulty;

typedef enum {CLEAR, RAIN, SNOW} Weather;


struct Team {
    int score;
    int id;  //1 or 2
    Formation form;
    Tactics tact;
    struct Player *players;
};

struct Match {
    struct Event evt;
    Difficulty dif;
    Weather weath;
    struct Team team1;
    struct Team team2;
    struct Ball ball;
    int nTick;
    int human;
    int first_half;   //0/1
    int holder_id;
    int focus_id;
    int topass_id;
};

#endif  //GLOBAL_H_
