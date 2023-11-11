# 该文件将会做出二维和三维矩阵
## 均可以单独做出
> TH3无法fill， bug未清除
```c
===========================================================
There was a crash.
This is the entire stack trace of all threads:
===========================================================
#0  0x00007f7dee8eb60c in waitpid () from /lib64/libc.so.6
#1  0x00007f7dee868f62 in do_system () from /lib64/libc.so.6
#2  0x00007f7df35cb5d4 in TUnixSystem::StackTrace (this=0x653980) at /home/inbeam/Downloads/root-6.16.00/core/unix/src/TUnixSystem.cxx:2413
#3  0x00007f7df35cdd0c in TUnixSystem::DispatchSignals (this=0x653980, sig=kSigSegmentationViolation) at /home/inbeam/Downloads/root-6.16.00/core/unix/src/TUnixSystem.cxx:3644
#4  <signal handler called>
#5  0x00007f7df24ce63e in TH3I::AddBinContent (this=0x1ed02e0, bin=305689502) at /home/inbeam/Downloads/root-6.16.00/hist/hist/src/TH3.cxx:3867
#6  0x00007f7df24d7752 in TH3::Fill (this=0x1ed02e0, x=117.79139476246776, y=<optimized out>, z=<optimized out>) at /home/inbeam/Downloads/root-6.16.00/hist/hist/src/TH3.cxx:311
#7  0x0000000000402c2b in YMatrix::FillHistogramSYM (this=this
entry=0x6f7df0, pde1=
0x7005c0: 117.79139476246776, pde2=
0x7005c8: 539.6078183572024, pa=
0x6f7e4a: 0, pp=
0x6f7e4c: 1, pt=
0x6f7e48: 1, psoption=..., pde3=
0x7005d0: 75.390072817876188) at YMatrix.cc:495
#8  0x0000000000408171 in YMatrix::Process (this=this
entry=0x6f7df0) at YMatrix.cc:1086
#9  0x00000000004021f5 in main (argc=<optimized out>, argv=<optimized out>) at main.cc:33
===========================================================


The lines below might hint at the cause of the crash.
You may get help by asking at the ROOT forum http://root.cern.ch/forum
Only if you are really convinced it is a bug in ROOT then please submit a
report at http://root.cern.ch/bugs Please post the ENTIRE stack trace
from above as an attachment in addition to anything else
that might help us fixing this issue.
===========================================================
#5  0x00007f7df24ce63e in TH3I::AddBinContent (this=0x1ed02e0, bin=305689502) at /home/inbeam/Downloads/root-6.16.00/hist/hist/src/TH3.cxx:3867
#6  0x00007f7df24d7752 in TH3::Fill (this=0x1ed02e0, x=117.79139476246776, y=<optimized out>, z=<optimized out>) at /home/inbeam/Downloads/root-6.16.00/hist/hist/src/TH3.cxx:311
#7  0x0000000000402c2b in YMatrix::FillHistogramSYM (this=this
entry=0x6f7df0, pde1=
0x7005c0: 117.79139476246776, pde2=
0x7005c8: 539.6078183572024, pa=
0x6f7e4a: 0, pp=
0x6f7e4c: 1, pt=
0x6f7e48: 1, psoption=..., pde3=
0x7005d0: 75.390072817876188) at YMatrix.cc:495
#8  0x0000000000408171 in YMatrix::Process (this=this
entry=0x6f7df0) at YMatrix.cc:1086
#9  0x00000000004021f5 in main (argc=<optimized out>, argv=<optimized out>) at main.cc:33
```
## 单谱是必须做出的
## 源文件为HIT文件输出的dpl修正后的文件
> 注意点
> 1. 探测器的id编号在函数已经写死
> 2. 创建单直方图的模板无法使用，bug未清除
