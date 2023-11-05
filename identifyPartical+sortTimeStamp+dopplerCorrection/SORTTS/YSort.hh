#ifndef _YSORT_H_
#define _YSORT_H_

#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <algorithm>
#include <climits>

#include <TROOT.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TBranch.h>

#include "Define.hh"
#include "CUT.hh"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::greater;
using std::ifstream;
using std::ios;
using std::priority_queue;
using std::vector;

struct QUEUE
{
    Short_t sr;
    // Bool_t pileup;
    // Bool_t outofr;
    Short_t cid;
    Short_t sid;
    Short_t ch;
    UShort_t evte;
    Long64_t ts;
    Short_t cfd;
    Bool_t cfdft;
    Short_t cfds;
    Double_t max;
    Double_t f;
    Double_t s;

    bool operator<(const QUEUE &a) const
    {
        return ts < a.ts;
    }
    bool operator>(const QUEUE &a) const
    {
        return ts > a.ts;
    }
};

class YSort
{
public:
    YSort(const int run);
    ~YSort(){};
    void Process();

private:
    void Init();
    void InitTimeBuffer();
    void CheckSort();
    void FillSort(QUEUE qtmp);
    bool GetPID(QUEUE qtmp);
    bool GetEPID(QUEUE qtmp);
    void ReadPar();

    /*私有非ROOT文件数据*/
    int run;
    Long64_t tsoffset[8][13][16];
    CUT *cutParticle;
    TString cutname;
    TCutG *cutgalpha;
    TCutG *cutgproton;
    TCutG *cutgtotal;
    UShort_t cutevte[8][13][16];
    bool cutgparticle[8][13][16];
    size_t sizelen;
    Long64_t tsnow;
    Long64_t initTime;
    QUEUE qtmp;
    priority_queue<QUEUE, vector<QUEUE>, greater<QUEUE>> pq;
    /*私有非ROOT数据*/

    /*读取原数据*/
    TFile *ipf;
    TTree *ipt;
    TFile *opf;
    TTree *opt;
    Long64_t TotalEntry;
    /*----------------------------------*/
    Short_t sr;
    Bool_t pileup;
    Bool_t outofr;
    Short_t cid;
    Short_t sid;
    Short_t ch;
    UShort_t evte;
    Long64_t ts;
    Short_t cfd;
    Bool_t cfdft;
    Short_t cfds;
    Double_t max;
    Double_t f;
    Double_t s;

    // List of branches
    TBranch *b_sr;     //!
    TBranch *b_pileup; //!
    TBranch *b_outofr; //!
    TBranch *b_cid;    //!
    TBranch *b_sid;    //!
    TBranch *b_ch;     //!
    TBranch *b_evte;   //!
    TBranch *b_ts;     //!
    TBranch *b_cfd;    //!
    TBranch *b_cfdft;  //!
    TBranch *b_cfds;   //!
    TBranch *b_max;    //!
    TBranch *b_f;      //!
    TBranch *b_s;      //!

    /*==================================*/
    Short_t sr1;
    Short_t cid1;
    Short_t sid1;
    Short_t ch1;
    unsigned short evte1;
    Long64_t ts1;
    Short_t cfd1;
    Bool_t cfdft1;
    Short_t cfds1;
    short total;
    short alpha;
    short proton;
    /*----------------------------------*/
};

#endif