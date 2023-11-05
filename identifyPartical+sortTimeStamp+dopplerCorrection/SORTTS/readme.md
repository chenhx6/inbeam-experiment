# 主要修改内容在Define.hh
# CUT.hh && CUT.cc存储粒子鉴别信息
- 注意在YSort.cc中定义gcut的位置，`TObject::FindObject()`函数的搜索深度有限
- 如何想更改CUT类的定义位置，可以更改`TObeject::FindObject()`函数为`TObeject::FindObjectAny()`
- 利用CUT类给出带电粒子total（总带电粒子数），alpha（阿尔法粒子数），proton（质子数）
# cutevte.txt
- 存储粒子分辨能力不好的探测器能量（道值表示）
- $\gamma$在带电粒子探测器中沉积能量与带电粒子沉积能量区别很大
- 判断 $\gamma$ 触发阈值，仅区别带电粒子和$\gamma$
