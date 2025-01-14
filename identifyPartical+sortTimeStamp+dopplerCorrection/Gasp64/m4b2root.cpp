/***
 * @Date: 2024-04-30 21:47:44
 * @LastEditTime: 2024-08-31 16:00:41
 */
/***
 * @Date: 2024-04-30 21:47:44
 * @LastEditTime: 2024-08-31 15:37:02
 */
#include <iostream>
using namespace std;
void m4b2root()
{
    TString m4bName("g90allmatprompt");
    TH2I *gg = new TH2I(m4bName.Data(), m4bName.Data(), 4096, 0, 2048, 4096, 0, 2048);
    FILE *m4bfile;
    m4bfile = fopen("/mnt/e/imp/imp/Data/iThemba_133Ce/133Ce_iThemba2019/mat/m4b/" + m4bName + ".m4b", "rb");
    if (m4bfile == nullptr)
    {
        cerr << "Can't open m4b file" << endl;
        exit(-1);
    }
    int binx = gg->GetNbinsX();
    int bincontent = 0;
    for (int i = 1; i <= binx; ++i)
    {
        for (int j = 1; j <= binx; ++j)
        {
            if (fread(&bincontent, sizeof(int), 1, m4bfile) != 1)
            {
                if (!feof(m4bfile))
                    cout << "Error reading file!" << endl;
                else
                    cout << "Finish reading file" << endl;
                cout << i << "*" << j << endl;
                break;
            }
            gg->SetBinContent(j, i, bincontent);
        }
    }

    if (fread(&bincontent, sizeof(int), 1, m4bfile) != 1)
    {
        if (!feof(m4bfile))
            cout << "Error reading file!" << endl;
        else
            cout << "Finish reading file" << endl;
    }

    if (fclose(m4bfile))
        cerr << "Can't close file" << endl;

    TFile *ipf = new TFile(m4bName + ".root", "RECREATE");
    ipf->cd();
    gg->Write();
    ipf->Close();
}