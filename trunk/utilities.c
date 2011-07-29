#include "global.h"
#include "engine.h"
#include "coordinate.h"
#include "utilities.h"


float distance(Position pos1, Position pos2)
{
    float re;
    re = sqrt((pos1.x-pos2.x)*(pos1.x-pos2.x) + (pos1.y-pos2.y)*(pos1.y-pos2.y)+(pos1.z - pos2.z)*(pos1.z - pos2.z));
    return re;
}

Direction normalizing(Direction dir)
{
    Direction d, o;
    float len;
    o.x = 0;
    o.y = 0;
    o.z = 0;
    len = distance(o, dir);
    d.x = 1.0/len*dir.x;
    d.y = 1.0/len*dir.y;
    d.z = 1.0/len*dir.z;
    return d;
}

int generate_power(Speed spd, float s_p_p)
{
    int power;
    Direction dirt = normalizing(spd);
    if (dirt.x != 0)
        power = spd.x / (dirt.x * s_p_p);
    else if (dirt.y != 0)
        power = spd.y / (dirt.y * s_p_p);
    else if (dirt.z != 0)
        power = spd.z / (dirt.z * s_p_p);
    return power;
}

Speed generate_speed(Direction dir, int power, float s_p_p)
{
    Direction d;
    d = normalizing(dir);
    d.x = d.x * s_p_p * power;
    d.y = d.y * s_p_p * power;
    d.z = d.z * s_p_p * power;
    return (Speed)d;
}

float angle(Direction dir1, Direction dir2)
{
    if ((dir1.x == 0 && dir1.y == 0 && dir1.z == 0)
            || (dir2.x == 0 && dir2.y == 0 && dir2.z == 0))
            return -1;

    Direction d1, d2;
    float dis, ang;
    d1 = normalizing(dir1);
    d2 = normalizing(dir2);
    dis = distance(d1, d2);
    ang = 2*asin((0.5*dis));
    return ang;
}

Direction direction(Position pos1, Position pos2)
{
    Direction dir;
    dir.x = pos2.x - pos1.x;
    dir.y = pos2.y - pos1.y;
    dir.z = pos2.z - pos1.z;
    return dir;
}

int in_scope(Scope scp, Position pos)
{
    if ((scp.lb.x <= pos.x && pos.x <= scp.ru.x)
            && (scp.lb.y <= pos.y && pos.y <= scp.ru.y) 
            && (scp.lb.z <= pos.z && pos.z <= scp.ru.z) )
        return 1;
    else
        return 0;
}

int up_scope(Scope scp, Position pos)
{
    if (( scp.ru.y < pos.y && (scp.lb.x <= pos.x && pos.x <= scp.ru.x)))
        return 1;
    else
        return 0;
}

int down_scope(Scope scp, Position pos)
{
    if (( scp.lb.y > pos.y && (scp.lb.x <= pos.x && pos.x <= scp.ru.x)))
        return 1;
    else 
        return 0;
}

int up_down_scope(Scope scp, Position pos)
{
    if (up_scope(scp, pos) || down_scope(scp, pos))
        return 1;
    else
        return 0;
}

int left_scope(Scope scp, Position pos)
{
    if ((scp.lb.x > pos.x && (scp.lb.y <= pos.y && pos.y <= scp.ru.y)))
        return 1;
    else
        return 0;
}

int right_scope(Scope scp, Position pos)
{
    if ((scp.ru.x < pos.x && (scp.lb.y <= pos.y && pos.y <= scp.ru.y)))
        return 1;
    else
        return 0;
}

int left_right_scope(Scope scp, Position pos)
{
    if (left_scope(scp, pos) || right_scope(scp, pos))
        return 1;
    else 
        return 0;
}

int above_scope(Scope scp, Position pos)
{
    if ((scp.lb.x <= pos.x && pos.x <= scp.ru.x)
            && (scp.lb.y <= pos.y && pos.y <= scp.ru.y) 
            && pos.z > scp.ru.z)
        return 1;
    else
        return 0;
}

float modular(struct Vector vec)
{
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

Position intersection(Position a, Position b, Position p)
{
    Position re;
    re.x = p.x;

    re.y = (re.x - a.x) / (b.x - a.x) * (b.y - a.y) + a.y;
    re.z = (re.x - a.x) / (b.x - a.x) * (b.z - a.z) + a.z;
    return re;
}

struct Vector add_vector(struct Vector vec1, struct Vector vec2)
{
    struct Vector re;
    re.x = vec1.x + vec2.x;
    re.y = vec1.y + vec2.y;
    re.z = vec1.z + vec2.z;
    return re;
}

int equal_vector(struct Vector vec1, struct Vector vec2)
{
    if (vec1.x == vec2.x
            && vec1.y == vec2.y
            && vec1.z == vec2.z )
        return 1;
    else
        return 0;
}

struct Vector reflecting(struct Vector inc_vec, struct Vector nor_vec)
{
    if (inc_vec.x == 0.0 && inc_vec.y == 0.0 && inc_vec.z == 0.0)
        return inc_vec;

    float ang = angle(inc_vec, nor_vec);
    if (ang > 88*PI/180)    // 88 degree
    {
        struct Vector err = {0.0, 0.0, 0.0};
        return err;
    }

    struct Vector vec = normalizing(inc_vec);
    Position inc_vertex = {vec.x, vec.y, vec.z};
    Position nor_vertex = {inc_vertex.x + nor_vec.x, inc_vertex.y + nor_vec.y, inc_vertex.z + nor_vec.z};
    Position ori_vertex = {0.0, 0.0, 0.0};
    Position int_point = intersection(inc_vertex, nor_vertex, ori_vertex);
    Position ref_vertex = {int_point.x * 2, int_point.y * 2, int_point.z * 2};
    struct Vector reflect = {ref_vertex.x - inc_vertex.x, ref_vertex.y - inc_vertex.y, ref_vertex.z - inc_vertex.z};
    return reflect;
}
