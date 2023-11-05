#ifndef _YHIT_H_
#define _YHIT_H_

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
#include <TBranch.h>
#include <TRandom.h>
#include <TMath.h>

#include "Define.hh"

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
    short det;
    short id;
    short sr;
    UShort_t raw;
    double e;
    double dpl;
    Long64_t ts;
    short cfd;
    bool cfdft;
    short cfds;
    short total;
    short alpha;
    short proton;

    bool operator<(const QUEUE &a) const
    {
        return ts < a.ts;
    }
    bool operator>(const QUEUE &a) const
    {
        return ts > a.ts;
    }
};
class YHit
{
private:
    /*INPUT FILE*/
    TFile *ipf;
    TTree *ipt;
    // Declaration of leaf types
    Short_t sr;
    Short_t cid;
    Short_t sid;
    Short_t ch;
    UShort_t evte;
    Long64_t ts;
    Short_t cfd;
    Bool_t cfdft;
    Short_t cfds;
    Short_t total;
    Short_t alpha;
    Short_t proton;

    // List of branches
    TBranch *b_sr;     //!
    TBranch *b_cid;    //!
    TBranch *b_sid;    //!
    TBranch *b_ch;     //!
    TBranch *b_evte;   //!
    TBranch *b_ts;     //!
    TBranch *b_cfd;    //!
    TBranch *b_cfdft;  //!
    TBranch *b_cfds;   //!
    TBranch *b_total;  //!
    TBranch *b_alpha;  //!
    TBranch *b_proton; //!
    /*INPUT FILE*/

    /*OUTPUT FILE*/
    TFile *opf;
    TTree *opt;
    short hit;
    short det1[HITLENGTH];
    short id1[HITLENGTH];
    short sr1[HITLENGTH];
    UShort_t raw1[HITLENGTH];
    double e1[HITLENGTH];
    double dpl[HITLENGTH];
    Long64_t ts1[HITLENGTH];
    short cfd1[HITLENGTH];
    bool cfdft1[HITLENGTH];
    short cfds1[HITLENGTH];
    short total1[HITLENGTH];
    short alpha1[HITLENGTH];
    short proton1[HITLENGTH];

    QUEUE qtmp;
    priority_queue<QUEUE, vector<QUEUE>, greater<QUEUE>> pq;
    /*OUTPUT FILE*/

    /*common private variable*/
    int run;
    Long64_t TotalEntry;
    Long64_t initTime;
    Long64_t tsnow;
    short flagdet[8][13][16];
    short flagid[8][13][16];
    double cali[8][13][16][3];
    double E;

    double beta[8][13][16];
    double radian[8][13][16];

    /*Addback private variable*/
    // Addbackclovertotal[cloverNumber][crystalNumber];
    short Addbackclovertotal[CLOVERNUM][4];
    short Addbackcloveralpha[CLOVERNUM][4];
    short Addbackcloverproton[CLOVERNUM][4];
    short Addbackcloversr[CLOVERNUM][4];
    short Addbackcloverdet[CLOVERNUM][4];
    short Addbackcloverid[CLOVERNUM][4];
    short Addbackcloverraw[CLOVERNUM][4];
    double Addbackcloverdpl[CLOVERNUM][4];
    double Addbackclovere[CLOVERNUM][4];
    Long64_t Addbackcloverts[CLOVERNUM][4];
    short Addbackclovercfd[CLOVERNUM][4];
    short Addbackclovercfds[CLOVERNUM][4];
    bool Addbackclovercfdft[CLOVERNUM][4];
    short Addbackcloverhit[CLOVERNUM];
    /*Addback private variable*/

    /*Addback private function*/
    inline bool IsHPGe(short idi);
    inline bool IsClover(short idi);
    inline bool IsLaBr(short idi);
    short gaidadj(short idi, short idj);
    void AddbackRead();
    void AddbackGo();
    bool Addback2crystal(const short deti, const short idi, const short idj);
    void clearoptAddback();
    void clearclovercrystal(const short deti, const short idi);
    /*Addback private function*/

    /*common private function*/
    void clearoptHit();
    void InitEvent();
    void Init();
    void EndEvent();
    void FillFile();
    void ProcessEntry();
    bool gtcoin(Long64_t tsi, const short sri, Long64_t tsj, const short srj, const Long64_t tscoinwd);
    void ReadPar(const int run);
    /*common private function*/

public:
    YHit(const int run);
    void Process();
    ~YHit()
    {
        // cout << "ipf" << endl;
        delete ipf;
        // cout << "opf" << endl;
        delete opf;
        // cout << "delete" << endl;
    };
};

#endif