---
title: "Cache模拟器"
author: "李炳睿 2018013391"
date: "2020-04-02"
output:
  pdf_document:
    latex_engine: xelatex
    number_sections: yes
    toc: yes
  word_document:
    toc: yes
  html_document:
    toc: yes
header-includes: \usepackage{ctex} \usepackage{graphicx}
---
\newpage

# 代码结构

TODO

# 缺失率统计与分析

## Cache 布局
在固定替换策略(二叉树替换), 固定写策略(写分配+写回)的前提下, 不同 Cache 布局下的缺失率(单位:1%)与元数据(单位:byte)如下所示:

|trace1  | 8B| 32B | 64B ||trace2  | 8B| 32B | 64B |
|--------|---|-----|-----|---|--------|---|-----|-----|
| direct | 4.94 | 2.20| 1.46 || direct | 2.06 | 1.33| 1.59 |
| 4-way  | 4.58 | 1.82| 1.08 || 4-way  | 1.22 | 0.306| 0.154 |
| 8-way  | 4.58| 1.82| 1.08 || 8-way  | 1.22| 0.306| 0.154 |
| full   | 4.58 | 1.82| 1.08 || full   | 1.22 | 0.306| 0.154 |

|trace3  | 8B| 32B | 64B ||trace4  | 8B| 32B | 64B |
|--------|---|-----|-----|---|--------|---|-----|-----|
| direct | 23.40| 9.84| 5.27 || direct | 3.67 | 2.31 | 1.89 |
| 4-way  | 23.28 | 9.63| 5.01 || 4-way  | 2.05 | 1.09| 0.819 |
| 8-way  | 23.29 | 9.63| 5.00 || 8-way  | 1.78 | 0.800| 0.591 |
| full   | 23.26 | 9.60| 4.98 || full   | 1.75 | 0.660 | 0.387 |

meta data TODO

分析 TODO

## 替换策略
在固定 Cache 布局(块大小 8B，8-way 组关联)，固定写策略(写分配 + 写回)的前提下, 不同替换策略下的缺失率(单位:1%)与元数据(单位:byte)如下所示:

|        | trace1  | trace2 | trace3 | trace4 | meta |
|--------|---------|--------|--------|--------|------|
| LRU    | 4.58    | 1.22   | 23.28  | 1.79   |      |
| BT     | 4.58    | 1.22   | 23.29  | 1.78   |      |
| SCORE  | 4.58    | 1.22   | 23.29  | 1.78   |      |


## 写策略
在固定 Cache 布局(块大小 8B，8-way 组关联)，固定替换策略(二叉树替换算法)的前提下, 不同写策略下的缺失率(单位:1%)与元数据(单位:byte)如下所示:

|        | trace1  | trace2 | trace3 | trace4 | meta |
|--------|---------|--------|--------|--------|------|
| 写回写分配      | 4.58    | 1.22   | 23.29  | 1.78   |      |
| 写回写不分配    | 11.15    | 8.67   | 34.66  | 4.67  |      |
| 写直达写分配    | 4.58    | 1.22   | 23.29  | 1.78   |      |
| 写直达写不分配  | 11.15    | 8.67   | 34.66  | 4.67  |      |

# 替换算法讨论

# 实验总结与收获


# 运行方式

编译方式: 在src目录下运行
```shell
g++ --std=c++11 cache.cpp policy.cpp utils.cpp main.cpp -o main
```
运行方式:
```
./main
Usage: ./main [src_trace]
Usage: ./main [src_trace] [rp] [w0] [w1]
Usage: ./main [src_trace] [rp] [w0] [w1] [dst]

例如
./main ../test_trace/1.trace BT back alloc ../out/dst.log
```
- 此种运行方式可以设置Cache模拟器的 替换策略、写策略以及源trace文件
- 若要修改Cache布局, 请在`config.h`中修改`WAY_NUM`和`LINE_SIZE`的值, 在第 28 与 29 行
- 提醒: 请按照文档中所要求的配置进行测试, 对于文档中未要求的配置, 不保证得到正确结果.

集成测试: 在src/下运行
```shell
python run.py [n]
```
- 其中n可以取值3/4/5, 分别对应文档中实验流程3/4/5中的要求
- 提醒: 在使用此种方式前, 请确保所有源文件未被改动