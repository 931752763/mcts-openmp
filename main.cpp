#include <stdio.h>
#include "CudaGo.h"
#include "mcts.h"
#include "point.h"
#include <stdlib.h>

#define NUM_MOVES 120
#define TIME_EACH_MOVE 2*60*1000 // ms

int main(int argc, char *argv[]) {
    int cpu_threads_num = atoi(argv[1]);

    int bd_size = 9;
	Mcts* cpu;
	Mcts* cpu0;
	// Mcts* gpu;
	Point p;
	CudaBoard board(bd_size);
	int step = 0;
	std::vector<Point> seq;
	printf("hybrid start. gpu first\n");
	while (step < NUM_MOVES) {
		// gpu = new Mcts(GPU, bd_size, TIME_EACH_MOVE, seq);
		// p = gpu->run(cpu_threads_num);
		// step++;
		// printf("gpu : (%d,%d)\n", p.i, p.j);
		// seq.push_back(p);
		// board.update_board(p);
		// board.print_board();

		cpu0 = new Mcts(CPU, bd_size, TIME_EACH_MOVE, seq);
		p = cpu0->run(cpu_threads_num);
		step++;
		seq.push_back(p);
		printf("cpu : (%d,%d)\n", p.i, p.j);
		board.update_board(p);
		board.print_board();

		cpu = new Mcts(CPU, bd_size, TIME_EACH_MOVE, seq);
		p = cpu->run(cpu_threads_num);
		step++;
		seq.push_back(p);
		printf("cpu : (%d,%d)\n", p.i, p.j);
		board.update_board(p);
		board.print_board();
		delete cpu;
		delete cpu0;
		// delete gpu;
	}
	printf("score:%d\n", board.score());
}

