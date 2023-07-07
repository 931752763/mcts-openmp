#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <fstream>
#include <httplib.h>

#include "get_schedule.hpp"

std::string host_port = "http://localhost:8888";

void register_to_scheduler()
{
    pid_t pid = getpid();
    httplib::Params params;
    params.emplace("pid", std::to_string(pid));
    if (auto res = httplib::Client(host_port).Post("/register", params))
    {
        // cout << res->status << endl;
        // cout << res->get_header_value("Content-Type") << endl;
        printf("body: %s\n", res->body);
    }
    else
    {
        printf("http error: %s", res.error());
    }
}

void unregister_to_scheduler()
{
    pid_t pid = getpid();
    httplib::Params params;
    params.emplace("pid", std::to_string(pid));
    if (auto res = httplib::Client(host_port).Post("/unregister", params))
    {
        // cout << res->status << endl;
        // cout << res->get_header_value("Content-Type") << endl;
        printf("body: %s\n", res->body);
    }
    else
    {
        printf("http error: %s", res.error());
    }
}

void get_schedule(int *grid_dim, int block_dim, int *cpu_threads_num, int max_index)
{
    pid_t pid = getpid();
    std::string path = "/schedule/" + std::to_string(pid);
    if (auto res = httplib::Client(host_port).Get(path))
    {
        // cout << res->status << endl;
        // cout << res->get_header_value("Content-Type") << endl;
        printf("body: %s\n", res->body);
    }
    else
    {
        printf("http error: %s", res.error());
    }
    double gpu_quota = stod(str1);
    double cpu_quota = stod(str2);

    static int GRID_DIM = 2048;
    static int BLOCK_DIM = 1;
    static int CPU_THREADS_NUM = 32;
    static int MAX_INDEX = 100;
    double g = (double)(GRID_DIM) * (double)BLOCK_DIM;
    double c = (double)(CPU_THREADS_NUM) * (double)MAX_INDEX;
    double c_new = (cpu_quota / gpu_quota) * (double)c;
    double total = g + c;
    double gpu_threads = total / (g + c_new) * g;
    double cpu_threads = total / (g + c_new) * c_new;
    *grid_dim = int(gpu_threads / (double)BLOCK_DIM);
    *cpu_threads_num = int(cpu_threads / (double)max_index);
}