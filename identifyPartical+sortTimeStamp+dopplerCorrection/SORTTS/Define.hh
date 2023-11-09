/*
 * @Date: 2023-10-26 16:59:42
 * @LastEditTime: 2023-11-03 21:38:11
 */
#ifndef _DEFINE_H_
#define _DEFINE_H_

/*ROOT数据文件*/
#define FINESIZEMIN 20  //MB
#define RAWFILEPATH "/mnt/e/GC_fit/"
#define RAWFILENAME "fit_gamma_csi"

#define SORTFILEPATH "/mnt/e/GC_sort/"
#define SORTFILENAME "sort_ts"
/*ROOT数据文件*/

/*排序缓冲-时间单位*/
#define SORTTIMEMAX 3
#define TSOFFSETFILEPATH "./"
#define TSOFFSETFILENAME "tsoffset"
#define TIMEBUFFER 5e10    // 排列数据长度（纳秒单位）
#define PROCESSBUFFER 0.75 // (0,1] 每次写入排列好的数据（比例）
/*排序缓冲-时间单位*/

/*能量卡带电粒子*/
#define CUTEFILEPATH "./"
#define CUTEFILENAME "cutevte"
/*能量卡带电粒子*/

#endif
