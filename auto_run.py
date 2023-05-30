import subprocess
import time
import sys
import threading
import os


# python auto_run.py {branch} {parallel} {cpu_threads_num}

arg = sys.argv
os.system("git switch " + arg[1])
os.system("make")

cmd = "ls"
parallel = int(arg[2])
filename = ""
for a in arg:
    filename += a
f = open("./data/" + filename + ".txt", "a+")


def do_loop():
    for j in range(10):
        time1 = time.time()
        result = subprocess.run(["./hybrid2", arg[3]], stdout=subprocess.PIPE)
        time2 = time.time()
        w = "{} {}".format(j, (time2 - time1))
        print(w)
        f.write(w + "\n")


# print(result.stdout.decode())
begin = time.time()
t = []
for i in range(parallel):
    ti = threading.Thread(target=do_loop)
    t.append(ti)
    ti.start()

for i in range(parallel):
    t[i].join()

end = time.time()
s = "total {}".format((end - begin))
print(s)
f.write(s + "\n")
