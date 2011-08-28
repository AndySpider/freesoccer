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
#include "utilities.h"


/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: distance 
 *
 * @Param: pos1
 * @Param: pos2
 *
 * @Returns: distance bewteen pos1 and pos2
 */
/* ----------------------------------------------------------------------------*/
float distance(Position pos1, Position pos2)
{
    float re;
    re = sqrt((pos1.x-pos2.x)*(pos1.x-pos2.x) + (pos1.y-pos2.y)*(pos1.y-pos2.y)+(pos1.z - pos2.z)*(pos1.z - pos2.z));
    return re;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: normalizing normalizing a 3D vector
 *
 * @Param: vec
 *
 * @Returns: normalized vector
 */
/* ----------------------------------------------------------------------------*/
struct Vector normalizing(struct Vector vec)
{
    if (vec.x == 0 && vec.y == 0 && vec.z == 0)
        return vec;

    struct Vector d, o;
    float len;
    o.x = 0;
    o.y = 0;
    o.z = 0;
    len = distance(o, vec);
    d.x = 1.0/len*vec.x;
    d.y = 1.0/len*vec.y;
    d.z = 1.0/len*vec.z;
    return d;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: generate_spower derive power from a position speed
 *
 * @Param: spd
 * @Param: s_p_p
 *
 * @Returns: power
 */
/* ----------------------------------------------------------------------------*/
int generate_spower(Speed spd, float s_p_p)
{
    int power = 0;
    struct Vector vec = normalizing(spd);
    if (vec.x != 0)
        power = spd.x / (vec.x * s_p_p);
    else if (vec.y != 0)
        power = spd.y / (vec.y * s_p_p);
    else if (vec.z != 0)
        power = spd.z / (vec.z * s_p_p);
    return power;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: generate_dpower derive power from a rotate(Direction) speed
 *
 * @Param: dspd
 * @Param: d_p_p
 *
 * @Returns: power
 */
/* ----------------------------------------------------------------------------*/
int generate_dpower(Dirspeed dspd, float d_p_p)
{
    return fabsf(dspd) / d_p_p;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: generate_speed generate position speed from the vector which directed from origin to destination and a power indicated the strenth.
 *
 * @Param: vec
 * @Param: power
 * @Param: s_p_p
 *
 * @Returns: position speed
 */
/* ----------------------------------------------------------------------------*/
Speed generate_speed(struct Vector vec, int power, float s_p_p)
{
    struct Vector d;
    d = normalizing(vec);
    d.x = d.x * s_p_p * power;
    d.y = d.y * s_p_p * power;
    d.z = d.z * s_p_p * power;
    if (modular(d) > modular(vec))     // vec passed in should lie from orgin to destination
        return (Speed)vec;
    else
        return (Speed)d;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: generate_dirspd generate rotate speed from the angle between original angle and the end angle and a power indicated the strenth
 *
 * @Param: agl
 * @Param: power
 * @Param: d_p_p
 *
 * @Returns: rotate speed
 */
/* ----------------------------------------------------------------------------*/
Dirspeed generate_dirspd(Angle agl, int power, float d_p_p)
{
    if (agl == 0.0)
        return 0.0;

    int sign = agl / fabs(agl);
    Dirspeed dspd = power * d_p_p * sign;

    if (dspd*dspd > agl*agl)
        return agl;
            
    return dspd;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: angle caculating the angle bewteen two vectors
 *
 * @Param: dir1
 * @Param: dir2
 *
 * @Returns: angle
 */
/* ----------------------------------------------------------------------------*/
Angle angle(struct Vector dir1, struct Vector dir2)
{
    if ((dir1.x == 0 && dir1.y == 0 && dir1.z == 0)
            || (dir2.x == 0 && dir2.y == 0 && dir2.z == 0))
            return -1;

    struct Vector d1, d2;
    float dis, ang;
    d1 = normalizing(dir1);
    d2 = normalizing(dir2);
    dis = distance(d1, d2);
    ang = 2*asin((0.5*dis));
    return ang;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: vector generate the vector pointing from pos1 to pos2
 *
 * @Param: pos1
 * @Param: pos2
 *
 * @Returns: vector
 */
/* ----------------------------------------------------------------------------*/
struct Vector vector(Position pos1, Position pos2)
{
    struct Vector dir;
    dir.x = pos2.x - pos1.x;
    dir.y = pos2.y - pos1.y;
    dir.z = pos2.z - pos1.z;
    return dir;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: in_scope judging if pos is inside scp
 *
 * @Param: scp
 * @Param: pos
 *
 * @Returns: True or False 
 */
/* ----------------------------------------------------------------------------*/
int in_scope(Scope scp, Position pos)
{
    if ((scp.lb.x <= pos.x && pos.x <= scp.ru.x)
            && (scp.lb.y <= pos.y && pos.y <= scp.ru.y) 
            && (scp.lb.z <= pos.z && pos.z <= scp.ru.z) )
        return 1;
    else
        return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: up_scope judging if pos is upside scp
 *
 * @Param: scp
 * @Param: pos
 *
 * @Returns: T/F
 */
/* ----------------------------------------------------------------------------*/
int up_scope(Scope scp, Position pos)
{
    if (( scp.ru.y < pos.y && (scp.lb.x <= pos.x && pos.x <= scp.ru.x)))
        return 1;
    else
        return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: down_scope judging if pos is downside scp
 *
 * @Param: scp
 * @Param: pos
 *
 * @Returns: T/F
 */
/* ----------------------------------------------------------------------------*/
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

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: modular get the modular of a vector
 *
 * @Param: vec
 *
 * @Returns: modular
 */
/* ----------------------------------------------------------------------------*/
float modular(struct Vector vec)
{
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: intersection get the intersecton of a plane through point p and a line made by point a and b, while the plane and the line are perpendicular
 *
 * @Param: a
 * @Param: b
 * @Param: p
 *
 * @Returns: the intersection
 */
/* ----------------------------------------------------------------------------*/
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

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: equal_vector test if two vector are indentical
 *
 * @Param: vec1
 * @Param: vec2
 *
 * @Returns: T/F
 */
/* ----------------------------------------------------------------------------*/
int equal_vector(struct Vector vec1, struct Vector vec2)
{
    if (vec1.x == vec2.x
            && vec1.y == vec2.y
            && vec1.z == vec2.z )
        return 1;
    else
        return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @Synopsis: reflecting get the reflecting vector of inc_vec from the mirror plane with nor_vec as its normal vector
 *
 * @Param: inc_vec
 * @Param: nor_vec
 *
 * @Returns: reflecting vector
 */
/* ----------------------------------------------------------------------------*/
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

struct Vector counter_vector(struct Vector vec)
{
    struct Vector v = {-vec.x, -vec.y, -vec.z};
    return v;
}

struct Vector direct2vector(Direction dir, float len)
{
    struct Vector vec;
    vec.x = cos(dir) * len;
    vec.y = sin(dir) * len;
    vec.z = 0.0;
    return vec;
}

Direction vector2direct(struct Vector vec)
{
    if (vec.x == 0 && vec.y == 0)
        return INFINITE;
    float len = sqrt(vec.x * vec.x + vec.y * vec.y);
    Direction dirt;
    dirt = acos(vec.x / len);
    if (vec.y < 0)
        dirt = 2*PI - dirt;
    return dirt;
}

struct Vector multiply(struct Vector vec, float factor)
{
    struct Vector re = {vec.x * factor, vec.y * factor, vec.z * factor};
    return re;
}

Direction direct_diff(Direction dir1, Direction dir2)
{
    Angle dir = dir2 - dir1;
    if (dir <= PI && dir >= -PI)
        return dir;
    else if (dir > PI)
        dir -= 2*PI;
    else if (dir < -PI)
        dir += 2*PI;

    return direct_diff(0.0, dir);
}

Direction relocate(Direction dir)
{
    if (dir == INFINITE)   // support for vector2direct()
        return INFINITE;

    return direct_diff(0.0, dir);
}
