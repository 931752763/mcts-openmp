import subprocess
import time
import sys
import threading
import os, getopt


# python auto_run.py -b {branch} -p {parallel_num} -l {loop_num} -c {cpu_threads_num}
branch = ""
parallel_num, loop_num, cpu_threads_num = 0, 0, 0
opts,args = getopt.getopt(sys.argv[1:],'-b:-p:-l:-c:')
for opt_name,opt_value in opts:
    if opt_name in ('-b'):
        branch = opt_value
    if opt_name in ('-p'):
        parallel_num = int(opt_value)
    if opt_name in ('-l'):
        loop_num = int(opt_value)
    if opt_name in ('-c'):
        cpu_threads_num = opt_value

filename = "{}_{}_{}_{}.txt".format(branch, parallel_num, loop_num, cpu_threads_num)
print(filename)
f = open("./data/" + filename, "a+")

os.system("git switch " + branch)
os.system("make")


def do_loop():
    for j in range(loop_num):
        time1 = time.time()
        result = subprocess.run(["./hybrid2", cpu_threads_num], stdout=subprocess.PIPE)
        time2 = time.time()
        w = "{} {}".format(j, (time2 - time1))
        print(w)
        f.write(w + "\n")


# print(result.stdout.decode())
begin = time.time()
t = []
for i in range(parallel_num):
    ti = threading.Thread(target=do_loop)
    t.append(ti)
    ti.start()

for i in range(parallel_num):
    t[i].join()

end = time.time()
s = "total {}".format((end - begin))
print(s)
f.write(s + "\n")
