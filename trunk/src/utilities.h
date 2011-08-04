#ifndef UTILITIES_H_
#define UTILITIES_H_
#include <math.h>

float distance(Position, Position);
Angle angle(struct Vector, struct Vector);
struct Vector normalizing(struct Vector); 
Direction direction(Position, Position);
int generate_spower(Speed, float);
int generate_dpower(Dirspeed, float);
Dirspeed generate_dirspd(Angle, int, float);
Speed generate_speed(struct Vector, int, float);
struct Vector vector(Position, Position);
int in_scope(Scope, Position);
int up_scope(Scope, Position);
int down_scope(Scope, Position);
int up_down_scope(Scope, Position);
int left_scope(Scope, Position);
int right_scope(Scope, Position);
int left_right_scope(Scope, Position);
int above_scope(Scope, Position);
float modular(struct Vector);
Position intersection(Position, Position, Position);
struct Vector add_vector(struct Vector, struct Vector);
int equal_vector(struct Vector, struct Vector);
struct Vector reflecting(struct Vector, struct Vector);
struct Vector counter_vector(struct Vector);
struct Vector direct2vector(Direction, float);
Direction vector2direct(struct Vector);
struct Vector multiply(struct Vector, float);
Direction direct_diff(Direction, Direction);
Direction relocate(Direction);

#endif
