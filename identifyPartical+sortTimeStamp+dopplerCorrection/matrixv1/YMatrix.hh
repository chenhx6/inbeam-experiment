/*
 * @Date: 2023-11-07 01:42:15
 * @LastEditTime: 2023-11-21 20:27:02
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
#include <TH3I.h>
#include <TROOT.h>
#include <TMath.h>
#include <TCanvas.h>

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
    typedef TH3F YTH3;
    int th3count;
    /*common variable*/
    const short kschargedDetectorFlag;
    const short ksGeFlag;
    const short ksLaBrFlag;
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

    TString shggaptname;
    TString shggatname;
    TString shggptname;
    TString shggapname;

    bool bapt[(ALPHAPTCMAX - ALPHAPTCMIN + 1)][(PROTONPTCMAX - PROTONPTCMIN + 1)][(TOTALPTCMAX - TOTALPTCMIN + 1)];
    bool bat[(ALPHAPTCMAX - ALPHAPTCMIN + 1)][(TOTALPTCMAX - TOTALPTCMIN + 1)];
    bool bpt[(PROTONPTCMAX - PROTONPTCMIN + 1)][(TOTALPTCMAX - TOTALPTCMIN + 1)];
    bool bap[(ALPHAPTCMAX - ALPHAPTCMIN + 1)][(PROTONPTCMAX - PROTONPTCMIN + 1)];

    YTH1 *h1ggaptPAM;
    YTH1 *h1ggatPAM;
    YTH1 *h1ggptPAM;
    YTH1 *h1ggapPAM;

    YTH1 *h1ggaptAM;
    YTH1 *h1ggatAM;
    YTH1 *h1ggptAM;
    YTH1 *h1ggapAM;

    YTH1 *h1ggaptPM;
    YTH1 *h1ggatPM;
    YTH1 *h1ggptPM;
    YTH1 *h1ggapPM;

#ifdef GEGE
    YTH2 *h2ggaptPAM;
    YTH2 *h2ggatPAM;
    YTH2 *h2ggptPAM;
    YTH2 *h2ggapPAM;
#ifdef GEGEAM
    YTH2 *h2ggaptAM;
    YTH2 *h2ggatAM;
    YTH2 *h2ggptAM;
    YTH2 *h2ggapAM;

    YTH2 *h2ggaptPM;
    YTH2 *h2ggatPM;
    YTH2 *h2ggptPM;
    YTH2 *h2ggapPM;
#endif
#endif

#ifdef CUBEMATRIX
    YTH3 *h3ggaptPAM;
    YTH3 *h3ggatPAM;
    YTH3 *h3ggptPAM;
    YTH3 *h3ggapPAM;
#ifdef CUBEMATRIXAM
    YTH3 *h3ggaptAM;
    YTH3 *h3ggatAM;
    YTH3 *h3ggptAM;
    YTH3 *h3ggapAM;

    YTH3 *h3ggaptPM;
    YTH3 *h3ggatPM;
    YTH3 *h3ggptPM;
    YTH3 *h3ggapPM;
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
    void Init();

    bool FileExist(const TString &pschain_file_name);
    void ChainFile();
    void OutputFile();
    void CreateHistogram();
    bool CreateHistogram(YTH1 *phistogram, const TString &pshname);
    bool CreateHistogram(YTH2 *phistogram, const TString &pshname);
    bool CreateHistogram(YTH3 *phistogram, const TString &pshname);
    bool FindHistogram(YTH1 *&ph, const TString &pshname);
    bool FindHistogram(YTH2 *&ph, const TString &pshname);
    bool FindHistogram(YTH3 *&ph, const TString &pshname);
    bool FillHistogram(const bool &pbsym, const double &pde1, const double &pde2, const short &pa, const short &pp, const short &pt, const TString &psoption, const double &pde3 = 0.0);
    bool FillPM();
    bool StoreFile();

    inline void InitBool();
    inline void InitPtr();
    inline void InitBP();
    inline void InitHitVar();
    inline void InitHitgpam();
    inline void InitHitgam();

    inline Long64_t GetTs(const Long64_t &plts, const short &pssr);
    inline bool TcoinLR(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwd);
    inline bool TcoinLR(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwdL, const Long64_t &pltwdR);
    inline bool TcoinL(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwdL);
    inline bool TcoinR(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwdR);
    inline Long64_t Max(const Long64_t &pltsi, const Long64_t &pltsj);
    inline Long64_t Min(const Long64_t &pltsi, const Long64_t &pltsj);
    inline short WhatDetector(const short &psid);

    inline void PtcAdd(const short &pa, const short &pp, const short &pt);
    inline void StoreHistogram(YTH1 *&ph, const TString &psdir);
    inline void StoreHistogram(YTH2 *&ph, const TString &psdir);
    inline void StoreHistogram(YTH3 *&ph, const TString &psdir);
    /*function*/

public:
    YMatrix(int irunBegin, int irunEnd);
    void Process();
    ~YMatrix();
};

#endif