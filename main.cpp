#include <stdio.h>
#include "CudaGo.h"
#include "mcts.h"
#include "point.h"
#include <stdlib.h>
#include <time.h>

#define NUM_MOVES 10
#define TIME_EACH_MOVE 2*60*1000 // ms

int main(int argc, char *argv[]) {
    int cpu_threads_num = atoi(argv[1]);
	int max_count = atoi(argv[2]);
	int max_index = atoi(argv[3]);

    int bd_size = 9;
	Mcts* player1;
	Mcts* player2;
	Point p;
	CudaBoard board(bd_size);
	int step = 0;
	std::vector<Point> seq;
	// printf("hybrid start. gpu first\n");
	clock_t start, end;
	double player1_time = 0.0;
	double player2_time = 0.0;
	while (step < NUM_MOVES) {
		player1 = new Mcts(GPU, bd_size, TIME_EACH_MOVE, seq);
		start = clock();
		p = player1->run(cpu_threads_num, max_count, max_index);
		end = clock();
		player1_time += (double)(end - start) / CLOCKS_PER_SEC;
		step++;
		printf("player1 : (%d,%d)\n", p.i, p.j);
		seq.push_back(p);
		board.update_board(p);
		board.print_board();

		player2 = new Mcts(CPU, bd_size, TIME_EACH_MOVE, seq);
		start = clock();
		p = player2->run(cpu_threads_num, max_count, max_index);
		end = clock();
		player2_time += (double)(end - start) / CLOCKS_PER_SEC;
		step++;
		seq.push_back(p);
		printf("player2 : (%d,%d)\n", p.i, p.j);
		board.update_board(p);
		board.print_board();
		delete player1;
		delete player2;
	}
	printf("score:%d\n", board.score());
	printf("player1 time: %lf \n", player1_time);
	printf("player2 time: %lf \n", player2_time);
}

