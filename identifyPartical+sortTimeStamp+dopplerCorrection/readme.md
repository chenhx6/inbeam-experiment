# 流程
> decode后的文件-> Fit波形 -> SORTTS处理波形数据 -> Hit刻度加多普勒修正 -> matrix做addback和整理矩阵 
## SORTTS
> 处理波形分析后的数据快慢成分得到total, alpha, proton
> 探测器分辨不好的用能量给出total带电粒子
### 功能
- 对探测器做出时间修正
- 鉴别触发带电粒子探测器的粒子种类
- 按照时间递增排序输出文件
## HIT
> 把sortts的处理数据用hit结构存储
### 功能
- 能量刻度
- 多普勒修正
- addback
- 处理为hit结构的数组
## matrix
- 有两个版本，均无法完成3维矩阵的建立，官方人员推荐用valgrind检查内存
    - 一个数组存储指针
    - 一个用ROOT内部函数寻找指针，较慢
