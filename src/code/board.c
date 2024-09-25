#include "board.h"
#include "step_table.h"

// generate moves from a board
// each move looks like this: {piece_number, destination}
int gen_moves(Board *board, int dice, int moves[16][2])
{
    // get 2 movable piece from movable_piece_table
    int pce0 = movable_piece_table[board->piece_bits][dice - 1][0];
    int pce1 = movable_piece_table[board->piece_bits][dice - 1][1];
    // initialize the move_cnt
    int move_cnt = 0;
    // start0 means the position of pce0
    // Obviously, all moves related to pce0 starts from start0
    int start0 = board->piece_position[pce0];
    for (int i = 0; i < 8; i++)
    {
        int endpos = step_table[start0][i];
        // search until it meets -1, means the end of the step table
        if (endpos == -1)
            break;
        moves[move_cnt][0] = pce0;
        moves[move_cnt][1] = endpos;
        move_cnt++;
    }
    if (pce1 == -1)
    {
        return move_cnt;
    }
    // start1 means the position of pce1
    // Obviously, all moves related to pce1 starts from start1
    int start1 = board->piece_position[pce1];
    for (int i = 0; i < 8; i++)
    {
        int endpos = step_table[start1][i];
        // search until it meets -1, means the end of the step table
        if (endpos == -1)
            break;
        moves[move_cnt][0] = pce1;
        moves[move_cnt][1] = endpos;
        move_cnt++;
    }
    return move_cnt;
}

// apply the given move to the board
void move(Board *board, int moves[2])
{
    // check piece capture
    for (int i = 0; i < 6; i++)
    {
        if (board->piece_position[i] == moves[1])
        {
            board->piece_position[i] = -1;
            // its a tricky bit operation
            // it sets bit i to 0
            (board->piece_bits) &= (~(1 << i));
            break;
        }
    }
    board->piece_position[moves[0]] = moves[1];
}