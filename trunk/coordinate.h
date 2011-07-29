#ifndef COORDINATE_H_
#define COORDINATE_H_ \
#include <math.h>

#define PI 3.14

#define Lfield 105.0
#define Wfield 68.0
#define Rcircle 9.15
#define Wgate 7.32
#define Hgate 2.44
#define Wpenaltyarea 20.16*2.0
#define Lpenaltyarea 16.5
#define Lgatearea 5.5
#define Wgatearea 9.16*2.0
#define Lpenaltypoint 11.0
#define Meter Lfield/105.0

#define HOLD_DISTANCE 1*Meter
#define COLLISION_DISTANCE 0.1*Meter
//#define OWN_DISTANCE 5*Meter

typedef struct {
    Position lb;
    Position ru;
} Scope;

typedef struct {
    Position rlb;
    Position rru;
} Relative_area;

extern Scope center_circle;
extern Scope full_field;
extern Scope left_field;
extern Scope right_field;
extern Scope penalty_field_1;
extern Scope penalty_field_2;
extern Scope gate_field_1;
extern Scope gate_field_2;
extern Scope goal_field_1;
extern Scope goal_field_2;
extern Scope corner_field_1u;
extern Scope corner_field_1d;
extern Scope corner_field_2u;
extern Scope corner_field_2d;
extern Scope penalty_curve_1;
extern Scope penalty_curve_2;
extern Scope up_gate_field;
extern Scope down_gate_field;
extern Position center_point;
extern Position penalty_point_1;
extern Position penalty_point_2;

extern Relative_area darea_balanced_front_442A;
extern Relative_area darea_balanced_middle_442A;
extern Relative_area darea_balanced_back_442A;
extern Relative_area darea_balanced_keeper_442A;

extern Relative_area darea_attack_front_442A;
extern Relative_area darea_attack_middle_442A;
extern Relative_area darea_attack_back_442A;
extern Relative_area darea_attack_keeper_442A;

extern Relative_area darea_defend_front_442A;
extern Relative_area darea_defend_middle_442A;
extern Relative_area darea_defend_back_442A;
extern Relative_area darea_defend_keeper_442A;

extern Relative_area varea_balanced_front_442A;
extern Relative_area varea_balanced_middle_442A;
extern Relative_area varea_balanced_back_442A;
extern Relative_area varea_balanced_keeper_442A;

extern Relative_area varea_attack_front_442A;
extern Relative_area varea_attack_middle_442A;
extern Relative_area varea_attack_back_442A;
extern Relative_area varea_attack_keeper_442A;

extern Relative_area varea_defend_front_442A;
extern Relative_area varea_defend_middle_442A;
extern Relative_area varea_defend_back_442A;
extern Relative_area varea_defend_keeper_442A;

extern Position formations[1][11];
#endif //COORDINATE_H_
