#include "solver.h"
#include "step_table.h"
#include "board.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

//int abs(int a, int b);
int max(int a, int b);
int heuristic(Board* refboard, int search_depth);
bool check_visit(Board* refboard, int search_depth);
int smartwalk(int remain_depth, int search_depth, int success, int ida_depth);

int dice_seq[_dice_sequence_len_];
int goal_piece;
int path[_dice_sequence_len_][2];

Board idachessboard[30 + 1];
// only used in sample code, you can delete it
//int abs(int a, int b){
//	if (a > b) return a - b;
//	return b - a;
//}

int max(int a, int b){
	if (a > b) return a;
	return b;
}

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
int iterative_deepening(){
	int success = 0;
	int step;
	if(idachessboard[0].piece_position[goal_piece] == 0) return 0;

	for(int i = 1; i <= 30; i++){
		success = smartwalk(i, 0, 0, 0);
		if(success == 1){
			step = i;
			break;
		}
	}

	return step; 	
}
int smartwalk(int remain_depth, int search_depth, int success, int ida_depth){
	if(remain_depth == 0) return 0;
	//if(search_depth >= 30) return 1;

	// 2 possible dices and 8 directions
	int moves[16][2];
	int move_num = gen_moves(&idachessboard[search_depth], dice_seq[search_depth], moves);

	// DFS move
	for(int i = 0; i < move_num; i++){
		Board child = idachessboard[search_depth];
		move(&child, moves[i]);
		
		// check if reach endpoint
		if(child.piece_position[goal_piece] == 0){
			path[search_depth][0] = idachessboard[search_depth].piece_position[moves[i][0]];
			path[search_depth][1] = moves[i][1];
			return 1;
		}
		
		// check if no revisit
		if(check_visit(&child, search_depth + 1)){
			int heu = heuristic(&child, search_depth + 1);
			if(heu > 0 && heu + search_depth + 1<= remain_depth){
				idachessboard[search_depth + 1] = child;
				success = smartwalk(remain_depth, search_depth + 1, 0, ida_depth + 1);
			}
		}
		else continue;
		
		if(success == 1){
			path[search_depth][0] = idachessboard[search_depth].piece_position[moves[i][0]];
			path[search_depth][1] = moves[i][1];
			return 1;
		}
	}
	return 0;
}

int heuristic(Board* refboard, int search_depth){
	int goal_pos = refboard->piece_position[goal_piece];
	if(goal_pos == -1) return -1;
	int count = 0;
	int alive[6] = {1, 1, 1, 1, 1, 1};
	alive[goal_piece] = 0;
	for(int i = search_depth; i < 30; i++){
		if(dice_seq[i] == goal_piece) break;
		if(alive[dice_seq[i]] * dice_seq[i] > 0 && refboard->piece_position[dice_seq[i]] != -1){
			count += 1;
			alive[dice_seq[i]] = 0;
		}
	}
	//return min(goal_pos % 10, goal_pos / 10) + abs(goal_pos % 10 - goal_pos / 10);
	return count + max(goal_pos % 10, goal_pos / 10);
}

bool check_visit(Board* refboard, int search_depth){
	for(int i = 0; i < search_depth; i++){
		//if(idachessboard[i].piece_position == refboard->piece_position){
		//	return 0;
		//}
		int check = 1;
		for(int j = 0; j < 6; j++){
			if(idachessboard[i].piece_position[j] != refboard->piece_position[j]){
				check = 0;
				break;
			}
		}
		if(check == 1) return 0;
	}
	return 1;
}
//void huristic(int )
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
	//smartwalk(&idachessboard, sizeof(dice_seq), 0);
	clock_gettime(CLOCK_REALTIME, &start);
	int deep = iterative_deepening();
	clock_gettime(CLOCK_REALTIME, &end);
	
	double wall_clock_time = (double)((end.tv_sec + end.tv_nsec * 1e-9) - (double)(start.tv_sec + start.tv_nsec * 1e-9));

    // output the result
    // print time
    printf("%lf\n", wall_clock_time);
    // print step number
    printf("%d\n", deep);
    // print steps
    for (int i = 0; i < deep; i++)
    {
        printf("%d %d ", path[i][0], path[i][1]);
    }
}
