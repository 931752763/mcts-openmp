import subprocess
import time
import sys
import threading
import os, getopt
import os.path
from openpyxl import Workbook, load_workbook

def create_excel(file):
    wb = Workbook()
    ws = wb.active
    title = ["thread_id", "time (s)"]
    ws.append(title)
    wb.save(file)

def write_excel(file, new_row):
    with lock:
        wb = load_workbook(file)
        ws = wb.active
        ws.append(new_row)
        wb.save(file)

def do_loop(cpu_threads_num, parallel_num):
    print("mcts cpu_threads_num: {}, parallel_num: {}, mcts will execute {} times in sequence".format(cpu_threads_num, parallel_num, loop_num))
    file = data_txt.format(branch, parallel_num, cpu_threads_num)
    for j in range(loop_num):
        time1 = time.time()
        process = subprocess.Popen(["./hybrid2", str(cpu_threads_num), "10", "10"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        # process = subprocess.Popen(["ls /bing"], shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        res = process.communicate()
        # print(res)
        time2 = time.time()
        if process.returncode != 0:
            new_row = ["{}-error".format(process.pid), (time2 - time1)]
        else:
            new_row = [process.pid, (time2 - time1)]
        print(new_row)
        write_excel(file, new_row)


# python auto_run.py -b {branch_list} -l {loop_num} -p {parallel_num} -c {cpu_threads_num_list}
branch_list = []
loop_num = 20
parallel_num_list = []
cpu_threads_num_list = []
opts,args = getopt.getopt(sys.argv[1:],'-b:-l:-p:-c:')
for opt_name,opt_value in opts:
    if opt_name in ('-b'):
        branch_list = opt_value.split(" ")
    if opt_name in ('-l'):
        loop_num = int(opt_value)
    if opt_name in ('-p'):
        parallel_num_list = list(map(int, opt_value.split(" ")))
    if opt_name in ('-c'):
        cpu_threads_num_list = list(map(int, opt_value.split(" ")))


for branch in branch_list:
    os.system("git checkout " + branch)
    os.system("make")
    print("parallel_num_list: {}, cpu_threads_num_list: {}".format(parallel_num_list, cpu_threads_num_list))
    data_txt = "../data/{}_{}_{}.xlsx"

    for cpu_threads_num in cpu_threads_num_list:
        for parallel_num in parallel_num_list:
            print("check gcc version")
            process = subprocess.run(["gcc", "--version"], stdout=subprocess.PIPE)
            print(process.stdout)
            
            file = data_txt.format(branch, parallel_num, cpu_threads_num)
            if not os.path.isfile(file):
                create_excel(file)
            
            begin = time.time()
            begin_str = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(begin))
            t = []
            lock = threading.Lock()
            for i in range(parallel_num):
                ti = threading.Thread(target=do_loop, args=(cpu_threads_num, parallel_num, ))
                t.append(ti)
                ti.start()

            for i in range(parallel_num):
                t[i].join()

            end = time.time()
            end_str = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(end))
            new_row = [begin_str, end_str]
            write_excel(file, new_row)
            s = "total {}".format((end - begin))
            print(s)
            # print("sleep 5 min to seperate")
            # time.sleep(300)
