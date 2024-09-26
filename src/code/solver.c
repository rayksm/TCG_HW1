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
int checknei(int a, int b);
int heuristic(Board* refboard, int search_depth);
int heuristic2(Board* refboard, int search_depth);
int heuristic3(Board* refboard, int search_depth);
bool check_visit(Board* refboard, int search_depth);
int smartwalk(int remain_depth, int search_depth, int success, int ida_depth);

int dice_seq[_dice_sequence_len_];
int goal_piece;
int path[_dice_sequence_len_][2];
int nowdis[6];

Board idachessboard[30 + 1];
// only used in sample code, you can delete it
//int abs(int a, int b){
//	if (a > b) return a - b;
//	return b - a;
//}
int min(int a, int b){
	if(a > b) return b;
	return a;
}
int max(int a, int b){
	if (a > b) return a;
	return b;
}
int checknei(int a, int b){
	if(a < 0|| b < 0) return 0;
	if (abs(a % 10 - b % 10) + abs(a / 10 - b / 10) > 2) return 0;
	return 1;
}

int mindis(int *position){
	int dis = 22, cdis;
	for(int i = 0; i < 6; i++){
		if(position[i] == -1) continue;
		for(int j = 0; j < 6; j++){
			//if(i == goal_piece || j == goal_piece) continue;
			if(position[j] == -1) continue;
			if(i != j) cdis = max(abs(position[i] / 10 - position[j] / 10), abs(position[i] % 10 - position[j] % 10));
			
			// additional dis by (3, 3)
			//if((position[i] >= 33 || position[j] >= 33) && (position[i] <= 33 || position[j] <= 33) && (position[i] % 11 == 0 && position[j] % 11 == 0)) cdis += 1;
			//if((position[i] / 10 == 3 && position[j] / 10 == 3) || (position[i] % 10 == 3 && position[j] % 10 == 3)) cdis += 1;

			if(cdis == 1) return 1;
			else dis = min(dis, cdis);
		}
	}

	if(dis == 22) return -1;
	return dis;
}

void dis_cal(int *position){
	int dis;
	for(int i = 0; i < 6; i++){
		if(position[i] == -1){
			nowdis[i] = -1;
			continue;
		}
		dis = 22;
		for(int j = 0; j < 6; j++){
			if(position[j] == -1) continue;
			if(i != j) dis = min(dis, max(abs(position[i] / 10 - position[j] / 10), abs(position[i] % 10 - position[j] % 10)));
			if(dis == 1){
				nowdis[i] = dis;
				break;
			}
		}
		if(dis == 22) nowdis[i] = -1;
		else nowdis[i] = dis;
	}
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
	int start = heuristic(&idachessboard[0], 0);
	for(int i = start; i <= 30; i++){
		success = smartwalk(i, 0, 0, 0);
		if(success == 1){
			step = i;
			break;
		}
	}

	return step; 	
}
int smartwalk(int remain_depth, int search_depth, int success, int ida_depth){
	//if(remain_depth == 0) return 0;
	if(search_depth > 30) return 0;

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
			int heu = heuristic2(&child, search_depth + 1);
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
	
	// 33 problem
	if(goal_pos > 33 && goal_pos % 11 == 0) count++;
	
	// next dice
	int next_dice = dice_seq[search_depth] - 1;
	//if(refboard->piece_position[next_dice] != -1) count++;
	char alivedice = refboard->piece_bits;
	int mo1, mo2;

	mo1 = movable_piece_table[alivedice][next_dice][0];
	mo2 = movable_piece_table[alivedice][next_dice][1];
	if(mo1 != goal_piece && mo2 != goal_piece) count++;
	
	//int alive[6] = {1, 1, 1, 1, 1, 1};
	//alive[goal_piece] = 0;
	
	//for(int i = search_depth; i < 30; i++){
	//	if(dice_seq[i] - 1 == goal_piece) break;
	//	if(alive[dice_seq[i] - 1] * (dice_seq[i]) > 0 && refboard->piece_position[dice_seq[i] - 1] != -1){
	//		count += 1;
	//		alive[dice_seq[i] - 1] = 0;
	//		for(int j = 0; j < 6; j++){
	//			if(alive[j] && checknei(refboard->piece_position[dice_seq[i] - 1], refboard->piece_position[j] - 1))
	//				alive[j] = 0;
	//		}
	//	}
	//}
	//return min(goal_pos % 10, goal_pos / 10) + abs(goal_pos % 10 - goal_pos / 10);
	return count + max(goal_pos % 10, goal_pos / 10);
}

int heuristic2(Board* refboard, int search_depth){
	int goal_pos = refboard->piece_position[goal_piece];
	if(goal_pos == -1) return -1;
	int count = 0, cheby = max(goal_pos % 10, goal_pos / 10);
	
	// 33 problem
	if(goal_pos > 33 && goal_pos % 11 == 0) count++;
	
	// next dice
	int next_dice = dice_seq[search_depth] - 1;
	//if(refboard->piece_position[next_dice] != -1) count++;
	char alivedice = refboard->piece_bits;
	int mo1, mo2, push = 0, spush = 0;
	int pri[6];
	for(int i = 1; i <=5; i++){
		if(goal_piece - i > 0 && refboard->piece_position[goal_piece - i] != -1){
			pri[push] = goal_piece - i;
			push++;
		}
		if(goal_piece + i < 6 && refboard->piece_position[goal_piece + i] != -1){
				pri[push] = goal_piece + i;
				push++;
		}
	}

	//printf("%d ", pri[push - 1]);
	for(int i = search_depth - 1; i < 30; i++){
		if(cheby == 0) break;
		next_dice = dice_seq[i];
		mo1 = movable_piece_table[alivedice][next_dice][0];
		mo2 = movable_piece_table[alivedice][next_dice][1];
		if(mo1 != goal_piece && mo2 != goal_piece){
			count++;
			
			if(spush < push){
				alivedice &= (~(1 << (pri[spush])));
				spush++;
			}

			if(spush < push){
				alivedice &= (~(1 << (pri[spush])));
				spush++;
			}
		}
		else{
			count++;
			cheby--;
		}
	}
	
	return count;
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
