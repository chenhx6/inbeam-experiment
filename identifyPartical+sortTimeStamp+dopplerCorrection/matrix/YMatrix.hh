/*
 * @Date: 2023-11-07 01:42:15
 * @LastEditTime: 2023-11-09 07:54:58
 */
#ifndef _YMATRIX_H_
#define _YMATRIX_H_

#include <iostream>
#include <cstring>

#include <TChain.h>
#include <TString.h>
#include <TBranch.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TROOT.h>
#include <TMath.h>

#include "Define.hh"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

class YMatrix
{
private:
    typedef TH1F YTH1;
    typedef TH2F YTH2;
    typedef TH3I YTH3;

    /*common variable*/
    const short schargedDetectorFlag;
    const short sGeFlag;
    const short sLaBrFlag;
    int irunBegin;
    int irunEnd;
    Long64_t ltscFirst, ltscSecond;
    Long64_t ltsgFirst, ltsgSecond, ltsgThird;
    Long64_t ltsmin, ltsmax;
    short stotalHit, salphaHit, sprotonHit;
    bool bfirstLoopHit;
    bool bpamgFirstgSecond, bamgFirstgSecond, bpamgFirstgThird, bamgFirstgThird, bpamgSecondgThird, bamgSecondgThird;
    /*common variable*/

    /*ROOT variable*/
    TChain *fchain;
    TFile *fchainFile;
    TFile *fopf;
    Long64_t ltotalEntry;

    YTH1 *h1ggaptPAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH1 *h1ggatPAM[(ALPHAPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH1 *h1ggptPAM[(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH1 *h1ggapPAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)];

#ifdef GEGE
    YTH2 *h2ggaptPAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH2 *h2ggatPAM[(ALPHAPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH2 *h2ggptPAM[(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH2 *h2ggapPAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)];

#ifdef GEGEAM
    YTH1 *h1ggaptAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH1 *h1ggatAM[(ALPHAPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH1 *h1ggptAM[(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH1 *h1ggapAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)];

    YTH2 *h2ggaptAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH2 *h2ggatAM[(ALPHAPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH2 *h2ggptAM[(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH2 *h2ggapAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)];

    YTH1 *h1ggaptPM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH1 *h1ggatPM[(ALPHAPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH1 *h1ggptPM[(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH1 *h1ggapPM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)];

    YTH2 *h2ggaptPM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH2 *h2ggatPM[(ALPHAPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH2 *h2ggptPM[(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH2 *h2ggapPM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)];
#endif
#endif

#ifdef CUBEMATRIX
    YTH3 *h3ggaptPAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH3 *h3ggatPAM[(ALPHAPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH3 *h3ggptPAM[(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH3 *h3ggapPAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)];

#ifdef CUBEMATRIXAM
    YTH3 *h3ggaptAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH3 *h3ggatAM[(ALPHAPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH3 *h3ggptAM[(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH3 *h3ggapAM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)];

    YTH3 *h3ggaptPM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH3 *h3ggatPM[(ALPHAPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH3 *h3ggptPM[(PROTONPTCMAX + 1)][(TOTALPTCMAX + 1)];
    YTH3 *h3ggapPM[(ALPHAPTCMAX + 1)][(PROTONPTCMAX + 1)];
#endif
#endif

    // Declaration of leaf types
    Short_t hit;
    Short_t total[HITLENGTH];  //[hit]
    Short_t alpha[HITLENGTH];  //[hit]
    Short_t proton[HITLENGTH]; //[hit]
    Short_t sr[HITLENGTH];     //[hit]
    Short_t det[HITLENGTH];    //[hit]
    Short_t id[HITLENGTH];     //[hit]
    UShort_t raw[HITLENGTH];   //[hit]
    Double_t e[HITLENGTH];     //[hit]
    Double_t dpl[HITLENGTH];   //[hit]
    Long64_t ts[HITLENGTH];    //[hit]
    Short_t cfd[HITLENGTH];    //[hit]
    Bool_t cfdft[HITLENGTH];   //[hit]
    Short_t cfds[HITLENGTH];   //[hit]

    // List of branches
    TBranch *b_hit;    //!
    TBranch *b_total;  //!
    TBranch *b_alpha;  //!
    TBranch *b_proton; //!
    TBranch *b_sr;     //!
    TBranch *b_det;    //!
    TBranch *b_id;     //!
    TBranch *b_raw;    //!
    TBranch *b_e;      //!
    TBranch *b_dpl;    //!
    TBranch *b_ts;     //!
    TBranch *b_cfd;    //!
    TBranch *b_cfdft;  //!
    TBranch *b_cfds;   //!

    /*ROOT variable*/

    /*function*/
    /* template function*/
    // template <typename Thn> // 无法使用？
    // Thn *CreateHistogram(const TString pshname);

    template <typename Thn>
    bool StoreHistogram(Thn *&ph, const TString &psdir);

    void Init();
    bool FileExist(const TString &pschain_file_name);
    void ChainFile();
    void OutputFile();
    void InitHistogram();
    void CreateHistogram();
    bool CreateHistogram(YTH1 *&phistogram, const TString &pshname);
    bool CreateHistogram(YTH2 *&phistogram, const TString &pshname);
    bool CreateHistogram(YTH3 *&phistogram, const TString &pshname);

    bool FillHistogram(const double &pde1, const double &pde2, const short &pa, const short &pp, const short &pt, const TString &psoption, const double &pde3 = 0.0);
    bool FillPM();
    bool StoreFile();

    inline Long64_t GetTs(const Long64_t &plts, const short &pssr);
    inline bool TcoinLR(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwd);
    inline bool TcoinLR(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwdL, const Long64_t &pltwdR);
    inline bool TcoinL(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwdL);
    inline bool TcoinR(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwdR);
    inline Long64_t Max(const Long64_t &pltsi, const Long64_t &pltsj);
    inline Long64_t Min(const Long64_t &pltsi, const Long64_t &pltsj);
    inline short WhatDetector(const short &psid);
    inline bool HistogramExist(const TString &pshname);
    inline void ClearHitVar();
    inline void PtcAdd(const short &pa, const short &pp, const short &pt);

    /*function*/

public:
    YMatrix(int irunBegin, int irunEnd);
    void Process();
    ~YMatrix();
};

/**
 * @brief - This function is aimed to create one histogram
 *@brief - using method  TH1D *h1d = CreateHistogram<class_name>(hname)
 * @tparam Thn TH1, TH2, or TH3, Matches declared variables
 * @param pshname histogram name and title
 * @return Thn* pointer of created histogram named h1 + hname, h2 + hname, or h3 + hname (There is no "+" in the histogram name)
 */
// template <typename Thn>
// Thn *YMatrix::CreateHistogram(TString pshname)
// {
//     Thn *thistogram = nullptr;
//     TString shclass = thistogram->Class_Name();
//     if (!shclass.SubString("TH1").IsNull())
//         thistogram = new Thn(("h1" + pshname).Data(), (pshname + "_projection").Data(), EBINNUMBER, EMIN, EMAX);

//     else if (!shclass.SubString("TH2").IsNull())
//         thistogram = new Thn(("h2" + pshname).Data(), (pshname + "_2matrix").Data(), EBINNUMBER, EMIN, EMAX, EBINNUMBER, EMIN, EMAX);

//     else if (!shclass.SubString("TH3").IsNull())
//         thistogram = new Thn(("h3" + pshname).Data(), (pshname + "_3matrix").Data(), EBINNUMBER, EMIN, EMAX, EBINNUMBER, EMIN, EMAX, EBINNUMBER, EMIN, EMAX);

//     else
//         cerr << "Can't create " << shclass << "named " << pshname << " histogram and dimension must be lesser than 4!" << endl;

//     if (thistogram == nullptr)
//         cerr << shclass << " named " << pshname << " histogram was not created successfully" << endl;
//     return thistogram;
// }

/**
 * @brief
 * @brief This function is intended to fill one gamma to TH1, TH2 or TH3
 * @tparam Thn YTH1, YTH2, or YTH3 histogram pointor
 * @param psdir this histogram is written to the pasdir path
 * @return - true write successfully
 * @return - false write failed
 */
template <typename Thn>
bool YMatrix::StoreHistogram(Thn *&ph, const TString &psdir)
{
    fopf->cd(psdir.Data());
    if (ph != nullptr)
        if (ph->GetEntries() > LEASTENTRY)
        {
            ph->Write();
            delete ph;
            ph == nullptr;
            return true;
        }
        else
            return false;
    else
        return false;
}

#endif
