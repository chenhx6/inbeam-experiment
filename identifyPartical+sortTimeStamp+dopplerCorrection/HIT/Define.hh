/*
 * @Date: 2023-10-26 17:34:27
 * @LastEditTime: 2023-11-09 17:33:44
 */
#ifndef _DEFINE_H_
#define _DEFINE_H_

/*ROOT数据文件*/
#define FILESIZEMIN 20 // MB
#define RAWFILEPATH "/mnt/e/GC_sort/"
#define RAWFILENAME "sort_ts"

#define ROOTFILEPATH "/mnt/e/GC_event/"
#define ROOTFILENAME "event_hit"
/*ROOT数据文件*/

/*时间窗口*/
#define EVENTTIMEWINDOWSWIDTH 1000LL // ns
#define HITLENGTH 100

/*多普勒修正开关*/
#define DOPPLERCORRECTION

/*Addback*/
#define ADDBACK // To define ADDBACK, one must first define DOPPLERCORRECTION
#define CLOVERNUM 5
#define CLOVERADDBACKWINDOW 90

#endif
