**OpenCL实现矩阵乘法 **

**并做local memory优化\单工作项多任务优化\向量加载指令优化**

# Quick Start

将OpenCL 的路径写入src/opencl1/Makefile src/opencl2/Makefile...

运行`bash run.sh`

自动编译和运行



# 结果分析

运行时间(ms) 取5次运算平均值

| Matrix Size | naive OpenCl | +local memory | +工作项      | +向量加载   |
| ----------- | ------------ | ------------- | ------------ | ----------- |
| 256*256     | 0.854867 ms  | 0.080474 ms   | 0.304480 ms  | 0.077216 ms |
| 512*512     | 5.980301 ms  | 0.379469 ms   | 2.227763 ms  | 0.259955 ms |
| 1024*1024   | 54.187514 ms | 2.790298 ms   | 16.222650 ms | 1.819251 ms |

加速比(相比naive OpenCL)

| Matrix Size | naive OpenCl | +local memory | +工作项 | +向量加载 |
| ----------- | ------------ | ------------- | ------- | --------- |
| 256*256     | 1            | 10.6          | 2.8     | 11.1      |
| 512*512     | 1            | 15.8          | 2.7     | 23.0      |
| 1024*1024   | 1            | 19.4          | 3.3     | 29.8      |

## 总结

local memory 优化效果显著

在此基础上做单工作项多任务优化,反而降低了速度,原因是这种优化将原本可以并行执行的任务部分得串行化了,而串行化的任务又没有因此得到局部的性能提升，整体上看性能就下降了

在单工作项多任务优化的基础上，再使用向量加载和向量运算，就使得单个项目运算多个任务，比多个任务分开执行有更好的效率，这才让整体效率得以提升

## 运行环境

> CPU Ryzen1700 
>
> GPU GTX1070 
>
> linux-Debian10  OpenCL1.2  

# 文件结构

源文件包含如下:

```
├── Makefile					// 全局编译文件
├── README.md					
├── run.sh						// 一键编译运行脚本
└── src
    ├── opencl1					// naive OpenCL 矩阵乘法
    │   ├── Makefile
    │   ├── opencl1.cpp
    │   ├── opencl1_kernel.cl
    │   └── opencl1_opencl.h
    ├── opencl2					// opencl1+localmemory优化
    │   ├── Makefile
    │   ├── opencl2.cpp
    │   ├── opencl2_kernel.cl
    │   └── opencl2_opencl.h
    ├── opencl3					// opencl2+工作项优化
    │   ├── Makefile
    │   ├── opencl3.cpp
    │   ├── opencl3_kernel.cl
    │   └── opencl3_opencl.h
    └── opencl4					// opencl3+向量加载优化
        ├── Makefile
        ├── opencl4.cpp
        ├── opencl4_kernel.cl
        └── opencl4_opencl.h
```



编译运行后新增如下：

```
├── opencl1			// opencl1 可执行文件
├── opencl2
├── opencl3
├── opencl4
├── result.out		// 结果输出文件
└── src
    ├── opencl1
    │   └── opencl1
    ├── opencl2
    │   └── opencl2
    ├── opencl3
    │   └── opencl3
    └── opencl4
        └── opencl4
```

