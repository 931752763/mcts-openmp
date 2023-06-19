import subprocess
import time
import sys
import threading
import os, getopt
import os.path
from openpyxl import Workbook, load_workbook
import psutil
from nvitop import *

def create_excel(file):
    wb = Workbook()
    ws = wb.active
    title = ["branch", "cpu_threads_num", "omp_num_threads", "rst_threads_num", "time (s)", 
             "thread_id", "parallel_num", "cmd", "error_flag", "start_time", "end_time",
             "gpu_sm_utilization", "gpu_memory", "cpu_percent", "host_memory"]
    ws.append(title)
    wb.save(file)

def write_excel(file, new_row):
    with lock:
        wb = load_workbook(file)
        ws = wb.active
        ws.append(new_row)
        wb.save(file)
        
def record_usage(pid, usage):
    while psutil.pid_exists(pid):
        try:
            devices = Device.all()  # or `Device.all()` to use NVML ordinal instead
            for device in devices:
                processes = device.processes()
                if len(processes) <= 0:
                    continue
                processes = GpuProcess.take_snapshots(processes.values(), failsafe=True)
                for p in processes:
                    if p.pid != pid:
                        continue
                    usage["gpu_sm_utilization"].append(p.gpu_sm_utilization)
                    usage["gpu_memory"].append(p.gpu_memory)
                    usage["cpu_percent"].append(HostProcess(pid).cpu_percent(interval=0.1))
                    usage["host_memory"].append(p.host_memory)
                    break
        except:
            break
    

def do_loop():
    rst = 0
    if branch == "openmp-depend-modify-rst":
        rst = rst_threads_num
    print("branch {}, parallel_num {}, cpu_threads_num {}, omp_num_threads {}, rst_threads_num {}, loop_num {}, board size {}".
          format(branch, parallel_num, cpu_threads_num, omp_num_threads, rst, loop_num, bd_size))
    # file = data_txt.format(branch, parallel_num, cpu_threads_num, omp_num_threads)
    for j in range(loop_num):
        begin = time.time()
        begin_str = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(begin))
        cmd = ["./hybrid2", "-n", str(cpu_threads_num), "-s", str(bd_size), "-c 10", "-i 200", "-m 10", "-r", str(rst)]
        process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        # process = subprocess.Popen(["ls /bin"], shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        pid = process.pid
        
        usage = {"gpu_sm_utilization":[], "gpu_memory":[], "cpu_percent":[], "host_memory":[]}
        thread_record = threading.Thread(target=record_usage, args=(pid, usage, ))
        thread_record.start()
        
        res = process.communicate()
        end = time.time()
        end_str = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(end))
        new_row = [branch, cpu_threads_num, omp_num_threads, rst, (end - begin), pid, parallel_num, str(cmd)]
        if process.returncode != 0:
            new_row.append("error")
            print(res)
        else:
            new_row.append("ok")
        new_row.append(begin_str)
        new_row.append(end_str)
        print(new_row)
        # record actual num of thread
        thread_record.join()
        for key in usage:
            val = usage[key]
            val_str = str(val)
            new_row.append(val_str[1:len(val_str)-1])
        write_excel(file, new_row)

def run():
    # print("check gcc version")
    # process = subprocess.run(["gcc", "--version"], stdout=subprocess.PIPE)
    # print(process.stdout)
    
    # file = data_txt.format(branch, parallel_num, cpu_threads_num, omp_num_threads)
    if not os.path.isfile(file):
        create_excel(file)
    
    t = []
    for i in range(parallel_num):
        ti = threading.Thread(target=do_loop)
        t.append(ti)
        ti.start()

    for i in range(parallel_num):
        t[i].join()
        
    # print("sleep 5 min to seperate")
    # time.sleep(300)

# python auto_run.py -b {branch_list} -l {loop_num} -p {parallel_num} 
# -c {cpu_threads_num_list} -o {omp_num_threads_list} -r {rst_threads_num_list} -s {board_size_list} -f {file_name}
branch_list = []
loop_num = 20
parallel_num_list = []
cpu_threads_num_list = []
omp_num_threads_list = []
rst_threads_num_list = []
board_size_list = []
file = "../data/test.xlsx"
opts,args = getopt.getopt(sys.argv[1:],'-b:-l:-p:-c:-o:-r:-s:-f:')
for opt_name,opt_value in opts:
    if opt_name in ('-b'):
        branch_list = opt_value.split(" ")
    if opt_name in ('-l'):
        loop_num = int(opt_value)
    if opt_name in ('-p'):
        parallel_num_list = list(map(int, opt_value.split(" ")))
    if opt_name in ('-c'):
        cpu_threads_num_list = list(map(int, opt_value.split(" ")))
    if opt_name in ('-o'):
        omp_num_threads_list = list(map(int, opt_value.split(" ")))
    if opt_name in ('-r'):
        rst_threads_num_list = list(map(int, opt_value.split(" ")))
    if opt_name in ('-s'):
        board_size_list = list(map(int, opt_value.split(" ")))
    if opt_name in ('-f'):
        file = "../data/{}.xlsx".format(opt_value)

lock = threading.Lock()
for branch in branch_list:
    os.system("git checkout " + branch)
    os.system("make")
    # data_txt = "../data/{}_{}_threads={}_pool={}.xlsx"

    if "pthread-norand" == branch:
        for bd_size in board_size_list:
            for parallel_num in parallel_num_list:
                for cpu_threads_num in cpu_threads_num_list:
                    omp_num_threads = 0
                    run()
                
    if "openmp-depend" == branch:
        for bd_size in board_size_list:
            for parallel_num in parallel_num_list:
                for cpu_threads_num in cpu_threads_num_list:
                    for omp_num_threads in omp_num_threads_list:
                        print("cpu_threads_num {}, omp_num_threads{}".format(cpu_threads_num, omp_num_threads))
                        os.environ["OMP_NUM_THREADS"] = str(omp_num_threads)
                        run()
    
    if "openmp-depend-modify-rst" == branch:
        os.environ["OMP_NESTED"] = "TRUE"
        for bd_size in board_size_list:
            for parallel_num in parallel_num_list:
                for cpu_threads_num in cpu_threads_num_list:
                    for omp_num_threads in omp_num_threads_list:
                        for rst_threads_num in rst_threads_num_list:
                            print("cpu_threads_num {}, omp_num_threads{}, rst_threads_num{}".format(cpu_threads_num, omp_num_threads, rst_threads_num))
                            os.environ["OMP_NUM_THREADS"] = "{}, {}".format(omp_num_threads, rst_threads_num)
                            run()
                
