#include "solver.h"
#include "step_table.h"
#include "board.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define Hash_Table_Size 1003

int max(int a, int b);
int heuristic(Board* refboard, int search_depth);
//bool check_visit(Board* refboard, int search_depth);
//bool check_visit(Board* refboard);
int smartwalk(int remain_depth, int search_depth, int success);

int future_use[64][30];
//int future_inc[64][30];
int dice_seq[_dice_sequence_len_];
int goal_piece;
int path[_dice_sequence_len_][2];
Board idachessboard[30 + 1];

typedef struct{
	size_t key;
	bool is_occupied;
} HashTableEntry;

HashTableEntry hash_table[Hash_Table_Size];

size_t hash_board_state(const int* piece_positions){
	size_t hash_value = 0;
	for(int i = 0; i < 6; i++){
		hash_value ^= piece_positions[i] + 0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
	}
	return hash_value % Hash_Table_Size;
}

bool insert_into_hs(size_t key){
	size_t index = key % Hash_Table_Size;
	size_t str = index;
	while(hash_table[index].is_occupied){
		if(hash_table[index].key == key) return false;

		index = (index + 1) % Hash_Table_Size;
		if(str == index) break;
	}

	hash_table[index].key = key;
	hash_table[index].is_occupied = true;
	return true;
}

void remove_from_hs(size_t key){
	size_t index = key % Hash_Table_Size;
	size_t str = index;

	while(hash_table[index].is_occupied){
		if(hash_table[index].key == key){
			hash_table[index].is_occupied = false;
			return;
		}
		index = (index + 1) % Hash_Table_Size;
		if(index == str) break;
	}
	return;
}
void clear_hash_table(){
	memset(hash_table, 0, sizeof(hash_table));
}

// calculate for each type of dice sequence how greedily can we move goal piece
void cal_future(){
	int dis = 0, mo1, mo2;
	for(int i = 0; i < 64; i++){
		dis = 0;
		for(int j = 29; j >= 0; j--){
			//future_use[i][j] = dis;
			mo1 = movable_piece_table[i][dice_seq[j] - 1][0];
			mo2 = movable_piece_table[i][dice_seq[j] - 1][1];
			if(mo1 == goal_piece || mo2 == goal_piece) dis++;

			future_use[i][j] = dis;
		}
	}
}

int min(int a, int b){
	if(a > b) return b;
	return a;
}
int max(int a, int b){
	if (a > b) return a;
	return b;
}

int iterative_deepening(){
	int success = 0, step;
	
	if(idachessboard[0].piece_position[goal_piece] == 0) return 0;
	int start = heuristic(&idachessboard[0], 0);
	
	for(int i = start; i <= 30; i++){
		clear_hash_table();
		success = smartwalk(i, 0, 0);
		if(success == 1){
			step = i;
			break;
		}
	}

	return step; 	
}

int smartwalk(int remain_depth, int search_depth, int success){
	// return if reach depth or time limit
	clock_gettime(CLOCK_REALTIME, &end);
	double wall_clock_time = (double)((end.tv_sec + end.tv_nsec * 1e-9) - (double)(start.tv_sec + start.tv_nsec * 1e-9));
	if(search_depth > 30 || wall_clock_time > 9.5) return 0;


	// 2 possible dices and 8 directions
	int moves[16][2];
	int move_num = gen_moves(&idachessboard[search_depth], dice_seq[search_depth], moves);

	// DFS move
	for(int i = 0; i < move_num; i++){
		Board child = idachessboard[search_depth];
		move(&child, moves[i]);
		
		// check if reach endpoint or be eaten
		if(child.piece_position[goal_piece] == 0){
			path[search_depth][0] = idachessboard[search_depth].piece_position[moves[i][0]];
			path[search_depth][1] = moves[i][1];
			return 1;
		}
		
		if(child.piece_position[goal_piece] == -1) continue;

		// check if no revisit
		//if(check_visit(&child, search_depth + 1)){
		size_t board_hash = hash_board_state(child.piece_position);
		if(insert_into_hs(board_hash)){
			int heu = heuristic(&child, search_depth + 1);
			if(heu + search_depth + 1<= remain_depth){
				idachessboard[search_depth + 1] = child;
				success = smartwalk(remain_depth, search_depth + 1, 0);
			}
			remove_from_hs(board_hash);
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
	
	int count = 0, cheby = max(goal_pos % 10, goal_pos / 10);
	
	// 33 problem
	if(goal_pos > 33 && goal_pos % 11 == 0) count++;
	
	// next dice
	int next_dice = dice_seq[search_depth] - 1, mo1, mo2;
	char alivedice = refboard->piece_bits;

	mo1 = movable_piece_table[alivedice][next_dice][0];
	mo2 = movable_piece_table[alivedice][next_dice][1];
	if(mo1 != goal_piece && mo2 != goal_piece) count++;
	
	// if this dice sequence is impossible to reach the end, no really no why is 2
	if(future_use[alivedice][search_depth] < cheby) count += 2;

	return count + cheby;
}

/*
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
*/
bool check_visit(Board* refboard){
	size_t board_hash = hash_board_state(refboard->piece_position);
	return insert_into_hs(board_hash);
}

void solve(prob_layout problem_layout)
{
    // TODO: Write your searching algorithm here

    // copy problem layout into board
    Board chessboard;
    chessboard.piece_bits = 0x3F;
    memcpy(chessboard.piece_position, problem_layout.piece_position, sizeof(problem_layout.piece_position));
    memcpy(dice_seq, problem_layout.dice_sequence, sizeof(dice_seq));

    // start from 0 <- start from 1
    goal_piece = problem_layout.goal_piece - 1;
    srand(time(NULL));
   
	// create root
	idachessboard[0] = chessboard;

	clock_gettime(CLOCK_REALTIME, &start);
	cal_future();
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
	printf("\n");
}
