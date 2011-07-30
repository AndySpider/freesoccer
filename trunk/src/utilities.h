#ifndef UTILITIES_H_
#define UTILITIES_H_
#include <math.h>

float distance(Position, Position);
float angle(Direction, Direction);
Direction normalizing(Direction); 
Direction direction(Position, Position);
int generate_power(Speed, float);
Speed generate_speed(Direction, int, float);
float distance(Position, Position);
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

#endif
