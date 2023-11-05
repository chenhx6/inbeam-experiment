#include "YSort.hh"

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
    YSort *a = new YSort(run);
    a->Process();
    return 0;
}