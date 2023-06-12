#include <stdio.h>
#include "CudaGo.h"
#include "mcts.h"
#include "point.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TIME_EACH_MOVE 2*60*1000 // ms

int main(int argc, char *argv[]) {
    int cpu_threads_num = 64;
	int max_count = 10;
	int max_index = 10;
	int bd_size = 9;
	int opt;
	int num_moves = 100;
    const char *optstring = "hn:c:i:s:m:";
    while((opt = getopt(argc, argv, optstring)) != -1)
    {
        switch(opt)
        {
		case 'h':
			printf("-n: cpu_threads_num, number of threads in parallel area\n"
			"-c: max_count, it affects the GPU computation time\n"
			"-i: max_index, it affects the CPU computation time\n"
			"-s: bd_size, the size of board\n"
			"-m: num_moves, determines how many moves the two players will make\n");
			return 0;
        case 'n':
            printf("cpu_threads_num=%s\n", optarg);
			cpu_threads_num = atoi(optarg);
            continue;
        case 'c':
            printf("max_count=%s\n", optarg);
			max_count = atoi(optarg);
            continue;
        case 'i':
            printf("max_index=%s\n", optarg);
			max_index = atoi(optarg);
            continue;
        case 's':
            printf("bd_size=%s\n", optarg);
			bd_size = atoi(optarg);
            continue;
		case 'm':
            printf("num_moves=%s\n", optarg);
			num_moves = atoi(optarg);
            continue;
        default:
            printf("error opt");
            return -1;
        }
    }

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
	while (step < num_moves) {
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

