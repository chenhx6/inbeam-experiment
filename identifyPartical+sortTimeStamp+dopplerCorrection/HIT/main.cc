/*
 * @Date: 2023-10-26 17:35:20
 * @LastEditTime: 2023-11-01 13:00:07
 */
#include "YHit.hh"

int main(int argc, char *argv[])
{
    TString runNumber;
    if (argc == 2)
    {
        runNumber = argv[1];
    }
    else
    {
        cout << "请输入运行文件编号：";
        cin >> runNumber;
    }

    int run = runNumber.Atoi();
    YHit a(run);
    // cout << "11" << endl;
    a.Process();
    // cout << "22" << endl;
    return 0;
}