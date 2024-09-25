#ifndef __board__
#define __board__ 1
typedef struct _board
{
    int piece_position[6];
    char piece_bits;
} Board;
int gen_moves(Board *board, int dice, int moves[16][2]);
void move(Board *board, int moves[2]);
#endif