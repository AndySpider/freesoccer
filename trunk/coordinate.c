#include "global.h"
#include "coordinate.h"

Scope center_circle;

Scope full_field = {{0.0, 0.0, 0.0},{Lfield, Wfield, 0.0}};
Scope left_field = {{0.0, 0.0, 0.0},{Lfield/2.0, Wfield, 0.0}};
Scope right_field = {{Lfield/2.0, 0.0, 0.0},{Lfield, Wfield, 0.0}};
Scope up_gate_field = {{0.0, (Wfield + Wgate)/2.0, 0.0}, {Lfield, Wfield, 0.0}};
Scope down_gate_field = {{0.0, 0.0, 0.0}, {Lfield, (Wfield - Wgate)/2.0, 0.0}};

Scope penalty_field_1 = {{0.0, (Wfield - Wpenaltyarea)/2.0, 0.0}, {Lpenaltypoint, (Wfield + Wpenaltyarea)/2.0, 0.0}};
Scope gate_field_1 = {{0.0, (Wfield - Wgatearea)/2.0, 0.0}, {Lgatearea, (Wfield + Wgatearea)/2.0, 0.0}};
Scope goal_field_1 = {{-0.5*Meter, (Wfield - Wgate)/2.0, 0.0},{0.0, (Wfield + Wgate)/2.0, Hgate}};

Scope penalty_field_2 = {{Lfield - Lpenaltyarea, (Wfield - Wpenaltyarea)/2.0, 0.0},{Lfield, (Wfield + Wpenaltyarea)/2.0, 0.0}};
Scope gate_field_2 = {{Lfield - Lgatearea, (Wfield - Wgatearea)/2.0, 0.0}, {Lfield, (Wfield + Wgatearea)/2.0, 0.0}};
Scope goal_field_2 = {{Lfield, (Wfield - Wgate)/2.0, 0.0}, {Lfield + 0.5*Meter, (Wfield + Wgate)/2.0, Hgate}};

Scope corner_field_1u;
Scope corner_field_1d;
Scope corner_field_2u;
Scope corner_field_2d;
Scope penalty_curve_1;
Scope penalty_curve_2;

Position center_point = {Lfield/2.0, Wfield/2.0, 0.0};
Position penalty_point_1 = {Lpenaltypoint, Wfield/2.0, 0.0};
Position penalty_point_2 = {Lfield - Lpenaltypoint, Wfield/2.0, 0.0};

Relative_area darea_balanced_front_442A = {{-Wfield/8.0, -Wfield/8.0}, {Wfield/8.0, Wfield/8.0}};
Relative_area darea_balanced_middle_442A = {{-Wfield/8.0, -Wfield/8.0}, {Wfield/8.0, Wfield/8.0}};
Relative_area darea_balanced_back_442A = {{-Wfield/8.0, -Wfield/8.0}, {Wfield/8.0, Wfield/8.0}};
Relative_area darea_balanced_keeper_442A = {{-0.0, -Wgate/2.0}, {0.5*Meter, Wgate/2.0}};

Relative_area darea_attack_front_442A = {{}, {}};
Relative_area darea_attack_middle_442A = {{}, {}};
Relative_area darea_attack_back_442A = {{}, {}};
Relative_area darea_attack_keeper_442A = {{}, {}};

Relative_area darea_defend_front_442A = {{}, {}};
Relative_area darea_defend_middle_442A = {{}, {}};
Relative_area darea_defend_back_442A = {{}, {}};
Relative_area darea_defend_keeper_442A = {{}, {}};

Relative_area varea_balanced_front_442A = {{-Wfield/8.0, -Wfield/2.0}, {Lfield/4.0, Wfield/2.0}};
Relative_area varea_balanced_middle_442A = {{-Wfield/8.0, -Wfield/4.0}, {Wfield/8.0, Wfield/4.0}};
Relative_area varea_balanced_back_442A = {{-Wfield/8.0, -Wfield/4.0}, {Wfield/8.0, Wfield/4.0}};
Relative_area varea_balanced_keeper_442A = {{-0.0,-Wgatearea/2.0}, {Lgatearea, Wgatearea/2.0}};

Relative_area varea_attack_front_442A = {{}, {}};
Relative_area varea_attack_middle_442A = {{}, {}};
Relative_area varea_attack_back_442A = {{}, {}};
Relative_area varea_attack_keeper_442A = {{}, {}};

Relative_area varea_defend_front_442A = {{}, {}};
Relative_area varea_defend_middle_442A = {{}, {}};
Relative_area varea_defend_back_442A = {{}, {}};
Relative_area varea_defend_keeper_442A = {{}, {}};

Position formations[1][11] = {
    {{0.0, Wfield/2, 0}, //{33.94/105*Lfield, 0/68*Wfield, 0},
    {9.5/105*Lfield, 57.79/68*Wfield, 0}, {8.22/105*Lfield, 43.87/68*Wfield, 0}, {7.99/105*Lfield, 24.08/68*Wfield, 0}, {9.70/105*Lfield, 10.89/68*Wfield, 0},
    {27.78/105*Lfield, 56.66/68*Wfield, 0}, {24.88/105*Lfield, 41.49/68*Wfield, 0}, {25.01/105*Lfield, 25.66/68*Wfield, 0}, {28.05/105*Lfield, 10.76/68*Wfield, 0},
    {50.01/105*Lfield, 36.63/68*Wfield, 0}, {50.86/105*Lfield, 31.14/68*Wfield, 0}}
};
///////////////////////////////////
