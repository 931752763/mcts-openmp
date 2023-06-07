#include <stdio.h>
#include "CudaGo.h"
#include "mcts.h"
#include "point.h"
#include <stdlib.h>
#include <time.h>

#define NUM_MOVES 120
#define TIME_EACH_MOVE 2*60*1000 // ms

int main(int argc, char *argv[]) {
    int cpu_threads_num = atoi(argv[1]);
	int max_count = atoi(argv[2]);
	int max_index = atoi(argv[3]);

    int bd_size = 9;
	Mcts* cpu;
	// Mcts* cpu0;
	Mcts* gpu;
	Point p;
	CudaBoard board(bd_size);
	int step = 0;
	std::vector<Point> seq;
	printf("hybrid start. gpu first\n");
	clock_t start, end;
	double gpu_time = 0.0;
	double cpu_time = 0.0;
	while (step < NUM_MOVES) {
		gpu = new Mcts(GPU, bd_size, TIME_EACH_MOVE, seq);
		start = clock();
		p = gpu->run(cpu_threads_num, max_count, max_index);
		end = clock();
		gpu_time += (double)(end - start) / CLOCKS_PER_SEC;
		step++;
		printf("gpu : (%d,%d)\n", p.i, p.j);
		seq.push_back(p);
		board.update_board(p);
		board.print_board();

		// cpu0 = new Mcts(CPU, bd_size, TIME_EACH_MOVE, seq);
		// p = cpu0->run(cpu_threads_num);
		// step++;
		// seq.push_back(p);
		// printf("cpu : (%d,%d)\n", p.i, p.j);
		// board.update_board(p);
		// board.print_board();

		cpu = new Mcts(CPU, bd_size, TIME_EACH_MOVE, seq);
		start = clock();
		p = cpu->run(cpu_threads_num, max_count, max_index);
		end = clock();
		cpu_time += (double)(end - start) / CLOCKS_PER_SEC;
		step++;
		seq.push_back(p);
		printf("cpu : (%d,%d)\n", p.i, p.j);
		board.update_board(p);
		board.print_board();
		delete cpu;
		// delete cpu0;
		delete gpu;
	}
	printf("score:%d\n", board.score());
	printf("gpu time: %lf \n", gpu_time);
	printf("cpu time: %lf \n", cpu_time);
}

