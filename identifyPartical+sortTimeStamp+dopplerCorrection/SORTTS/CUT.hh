/*
 * @Date: 2023-09-09 05:04:39
 * @LastEditTime: 2023-10-08 13:39:22
 */
#ifndef _CUT_H_
#define _CUT_H_

#include <TCutG.h>
#include <vector>
class CUT
{
public:
    TCutG *cutg;
    std::vector<TCutG *> cutgList;
    CUT(const int run);
    void cut1801();
    void cut1035(){};
    ~CUT(){};
};
#endif