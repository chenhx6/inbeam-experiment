#include "YSort.hh"

YSort::YSort(const int run)
{
    this->run = run;
    ReadPar();
    Init();
}

void YSort::Init()
{
    ipf = nullptr;
    ipt = nullptr;
    opf = nullptr;
    opt = nullptr;
    cout << "排序原始文件：" << TString::Format("%s%s_%d.root", RAWFILEPATH, RAWFILENAME, run) << endl;
    ipf = new TFile(TString::Format("%s%s_%d.root", RAWFILEPATH, RAWFILENAME, run).Data(), "READ");
    if (!ipf->IsOpen())
    {
        cerr << "Can't open root file: " << TString::Format("%s%s_%d.root", RAWFILEPATH, RAWFILENAME, run) << endl;
        exit(-1);
    }
    ipt = (TTree *)ipf->Get("tree");
    ipt->SetBranchAddress("sr", &sr, &b_sr);
    ipt->SetBranchAddress("pileup", &pileup, &b_pileup);
    ipt->SetBranchAddress("outofr", &outofr, &b_outofr);
    ipt->SetBranchAddress("cid", &cid, &b_cid);
    ipt->SetBranchAddress("sid", &sid, &b_sid);
    ipt->SetBranchAddress("ch", &ch, &b_ch);
    ipt->SetBranchAddress("evte", &evte, &b_evte);
    ipt->SetBranchAddress("ts", &ts, &b_ts);
    ipt->SetBranchAddress("cfd", &cfd, &b_cfd);
    ipt->SetBranchAddress("cfdft", &cfdft, &b_cfdft);
    ipt->SetBranchAddress("cfds", &cfds, &b_cfds);
    ipt->SetBranchAddress("max", &max, &b_max);
    ipt->SetBranchAddress("f", &f, &b_f);
    ipt->SetBranchAddress("s", &s, &b_s);
    TotalEntry = ipt->GetEntries();
    /*读取原数据*/

    /*排序文件*/
    cout << "输出排序文件：" << TString::Format("%s%s_%d.root", SORTFILEPATH, SORTFILENAME, run) << endl;
    opf = new TFile(TString::Format("%s%s_%d.root", SORTFILEPATH, SORTFILENAME, run).Data(), "RECREATE");
    opt = new TTree("tree", "sortTs");
    opt->Branch("sr", &sr1);
    opt->Branch("cid", &cid1);
    opt->Branch("sid", &sid1);
    opt->Branch("ch", &ch1);
    opt->Branch("evte", &evte1);
    opt->Branch("ts", &ts1);
    opt->Branch("cfd", &cfd1);
    opt->Branch("cfdft", &cfdft1);
    opt->Branch("cfds", &cfds1);
    opt->Branch("total", &total);
    opt->Branch("alpha", &alpha);
    opt->Branch("proton", &proton);
}

void YSort::InitTimeBuffer()
{
    if (sr == 250)
        initTime = ts * 8;
    else
        initTime = ts * 10;
}

/**
 * @brief 进行sort排序，从小到大
 */
void YSort::Process()
{
    cutParticle = new CUT(run);
    tsnow = LLONG_MAX;
    bool firstflag = true;
    // short i = 2;
    /*读取文件*/
    for (Long64_t jentry = 0; jentry < TotalEntry; ++jentry)
    {
        ipt->GetEntry(jentry);

        if (firstflag)
        {
            InitTimeBuffer();
            firstflag = false;
        }
        if (sr == 250)
            tsnow = 8 * (ts + tsoffset[cid][sid][ch]); /*加上tsoffset*/
        else
            tsnow = 10 * (ts + tsoffset[cid][sid][ch]);
        if (TMath::Abs(tsnow - initTime) > TIMEBUFFER)
        {
            // cout << initTime << endl;
            InitTimeBuffer();
            sizelen = pq.size() * PROCESSBUFFER;
            // cout << sizelen << endl;
            // cout << tsnow << endl;
            for (size_t pqsize = 0; pqsize < sizelen; ++pqsize)
            {
                qtmp = pq.top(); // 取出堆顶
                pq.pop();        // 弹出堆顶
                FillSort(qtmp);
            }
            // if (!(i--))
            //     break;
        }
        if (jentry % 10000 == 0)
            printf("\r排序进度：%.2f%%", static_cast<float>(jentry) / TotalEntry * 100);

        qtmp.sr = sr;
        qtmp.cid = cid;
        qtmp.sid = sid;
        qtmp.ch = ch;
        qtmp.evte = evte;
        /* !!! ns !!! */
        qtmp.ts = tsnow;
        /* !!! ns !!! */
        qtmp.cfd = cfd;
        qtmp.cfdft = cfdft;
        qtmp.cfds = cfds;
        qtmp.max = max;
        qtmp.f = f;
        qtmp.s = s;
        pq.push(qtmp);
    }

    cout << endl;
    /*遍历完文件后，将剩余的填入*/
    while (!pq.empty())
    {
        qtmp = pq.top(); // 取出堆顶
        pq.pop();        // 弹出堆顶
        FillSort(qtmp);
    }

    ipf->Close();
    opf->cd();
    opt->Write();
    opf->Close();

    delete ipf;
    ipf = nullptr;
    if (opf != nullptr)
    {
        delete opf;
        opf = nullptr;
    }
    delete cutParticle;
    cutParticle = nullptr;

    CheckSort();
}

void YSort::ReadPar()
{
    /*预先置零，如何没有记录这个探测器，则不对ts修改*/
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 13; j++)
            for (int k = 0; k < 16; k++)
            {
                tsoffset[i][j][k] = 0;
                cutevte[i][j][k] = 0;
                cutgparticle[i][j][k] = true;
            }
    /*预先置零，如何没有记录这个探测器，则不对ts修改*/

    TString parfile;
    std::string str_tmp;
    short cid_tmp, sid_tmp, ch_tmp;

    /*read cutevte file*/
    parfile = TString::Format("%s%s_%d.txt", TSOFFSETFILEPATH, TSOFFSETFILENAME, (run > 1050 ? 1081 : 1035));
    cout << "read par: " << parfile << endl;
    std::ifstream readpar;
    readpar.open(parfile.Data(), std::ios::in);
    if (!readpar.is_open())
    {
        cerr << "Can't open tsoffset par:" << parfile << endl;
        exit(1);
    }
    std::getline(readpar, str_tmp);
    cout << str_tmp << endl;
    Long64_t ts_tmp;
    while (!readpar.eof())
    {
        readpar >> cid_tmp >> sid_tmp >> ch_tmp >> ts_tmp;
        if (readpar.eof())
            break;

        tsoffset[cid_tmp][sid_tmp][ch_tmp] = ts_tmp;
        cout << cid_tmp << "\t" << sid_tmp << "\t" << ch_tmp << "\t" << tsoffset[cid_tmp][sid_tmp][ch_tmp] << endl;
    }
    readpar.close();

    /*read cutevte file*/
    parfile = TString::Format("%s%s_%d.txt", CUTEFILEPATH, CUTEFILENAME, (run > 1050 ? 1081 : 1035));
    cout << "read par: " << parfile << endl;
    readpar.open(parfile.Data(), std::ios::in);
    if (!readpar.is_open())
    {
        cerr << "Can't open cutevte par:" << parfile << endl;
        exit(1);
    }
    std::getline(readpar, str_tmp);
    cout << str_tmp << endl;
    UShort_t cutevte_tmp;
    while (!readpar.eof())
    {
        readpar >> cid_tmp >> sid_tmp >> ch_tmp >> cutevte_tmp;
        if (readpar.eof())
            break;

        cutevte[cid_tmp][sid_tmp][ch_tmp] = cutevte_tmp;
        cout << cid_tmp << "\t" << sid_tmp << "\t" << ch_tmp << "\t" << cutevte[cid_tmp][sid_tmp][ch_tmp] << endl;
    }
    readpar.close();
}

void YSort::FillSort(QUEUE qtmp)
{
    sr1 = qtmp.sr;
    cid1 = qtmp.cid;
    sid1 = qtmp.sid;
    ch1 = qtmp.ch;
    evte1 = qtmp.evte;

    if (qtmp.sr == 250)
        ts1 = qtmp.ts / 8;
    else
        ts1 = qtmp.ts / 10;

    cfd1 = qtmp.cfd;
    cfdft1 = qtmp.cfdft;
    cfds1 = qtmp.cfds;
    alpha = 0;
    proton = 0;
    total = 0;
    if (cid1 == 1)
    {
        if (!GetPID(qtmp))
            if (cutgparticle[qtmp.cid][qtmp.sid][qtmp.ch])
            {
                cerr << "Check the detector cuts particles: cid[" << qtmp.cid << "]sid[" << qtmp.sid << "]ch[" << qtmp.ch << "], end!" << endl;
                cutgparticle[qtmp.cid][qtmp.sid][qtmp.ch] = false;
            }

        if (total == 0) // 带电粒子探测器无效信息，无需填充直接丢弃
            return;
    }
    opf->cd();
    opt->Fill();
}

bool YSort::GetPID(QUEUE qtmp)
{
    cutname = TString::Format("%d%d%d_%d", qtmp.cid, qtmp.sid, qtmp.ch, (run > 1050) ? 1081 : 1035);

    /*Initialize*/
    cutgalpha = nullptr;
    cutgproton = nullptr;
    cutgtotal = nullptr;
    /*Initialize*/

    cutgalpha = (TCutG *)gROOT->FindObject(((TString)("alpha" + cutname)).Data());
    /*If this detector doesn't have praticle resolution, simply return false, saving the calculation of computer*/
    if (cutgalpha == nullptr)
        return GetEPID(qtmp);
    /*If this detector doesn't have praticle resolution, simply return false, saving the calculation of computer*/

    cutgproton = (TCutG *)gROOT->FindObject(((TString)("proton" + cutname)).Data());
    cutgtotal = (TCutG *)gROOT->FindObject(((TString)("total" + cutname)).Data());

    if (cutgalpha->IsInside(qtmp.f, qtmp.s))
    { // alpha
        alpha = 1;
        proton = 0;
        total = 1;
    }
    else if (cutgproton->IsInside(qtmp.f, qtmp.s))
    { // proton
        alpha = 0;
        proton = 1;
        total = 1;
    }
    else if (cutgtotal->IsInside(qtmp.f, qtmp.s))
    { // total
        alpha = 0;
        proton = 0;
        total = 1;
    }
    // else gamma or f = 0 or s = 0
    cutgalpha = nullptr;
    cutgproton = nullptr;
    cutgtotal = nullptr;
    return true;
}

bool YSort::GetEPID(QUEUE qtmp)
{
    if (cutevte[qtmp.cid][qtmp.sid][qtmp.ch] < qtmp.evte)
    {
        alpha = 0;
        proton = 0;
        total = 1;
        return true;
    }
    else if (cutevte[qtmp.cid][qtmp.sid][qtmp.ch] > 0)
        return true;
    else
        return false;
}

void YSort::CheckSort()
{
    /*检查排序情况*/
    cout << "检查排序情况" << endl;
    Long64_t min = LLONG_MIN;
    ipf = new TFile(TString::Format("%s%s_%d.root", SORTFILEPATH, SORTFILENAME, run).Data(), "READ");
    ipt = (TTree *)ipf->Get("tree");
    ipt->SetBranchAddress("sr", &sr, &b_sr);
    ipt->SetBranchAddress("ts", &ts, &b_ts);
    TotalEntry = ipt->GetEntries();
    Long64_t tsi;
    for (Long64_t jentry = 0; jentry < TotalEntry; ++jentry)
    {
        ipt->GetEntry(jentry);

        if (sr == 250)
            tsi = ts * 8;
        else
            tsi = ts * 10;
        if (tsi < min)
        {
            ipf->Close();
            delete ipf;
            ipf = nullptr;
            ipt = nullptr;
            cerr << "排序出错: " << TString::Format("%s%s_%d.root", SORTFILEPATH, SORTFILENAME, run) << ", entry = " << jentry << endl;
            // exit(1);
        }

        min = tsi;
        if (jentry % 10000 == 0)
            printf("\r检查排序进度: %.2f%%\t", ((float)jentry) / TotalEntry * 100);
    }
    cout << endl;
    ipf->Close();
    delete ipf;
    ipf = nullptr;
    ipt = nullptr;
    cout << "检查排序结束！一起正常！" << endl;
}
