#ifndef __steptable__
#define __steptable__ 1

// it sotres every legal next step location for every positions on the board
extern const int step_table[100][8];

// given piece bits and dice number, it stores the corresponding movable pieces
extern const int movable_piece_table[64][6][2];
#endif