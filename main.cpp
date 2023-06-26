#include <stdio.h>
#include "CudaGo.h"
#include "mcts.h"
#include "point.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#define TIME_EACH_MOVE 2*60*1000 // ms

int main(int argc, char *argv[]) {
    int cpu_threads_num = 64;
	int rst_threads_num = 16;
	int max_count = 10;
	int max_index = 10;
	int bd_size = 9;
	int opt;
	int num_moves = 100;
	int grid_dim = 2048;
	int block_dim = 1;
    const char *optstring = "hn:c:i:s:m:r:";
	struct option opts[] = {
        {"help", optional_argument, NULL, 'h'},
        {"cpu_threads_num", optional_argument, NULL, 'n'},
        {"max_count", optional_argument, NULL, 'c'},
        {"max_index", optional_argument, NULL, 'i'},
		{"bd_size", optional_argument, NULL, 's'},
		{"num_moves", optional_argument, NULL, 'm'},
		{"rst_threads_num", optional_argument, NULL, 'r'},
		{"grid_dim", optional_argument, NULL, "g"},
		{"block_dim", optional_argument, NULL, "b"},
        {0, 0, 0, 0},
    };
    while((opt = getopt_long(argc, argv, optstring, opts, NULL)) != -1)
    {
        switch(opt)
        {
		case 'h':
			printf("-n: cpu_threads_num, number of threads in parallel area\n"
			"-c: max_count, it affects the GPU computation time\n"
			"-i: max_index, it affects the CPU computation time\n"
			"-s: bd_size, the size of board\n"
			"-m: num_moves, determines how many moves the two players will make\n"
			"-r: rst_threads_num, nested parallel number in function run_simulation_thread\n");
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
		case 'r':
            printf("rst_threads_num=%s\n", optarg);
			rst_threads_num = atoi(optarg);
            continue;
		case 'g':
            printf("grid_dim=%s\n", optarg);
			grid_dim = atoi(optarg);
            continue;
		case 'b':
            printf("block_dim=%s\n", optarg);
			block_dim = atoi(optarg);
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
		p = player1->run(cpu_threads_num, rst_threads_num, max_count, max_index, grid_dim, block_dim);
		end = clock();
		player1_time += (double)(end - start) / CLOCKS_PER_SEC;
		step++;
		printf("player1 : (%d,%d)\n", p.i, p.j);
		seq.push_back(p);
		board.update_board(p);
		board.print_board();

		player2 = new Mcts(GPU, bd_size, TIME_EACH_MOVE, seq);
		start = clock();
		p = player2->run(cpu_threads_num, rst_threads_num, max_count, max_index, grid_dim, block_dim);
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

