/***
 * @Date: 2023-09-12 08:47:27
 * @LastEditTime: 2023-10-08 13:52:40
 */
// Linkdef.h ---
//
// Description:
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com
// Created: 一 9月 21 16:30:02 2020 (+0800)
// Last-Updated: 日 12月  4 00:29:08 2022 (+0800)
//           By: Hongyi Wu(吴鸿毅)
//     Update #: 2
// URL: http://wuhongyi.cn

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class QUEUE + ;
#pragma link C++ class vector < QUEUE> + ;
// #pragma link C++ class NODE + ;
// #pragma link C++ class vector < NODE> + ;

#endif

//
// Linkdef.h ends here
