#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <fstream>

std::string get_path();

void create_file(std::string path);

void read_file(std::string path);

void register_to_scheduler();

void unregister_to_scheduler();

/// @brief The parameters are obtained according to the input parameters and the current resource usage
/// @param grid_dim optimal, will be changed
/// @param block_dim optimal, will NOT be changed
/// @param cpu_threads_num optimal, will be changed
/// @param max_index constant
void scheduled(int * grid_dim, int block_dim, int * cpu_threads_num, int max_index);