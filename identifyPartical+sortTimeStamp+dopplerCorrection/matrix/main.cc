/*
 * @Date: 2023-11-07 01:42:57
 * @LastEditTime: 2023-11-10 13:12:36
 */
#include "YMatrix.hh"

int main(int argc, char *argv[])
{
    TString srunNumberBegin;
    TString srunNumberEnd; // [begin, end]
    cout << "Process file from begining file  to end file! " << endl;
    if (argc == 3)
    {
        srunNumberBegin = argv[1];
        srunNumberEnd = argv[2];
    }
    else if (argc == 2)
    {
        srunNumberBegin = argv[1];
        srunNumberEnd = argv[1];
    }
    else
    {
        cout << "请输入起始和终止文件编号(用空格分开)：";
        cin >> srunNumberBegin;
        cin >> srunNumberEnd;
    }

    int irunBegin = srunNumberBegin.Atoi();
    int irunEnd = srunNumberEnd.Atoi();
    cout << "run file number from " << irunBegin << " to " << irunEnd << endl;
    YMatrix *a = new YMatrix(irunBegin, irunEnd);
    a->Process();
    return 0;
}