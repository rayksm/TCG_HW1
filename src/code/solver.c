#include "solver.h"
#include "step_table.h"
#include "board.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>


int dice_seq[_dice_sequence_len_];
int goal_piece;
int path[_dice_sequence_len_][2];

Board idachessboard[30 + 1];
// only used in sample code, you can delete it
#include <stdlib.h>
void random_walk(Board chessboard, int remain_depth, int search_depth)
{
    if (remain_depth == 0)
        return;
    // 8 directions and maximum 2 possible dice choices
    int moves[16][2];
    int move_num = gen_moves(&chessboard, dice_seq[search_depth], moves);
    int chosen_move = rand() % move_num;
    Board child = chessboard;
    move(&child, moves[chosen_move]);
    random_walk(child, remain_depth - 1, search_depth + 1);
    // back propagation (kind of)
    path[search_depth][0] = chessboard.piece_position[moves[chosen_move][0]];
    path[search_depth][1] = moves[chosen_move][1];
}
// ******************end of the removable code******************
void smartwalk(int remain_depth, int search_depth){
	if(remain_depth == 0) return;

	int moves[16][2];
	int move_num = gen_moves(&idachessboard[search_depth], dice_seq[search_depth], moves);
}

void solve(prob_layout problem_layout)
{
    // TODO: Write your searching algorithm here

    // sample code: random walk 20 steps

    // copy problem layout into board
    Board chessboard;
    chessboard.piece_bits = 0x3F;
    memcpy(chessboard.piece_position, problem_layout.piece_position, sizeof(problem_layout.piece_position));
    memcpy(dice_seq, problem_layout.dice_sequence, sizeof(dice_seq));

    // start from 0 <- start from 1
    goal_piece = problem_layout.goal_piece - 1;
    srand(time(NULL));
    //random_walk(chessboard, 20, 0);
	// create root
	idachessboard[0] = chessboard;
	smartwalk(&idachessboard, sizeof(dice_seq), 0);

    // output the result
    // print time
    printf("0.000\n");
    // print step number
    printf("20\n");
    // print steps
    for (int i = 0; i < 20; i++)
    {
        printf("%d %d ", path[i][0], path[i][1]);
    }
}
