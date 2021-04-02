# help to run the problem2
import os
import sys
rps = ['LRU', 'BT', 'Score', 'Direct']
w0s = ['back', 'through']
w1s = ['alloc', 'non-alloc']
comp = 'g++ --std=c++11 cache.cpp policy.cpp utils.cpp main.cpp -o main'

def run_exp4():
    print(comp)
    os.system(comp)
    task = 4
    for i in (0, 1, 2):
        rp = rps[i]
        for t in (1, 2, 3, 4):
            src = '../test_trace/%d.trace' % t
            dst = '../out/exp%d_8B_8way_%s_back_alloc_trace%d' % (task, rp, t)
            cmd = './main %s %s back alloc %s > %s' %(src, rp, dst + '.out', dst + '.log')
            print(cmd)
            os.system(cmd)

def run_exp5():
    print(comp)
    os.system(comp)
    task = 5
    rp = rps[1]
    for i in range(2):
        w0 = w0s[i]
        for j in range(2):
            w1 = w1s[j]
            for t in (1, 2, 3, 4):
                src = '../test_trace/%d.trace' % t
                dst = '../out/exp%d_8B_8way_%s_%s_%s_trace%d' % (task, rp, w0, w1, t)
                cmd = './main %s %s %s %s %s > %s' %(src, rp, w0, w1, dst + '.out', dst + '.log')
                print(cmd)
                os.system(cmd)


def run_exp3():
    cache_size = 0x20000
    task = 3
    rp = rps[1]
    w0 = w0s[0]
    w1 = w1s[0]
    config = './config.h'
    for block in (8, 32, 64):
        for idx in (1, 4, 8, -1):
            way = idx if (idx != -1) else cache_size / block
            rp = rps[3] if (way == 1) else rps[1]
            lines = []
            lines_before = []
            with open(config, 'r') as f:
                for line in f.readlines():
                    lines_before.append(line)
                    if (line[0:5] == '/*L*/'):
                        line = line.replace('8', str(block))
                        print(line)
                    if (line[0:5] == '/*W*/'):
                        line = line.replace('8', str(way))
                        print(line)
                    lines.append(line)
            # file edits
            with open(config, 'w+') as f:
                f.writelines(lines)
            # file edited, run the cmd
            print(comp)
            os.system(comp)
            for t in (1, 2, 3, 4):
                src = '../test_trace/%d.trace' % t
                dst = '../out/exp%d_%dB_%dway_%s_%s_%s_trace%d' % (task, block, way, rp, w0, w1, t)
                cmd = './main %s %s %s %s %s > %s' %(src, rp, w0, w1, dst + '.out', dst + '.log')
                print(cmd)
                os.system(cmd)
            # file restored
            with open(config, 'w+') as f:
                f.writelines(lines_before)



# print(comp)
if __name__ == "__main__":
    task = int(sys.argv[1])
    if task == 3:
        run_exp3()
    elif task == 4:
        run_exp4()
    elif task == 5:
        run_exp5()
    else:
        print('task should be 3/4/5')
        exit(-1)
