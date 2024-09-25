#ifndef __solver__
#define __solver__ 1
#include "../lib/problem_layout.h"
extern int dice_seq[_dice_sequence_len_];
extern int goal_piece;
extern int path[_dice_sequence_len_][2];
void solve(prob_layout problem_layout);
#endif 