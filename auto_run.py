import subprocess
import time
import sys
import threading
import os, getopt
from openpyxl import Workbook, load_workbook

# python auto_run.py -b {branch} -p {parallel_num} -c {cpu_threads_num_list}
branch = ""
parallel_num = 0
cpu_threads_num_list = []
opts,args = getopt.getopt(sys.argv[1:],'-b:-p:-c:')
for opt_name,opt_value in opts:
    if opt_name in ('-b'):
        branch = opt_value
    if opt_name in ('-p'):
        parallel_num = int(opt_value)
    if opt_name in ('-c'):
        cpu_threads_num_list = list(map(int, opt_value.split(" ")))

os.system("git checkout " + branch)
os.system("make")
print("parallel_num: {}, cpu_threads_num_list: {}".format(parallel_num, cpu_threads_num_list))
data_txt = "../data/{}_{}_{}.xlsx"

def do_loop(cpu_threads_num):
    wb = Workbook()
    ws = wb.active
    file = data_txt.format(branch, parallel_num, cpu_threads_num)
    for j in range(10):
        time1 = time.time()
        result = subprocess.run(["./hybrid2", str(cpu_threads_num)], stdout=subprocess.PIPE)
        # result = subprocess.run(["ls"], stdout=subprocess.PIPE)
        time2 = time.time()
        new_row = [os.getpid(), threading.get_native_id(), (time2 - time1)]
        print(new_row)
        ws.append(new_row)
        wb.save(file)

for cpu_threads_num in cpu_threads_num_list: 
    begin = time.time()
    t = []
    for i in range(parallel_num):
        ti = threading.Thread(target=do_loop, args=(cpu_threads_num, ))
        t.append(ti)
        ti.start()

    for i in range(parallel_num):
        t[i].join()

    end = time.time()
    s = "total {}".format((end - begin))
    print(s)
