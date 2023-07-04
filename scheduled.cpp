#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <fstream>

#include "scheduled.hpp"

using namespace std;

// 共享内存 暂时不用
// void* get_shm_p(const char * _key)
// {
//     key_t key = ftok(_key, 200);
//     // get the share memory
//     int shmid = shmget(key, 0, 0);
//     if(shmid == -1)
//     {
//         perror("shmget failed\n");
//         exit(1);
//     }
//     // map share memory to get the virtual address
//     void *p = shmat(shmid, 0, 0);
//     if((void *)-1 == p)
//     {
//         perror("shmat failed");
//         exit(2);
//     }
//     return p;
// }

string get_path()
{
    pid_t pid = getpid();
    printf("pid: %d\n", pid);
    string path = "../register/" + to_string(pid);
    return path;
}

void create_file(string path)
{
    fstream ob;
    ob.open(path, ios::out);
    ob.close();
    return;
}

void register_to_scheduler()
{
    string path = get_path();
    create_file(path);
}

void unregister_to_scheduler()
{
    string path = get_path();
    int status = remove(path.c_str());
    printf("status: %d \n", status);
}

void scheduled(int *grid_dim, int block_dim, int *cpu_threads_num, int max_index)
{
    string path = get_path();

    fstream ob;
    string str1;
    ob.open(path, ios::in);
    while (str1.empty())
    {
        ob.close();
        ob.open(path, ios::in);
        ob >> str1;
    }
    double gpu_quota = stod(str1);
    string str2;
    ob >> str2;
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