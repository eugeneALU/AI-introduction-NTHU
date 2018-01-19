/*

Mid-term Project:
Implement a computer program that uses MCTS (Monte Carlo Tree Search) to play a five-in-a-line in a Go-game board of 15 by 15. (The true GO game board is 19 by 19)

A five-in-a-line GO game winner is the one who can ¡§firstly¡¨ play the stones (Black or White) in a connected line either horizontal or vertical or diagonal in the
Go-game board. We assign the board position according to the Row (A,B,C,D,E,F,G,H,I,J,K,L,M,N,O) and the Column (0,1,2,3,4,5,6,7,8,9,10,11,12,13,14), So the center
position is at the coordinate (H,7). You should specify the stone to play at the certain position in term of the format [Black (H,7)]; [White,(G,6)], etc. for example,
so that every one can use the same representation to show to other where the stones are played.



We assume Black play first, and white play the second. So you should allow the program to play either in black stone or the white stone. We also would only the program
to spend no more than 10 seconds to play a move. So you should control your MCTS computation resource to explore the space within the time limit. You should consider the
¡§quality score¡¨ of a ¡§win¡¨,¡¨ tie¡¨, ¡§loose¡¨ of a game for backpropragation to a selection node and balance parameters between the exploration and exploitation.
You could combine other heuristics or knowledge to enhance the chance of winning.

*/
//day one : 2-win 2-lose but one win with many stones 
//day two : 5-win 1-lose and lose with less stones 

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include <string.h>
#include <time.h>

using namespace std;

// Global var
int turn = 0; // 0-player 1-system
int first = 0; // 1-player 2-system
int endgame = 0; // endgame = 1, game over
int board[15][15] = {};
int weight[15][15] = {};
int max_weight[10][3] = {};		// max_weight[i][0] store max weight, max_weight[i][1] store row of location, max_weight[i][2] store col of location

/*
main function contains two function
void Player_black_white_select()
let the player select black or white
sel black set turn = 0; first = 1;
sel white set turn = 1; first = 2;
void Gamestart()
Function to run the game
*/

void Player_black_white_select();
void Gamestart();
void Show_go_board();
void player_play();
void system_play();
bool check_win(int row, int col, int turn);
void calculate_weight_attack(int row, int col, int player);
void calculate_weight_defense(int row, int col, int player);
void assign_weight(int mode, int count, int mark0, int mark1, int row, int col);
void Set_stone_on_board(int col, int row);
int* system_search(int z);
int expand(int z);

int main(){

	Player_black_white_select();
	Gamestart();
	system("pause");
	return 0;
}
void Player_black_white_select(){
	char temp;
	printf("Hi welcom to five-in-a-line Go game\n");
	printf("Choose black or white stone, black go first\n");
	printf("Choose black please press 0, white press 1\n");
	scanf("%d%c", &turn, &temp);
	if (turn == 0){
		printf("You select black stone %d\n", turn);
		first = 1;
	}
	else {
		printf("You select white stone\n");
		first = 2;
	}
	printf("Let's start the game\n");
}
void Gamestart(){
	Show_go_board();
	while (!endgame){
		if (turn == 0){
			player_play();
			turn = 1;
		}
		else{
			system_play();
			turn = 0;
		}
	}
}
void Show_go_board(){
	char row_label = 'A';
	printf("   ");
	for (int z = 0; z<15; z++){
		printf("%4d", z);
	}
	printf("\n");
	for (int i = 0; i<15; i++){
		printf("-----------------------------------------------------------------\n");
		printf("%2c  |", i + row_label);
		for (int j = 0; j<15; j++){
			if (board[i][j] == 0)
				printf("   |");
			else if (board[i][j] == 1 && first == 1)
				printf("¡´ |");
			else if (board[i][j] == 2 && first == 2)
				printf("¡´ |");
			else if (board[i][j] == 1 && first == 2)
				printf("¡³ |");
			else if (board[i][j] == 2 && first == 1)
				printf("¡³ |");
		}
		printf("\n");
	}
	printf("-----------------------------------------------------------------\n");
	if (first == 1)
		printf("Black : Player		White : System\n");
	else
		printf("Black : System		White : Player\n");
}
void player_play(){
	char row;
	int col;
	printf("Player Input Row and Col (EX: A1)\n");
	scanf("%c%d", &row, &col);
	row = row - 65;
	fflush(stdin);
	// Set Stone on the board
	// check whether stone set at the same place
	// check whether pos is inside board
	// if input more than 2 char deal
	if (row > 14 || row < 0 || col > 14 || col < 0){
		printf("Out of board, choose another position\n");
		player_play();
	}
	else{
		if (board[row][col] == 0){
			Set_stone_on_board(row, col);
			printf("Player set a stone at (%c,%d) (EX: A1) \n", row + 65, col);
		}
		else{
			printf("This play have stone, choose another position\n");
			player_play();
		}
	}
	Show_go_board();
	if (check_win(row, col, turn)){
		printf("player win\n");
		endgame = 1;
	}
}
void system_play()
{
	int col, z = 0;
	char row;
	int* bestchoose;

	// clean the weight table 
	for (int i = 0; i<15; i++){
		for (int j = 0; j<15; j++){
			weight[i][j] = 0;
		}
	}
	//reset max weight 
	for (int i = 0; i < 10; i++){
		max_weight[i][0] = 0;
		max_weight[i][1] = 0;
		max_weight[i][2] = 0;
	}
	// assign the weight of every positions  
	for (int i = 0; i<15; i++){
		for (int j = 0; j<15; j++){
			calculate_weight_attack(i, j, 2);
			calculate_weight_defense(i, j, 2);
		}
	}
	//find the location	of the highest weight
	for (int i = 0; i<15; i++){
		for (int j = 0; j<15; j++){
			if (weight[i][j] > max_weight[0][0]){
				max_weight[0][0] = weight[i][j];
				max_weight[0][1] = i;		//store row
				max_weight[0][2] = j;		//store col
			}
		}
	}
	if (max_weight[0][0] == 0){			//system play first
		row = 7;
		col = 7;
	}
	else {
		//find all points with max-weigh 
		for (int i = 0; i < 15; i++){
			for (int j = 0; j < 15; j++){
				if (weight[i][j] == max_weight[0][0]){
					z++;
					max_weight[z][0] = weight[i][j];
					max_weight[z][1] = i;		//store row
					max_weight[z][2] = j;		//store col
				}
			}
		}
		bestchoose = system_search(z);	//search the best point 
		row = bestchoose[0];
		col = bestchoose[1];
	}

	system("cls");
	turn = 1; //incase the turn go unpredectable when we expending
	Set_stone_on_board(row, col);
	printf("System set a stone at (%c,%d)\n", row + 65, col);
	Show_go_board();

	if (check_win(row, col, turn)){		//check if win or not 
		printf("System win\n");
		endgame = 1;
	}
}
void Set_stone_on_board(int row, int col){
	if (turn == 0){
		board[row][col] = 1;
	}
	else board[row][col] = 2;
}
bool check_win(int row, int col, int turn){
	int i, j, count;

	count = 1;
	i = row;
	while (board[--i][col] == turn + 1 && i>-1){
		count++;
	}
	i = row;
	while (board[++i][col] == turn + 1 && i<15){
		count++;
	}
	if (count >= 5){
		return 1;
	}

	count = 1;
	j = col;
	while (board[row][--j] == turn + 1 && j>-1){
		count++;
	}
	j = col;
	while (board[row][++j] == turn + 1 && j<15){
		count++;
	}
	if (count >= 5){
		return 1;
	}

	count = 1;
	i = row; j = col;
	while (board[--i][--j] == turn + 1 && i>-1 && j>-1){
		count++;
	}
	i = row; j = col;
	while (board[++i][++j] == turn + 1 && i<15 && j<15){
		count++;
	}
	if (count >= 5){
		return 1;
	}

	count = 1;
	i = row; j = col;
	while (board[--i][++j] == turn + 1 && i>-1 && j<15){
		count++;
	}
	i = row; j = col;
	while (board[++i][--j] == turn + 1 && i<15 && j>-1){
		count++;
	}
	if (count >= 5){
		return 1;
	}

	return 0;

}
void calculate_weight_attack(int row, int col, int player){
	int count = 0, i, j, mark0 = 0, mark1 = 0;				//use "mark" to check if the line is blocked by player (two sides)     

	if (board[row][col] != 0)									//the position has been set the stone
		return;

	// check up and down directions	
	i = row; j = col;
	while (board[--i][col] == player && i>-1){
		count++;
	}
	if (board[i][col] == (3 - player) && i>-1)
		mark0 = 1;
	i = row; j = col;
	while (board[++i][col] == player && i<15){
		count++;
	}
	if (board[i][col] == (3 - player) && i<15)
		mark1 = 1;

	// assign the weight of attack

	assign_weight(0, count, mark0, mark1, row, col);

	// check left and right directions		
	count = 0; mark0 = 0; mark1 = 0;
	i = row; j = col;
	while (board[row][--j] == player  && j>-1){
		count++;
	}
	if (board[row][j] == (3 - player) && j>-1)
		mark0 = 1;
	i = row; j = col;
	while (board[row][++j] == player && j<15){
		count++;
	}
	if (board[row][j] == (3 - player) && j<15)
		mark1 = 1;

	assign_weight(0, count, mark0, mark1, row, col);

	//check directions
	count = 0; mark0 = 0; mark1 = 0;
	i = row; j = col;
	while (board[--i][++j] == player  && i>-1 && j<15){
		count++;
	}
	if (board[i][j] == (3 - player) && i>-1 && j<15)
		mark0 = 1;
	i = row; j = col;
	while (board[++i][--j] == player  && i<15 && j>-1){
		count++;
	}
	if (board[i][j] == (3 - player) && i<15 && j>-1)
		mark1 = 1;

	assign_weight(0, count, mark0, mark1, row, col);

	// check directions		
	count = 0; mark0 = 0; mark1 = 0;
	i = row; j = col;
	while (board[--i][--j] == player && i>-1 && j>-1){
		count++;
	}
	if (board[i][j] == (3 - player) && i>-1 && j>-1)
		mark0 = 1;
	i = row; j = col;
	while (board[++i][++j] == player && i<15 && j<15){
		count++;
	}
	if (board[i][j] == (3 - player) && i<15 && j<15)
		mark1 = 1;

	assign_weight(0, count, mark0, mark1, row, col);


}
void calculate_weight_defense(int row, int col, int player){    // assign the weight of defense
	int count = 0, i, j, mark0 = 0, mark1 = 0;
	int times[5] = {};
	player = 3 - player;

	if (board[row][col] != 0)
		return;
	i = row; j = col;
	while (board[--i][col] == player && i>-1){
		count++;
	}
	if (board[i][col] == (3 - player) && i>-1)
		mark0 = 1;
	i = row; j = col;
	while (board[++i][col] == player && i<15){
		count++;
	}
	if (board[i][col] == (3 - player) && i<15)
		mark1 = 1;

	assign_weight(1, count, mark0, mark1, row, col);

	count = 0;  mark0 = 0; mark1 = 0;
	i = row; j = col;
	while (board[row][--j] == player  && j>-1){
		count++;
	}
	if (board[row][j] == (3 - player) && j>-1)
		mark0 = 1;
	i = row; j = col;
	while (board[row][++j] == player && j<15){
		count++;
	}
	if (board[row][j] == (3 - player) && j<15)
		mark1 = 1;

	assign_weight(1, count, mark0, mark1, row, col);

	count = 0; mark0 = 0; mark1 = 0;
	i = row; j = col;
	while (board[--i][++j] == player  && i>-1 && j<15){
		count++;
	}
	if (board[i][j] == (3 - player) && i>-1 && j<15)
		mark0 = 1;
	i = row; j = col;
	while (board[++i][--j] == player  && i<15 && j>-1){
		count++;
	}
	if (board[i][j] == (3 - player) && i<15 && j>-1)
		mark1 = 1;

	assign_weight(1, count, mark0, mark1, row, col);


	count = 0; mark0 = 0; mark1 = 0;
	i = row; j = col;
	while (board[--i][--j] == player && i>-1 && j>-1){
		count++;
	}
	if (board[i][j] == (3 - player) && i>-1 && j>-1)
		mark0 = 1;
	i = row; j = col;
	while (board[++i][++j] == player && i<15 && j<15){
		count++;
	}
	if (board[i][j] == (3 - player) && i<15 && j<15)
		mark1 = 1;

	assign_weight(1, count, mark0, mark1, row, col);



}
void assign_weight(int mode, int count, int mark0, int mark1, int row, int col){
	/*mode 0 for assign attack weight
	mode 1 for assign defense weight 
	if (mode == 0){
		if (count == 4)
			weight[row][col] += 80;
		else if (count == 3 && mark0 == 0 && mark1 == 0)
			weight[row][col] += 48;
		else if (count == 3 && (mark0 == 0 || mark1 == 0))
			weight[row][col] += 3;
		else if (count == 2 && mark0 == 0 && mark1 == 0)
			weight[row][col] += 5;
		else if (count == 1 && mark0 == 0 && mark1 == 0)
			weight[row][col] += 2;
	}
	else if (mode == 1){
		if (count == 4)
			weight[row][col] += 20;
		else if (count == 3 && mark0 == 0 && mark1 == 0)
			weight[row][col] += 12;
		else if (count == 3 && (mark0 == 0 || mark1 == 0))
			weight[row][col] += 2;
		else if (count == 2)
			weight[row][col] += 2;
		else if (count == 1)
			weight[row][col] += 1;
	}*/
	if (mode == 0){
		int times[2] = {};
		if (count == 4)
			weight[row][col] += 25;	// 800
		else if (count == 3 && mark0 == 0 && mark1 == 0)
			weight[row][col] += 15;	// 48
		else if (count == 3 && (mark0 == 0 || mark1 == 0)){
			times[0]++;
			// x3 + x3
			if (times[0] > 1)								  // if there will be double half alive four stones 	
				weight[row][col] += 25;
			// x3 + 2
			if (times[0] != 0 && times[1] != 0)		 // if there will be half alive four stones and alive three stones	 
				weight[row][col] += 25;
		}
		else if (count == 3 && (mark0 == 0 || mark1 == 0))
			weight[row][col] += 5;
		else if (count == 2 && mark0 == 0 && mark1 == 0)
			weight[row][col] += 5;
		else if (count == 1 && mark0 == 0 && mark1 == 0)
			weight[row][col] += 2;
	}
	else if (mode == 1){
		int times[2] = {};
		if (count == 4)
			weight[row][col] += 28;	// 200
		else if (count == 3 && mark0 == 0 && mark1 == 0)
			weight[row][col] += 20;	// 12

		else if (count == 3 && (mark0 == 0 || mark1 == 0)){
			times[0]++;
			if (times[0] > 1)								  // if there will be double half alive four stones 	
				weight[row][col] += 30;
			if (times[0] != 0 && times[1] != 0)		 // if there will be half alive four stones and alive three stones	 
				weight[row][col] += 30;
		}
		else if (count == 2 && mark0 == 0 && mark1 == 0){
			weight[row][col] += 3;
			times[1]++;
			if (times[0] != 0 && times[1] != 0)		 	// if there will be half alive four stones and alive three stones
				weight[row][col] += 30;
		}
		else if (count == 1)
			weight[row][col] += 1;
	}

}
int* system_search(int z)
{
	int store_max_weight[10][3];
	int store_board[15][15] = {};
	int winvalue[20] = {};
	int row;
	int col;
	int maxnumber = 0;
	int bestindex = 0, bestchoose[2] = {};

	//store the board 
	for (int i = 0; i < 15; i++){
		for (int j = 0; j < 15; j++){
			store_board[i][j] = board[i][j];
		}
	}
	//store the weight 
	for (int i = 1; i <= z; i++){
		store_max_weight[i][0] = max_weight[i][0];
		store_max_weight[i][1] = max_weight[i][1];
		store_max_weight[i][2] = max_weight[i][2];
	}

	for (int i = 1; i <= z; i++){

		//set the stone point we are going to evaluate 
		row = store_max_weight[i][1];
		col = store_max_weight[i][2];

		//reset the maxnumber 
		maxnumber = 0;

		//restore the board 
		for (int i = 0; i < 15; i++){
			for (int j = 0; j < 15; j++){
				board[i][j] = store_board[i][j];
			}
		}
		//reset max weight 
		for (int i = 0; i < 10; i++){
			max_weight[i][0] = 0;
			max_weight[i][1] = 0;
			max_weight[i][2] = 0;
		}
		// clean the weight table 
		for (int i = 0; i<15; i++){
			for (int j = 0; j<15; j++){
				weight[i][j] = 0;
			}
		}

		Set_stone_on_board(row, col);
		turn = 0;

		//evaluate the new weight 
		for (int i = 0; i<15; i++){
			for (int j = 0; j<15; j++){
				calculate_weight_attack(i, j, 1);
				calculate_weight_defense(i, j, 1);
			}
		}
		//find the location	of the highest weight
		for (int i = 0; i<15; i++){
			for (int j = 0; j<15; j++){
				if (weight[i][j] > max_weight[0][0]){
					max_weight[0][0] = weight[i][j];
					max_weight[0][1] = i;		//store row
					max_weight[0][2] = j;		//store col
				}
			}
		}
		//find all point with max-weight 
		for (int i = 0; i < 15; i++){
			for (int j = 0; j < 15; j++){
				if (weight[i][j] == max_weight[0][0]){
					maxnumber++;
					max_weight[maxnumber][0] = weight[i][j];
					max_weight[maxnumber][1] = i;		//store row
					max_weight[maxnumber][2] = j;		//store col
				}
			}
		}
		srand(time(NULL));
		for (int y = 0; y < 1000000; y++){
			winvalue[i] += expand(maxnumber);
		}
	}
	//choose the point with high win value
	int high_win_value = winvalue[1];
	bestindex = 1;
	for (int i = 1; i <= z; i++){
		if (high_win_value < winvalue[i]){
			high_win_value = winvalue[i];
			bestindex = i;
		}
	}
	bestchoose[0] = store_max_weight[bestindex][1];
	bestchoose[1] = store_max_weight[bestindex][2];

	//restore the board 
	for (int i = 0; i < 15; i++){
		for (int j = 0; j < 15; j++){
			board[i][j] = store_board[i][j];
		}
	}
	return bestchoose;
}
int expand(int z)
{
	int row;
	int col;
	int expandepth = 225;
	int maxnumber = z;
	//int winvalue = 0;
	
	for (int times = 0; times < expandepth;){
		//sel position randomly
		//if (turn == 0){
			row = rand() % 15;
			col = rand() % 15;
			if (board[row][col] == 0){
				Set_stone_on_board(row, col);
				times++;
			}
			//check win or not if anyone win return 
			if (check_win(row, col, turn)){
				if (turn == 1)  {
					return 1;
				}
				return 0;
			}
			else if (times == expandepth){
				return 0;
			}
			turn = !turn;	//change the player
		}
		/*else {
			// clean the weight table 
			for (int i = 0; i < 15; i++){
				for (int j = 0; j < 15; j++){
					weight[i][j] = 0;
				}
			}
			//reset max weight 
			for (int i = 0; i < 10; i++){
				max_weight[i][0] = 0;
				max_weight[i][1] = 0;
				max_weight[i][2] = 0;
			}
			//evaluate the new weight 
			for (int i = 0; i < 15; i++){
				for (int j = 0; j < 15; j++){
					calculate_weight_attack(i, j, turn + 1);
					calculate_weight_defense(i, j, turn + 1);
				}
			}
			//find the location	of the highest weight
			for (int i = 0; i < 15; i++){
				for (int j = 0; j < 15; j++){
					if (weight[i][j] > max_weight[0][0]){
						max_weight[0][0] = weight[i][j];
						max_weight[0][1] = i;			//store row
						max_weight[0][2] = j;		//store col
					}
				}
			}
			//find all point with max-weight  but reset the maxnumber first 
			maxnumber = 0;
			for (int i = 0; i < 15; i++){
				for (int j = 0; j < 15; j++){
					if (weight[i][j] == max_weight[0][0]){
						maxnumber++;
						max_weight[maxnumber][0] = weight[i][j];
						max_weight[maxnumber][1] = i;			//store row
						max_weight[maxnumber][2] = j;		//store col
					}
				}
			}
			maxnumber = rand() % maxnumber + 1;
			row = max_weight[maxnumber][1];
			col = max_weight[maxnumber][2];
			Set_stone_on_board(row, col);
			times++;
			turn = !turn;	//change the player
		}
		//check win or not if anyone win return 
		if (check_win(row, col, turn)){
			if (turn == 1)  {
				return 1;
			}
			return 0;
		}
		else if (times == expandepth){
			return 0;
		}
	}*/
}
