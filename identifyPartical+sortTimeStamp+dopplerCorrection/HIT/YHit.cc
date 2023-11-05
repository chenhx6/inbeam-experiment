#include "YHit.hh"

YHit::YHit(const int run)
{
    this->run = run;
    ReadPar(run);
    Init();
}

void YHit::Init()
{
    ipf = nullptr;
    ipt = nullptr;
    opf = nullptr;
    opt = nullptr;
    ipf = new TFile(TString::Format("%s%s_%d.root", RAWFILEPATH, RAWFILENAME, run), "READ");
    if (!ipf->IsOpen())
    {
        cerr << "Can't open root file: " << TString::Format("%s%s_%d.root", RAWFILEPATH, RAWFILENAME, run) << endl;
        exit(-1);
    }
    ipt = (TTree *)ipf->Get("tree");
    ipt->SetBranchAddress("sr", &sr, &b_sr);
    ipt->SetBranchAddress("cid", &cid, &b_cid);
    ipt->SetBranchAddress("sid", &sid, &b_sid);
    ipt->SetBranchAddress("ch", &ch, &b_ch);
    ipt->SetBranchAddress("evte", &evte, &b_evte);
    ipt->SetBranchAddress("ts", &ts, &b_ts);
    ipt->SetBranchAddress("cfd", &cfd, &b_cfd);
    ipt->SetBranchAddress("cfdft", &cfdft, &b_cfdft);
    ipt->SetBranchAddress("cfds", &cfds, &b_cfds);
    ipt->SetBranchAddress("total", &total, &b_total);
    ipt->SetBranchAddress("alpha", &alpha, &b_alpha);
    ipt->SetBranchAddress("proton", &proton, &b_proton);
    TotalEntry = ipt->GetEntries();

#ifdef DOPPLERCORRECTION
#ifdef ADDBACK
    opf = new TFile(TString::Format("%s%s_W%lld_%d_addback_doppler.root", ROOTFILEPATH, ROOTFILENAME, EVENTTIMEWINDOWSWIDTH, run), "RECREATE");
#else
    opf = new TFile(TString::Format("%s%s_W%lld_%d_doppler.root", ROOTFILEPATH, ROOTFILENAME, EVENTTIMEWINDOWSWIDTH, run), "RECREATE");
#endif
#else
    opf = new TFile(TString::Format("%s%s_W%lld_%d_nodoppler.root", ROOTFILEPATH, ROOTFILENAME, EVENTTIMEWINDOWSWIDTH, run), "RECREATE");
#endif

    opt = new TTree("tree", TString::Format("EVENT HIT[%d]", HITLENGTH).Data());
    opt->Branch("hit", &hit, "hit/S");
    opt->Branch("total", &total1, "total[hit]/S");
    opt->Branch("alpha", &alpha1, "alphal[hit]/S");
    opt->Branch("proton", &proton1, "proton[hit]/S");
    opt->Branch("sr", &sr1, "sr[hit]/S");
    opt->Branch("det", &det1, "det[hit]/S");
    opt->Branch("id", &id1, "id[hit]/S");
    opt->Branch("raw", &raw1, "raw[hit]/s");
    opt->Branch("e", &e1, "e[hit]/D");

#ifdef DOPPLERCORRECTION
    opt->Branch("dpl", &dpl, "dpl[hit]/D");
#endif

    opt->Branch("ts", &ts1, "ts[hit]/L");
    opt->Branch("cfd", &cfd1, "cfd[hit]/S");
    opt->Branch("cfdft", &cfdft1, "cfdft[hit]/O");
    opt->Branch("cfds", &cfds1, "cfds[hit]/S");
}

void YHit::Process()
{
    Long64_t nevt = 0LL;
    bool flag = false;
    for (Long64_t jentry = 0; jentry < TotalEntry; ++jentry)
    {
        ipt->GetEntry(jentry);

        if (jentry % 1000 == 0)
        {
            cout << "entry: " << jentry << " | event: " << nevt << " | TotalEntry: " << TotalEntry << endl;
            cout << "\033[1A\033[K";
        }

        if (flag)
        {
            if (sr == 250)
                tsnow = ts * 8;
            else
                tsnow = ts * 10;

            if (TMath::Abs(tsnow - initTime) <= EVENTTIMEWINDOWSWIDTH)
            {
                ProcessEntry();
            }
            else
            {
                FillFile();
                ++nevt;
                InitEvent();
            }
        }
        else
        {
            flag = true;
            InitEvent();
        }
    }
    cout << "event: " << (nevt + 1) << " | TotalEntry: " << TotalEntry << endl;
    FillFile();
    opt->Write();
    opf->Close();
    ipf->Close();
    cout << "Finish!" << endl;
}

void YHit::InitEvent()
{
    clearoptHit();
    if (sr == 250)
        initTime = 8 * ts;
    else
        initTime = 10 * ts;

    ProcessEntry();
}

void YHit::ProcessEntry()
{
    total1[hit] = total;
    alpha1[hit] = alpha;
    proton1[hit] = proton;
    sr1[hit] = sr;
    det1[hit] = flagdet[cid][sid][ch];
    id1[hit] = flagid[cid][sid][ch];
    raw1[hit] = evte;
    E = raw1[hit] + gRandom->Rndm();
    if (det1[hit] == 0 && id1[hit] == 1)
    {
        e1[hit] = -2.94696 + 0.405638 * E - 4.8969E-05 * E * E + 2.15015E-08 * E * E * E;
    }
    else
    {
        // 普遍探测器刻度
        e1[hit] = cali[cid][sid][ch][0] + cali[cid][sid][ch][1] * E + cali[cid][sid][ch][2] * E * E;
    }

#ifdef DOPPLERCORRECTION
    dpl[hit] = e1[hit] * (1 - beta[cid][sid][ch] * TMath::Cos(radian[cid][sid][ch])) / TMath::Sqrt(1 - beta[cid][sid][ch] * beta[cid][sid][ch]);
#endif

    ts1[hit] = ts;
    cfd1[hit] = cfd;
    cfdft1[hit] = cfdft;
    cfds1[hit] = cfds;
    hit++;
}

void YHit::EndEvent()
{
    AddbackRead();
    AddbackGo();
    for (short i = 0; i < hit; i++)
    {
        if (IsClover(id1[i]))
            continue;

        qtmp.det = det1[i];
        qtmp.id = id1[i];
        qtmp.sr = sr1[i];
        qtmp.raw = raw1[i];
        qtmp.e = e1[i];
        qtmp.dpl = dpl[i];

        if (qtmp.sr == 250)
            qtmp.ts = ts1[i] * 8;
        else
            qtmp.ts = ts1[i] * 10;

        qtmp.cfd = cfd1[i];
        qtmp.cfdft = cfdft1[i];
        qtmp.cfds = cfds1[i];
        qtmp.total = total1[i];
        qtmp.alpha = alpha1[i];
        qtmp.proton = proton1[i];
        pq.push(qtmp);
    }
    for (short i = 0; i < CLOVERNUM; i++)
    {
        for (short j = 0; j < 4; j++)
        {
            if (Addbackcloverdpl[i][j] < 1e-8)
                continue;

            qtmp.det = Addbackcloverdet[i][j];
            qtmp.id = Addbackcloverid[i][j];
            qtmp.sr = Addbackcloversr[i][j];
            qtmp.raw = Addbackcloverraw[i][j];
            qtmp.e = Addbackclovere[i][j];
            qtmp.dpl = Addbackcloverdpl[i][j];

            if (qtmp.sr == 250)
                qtmp.ts = Addbackcloverts[i][j] * 8;
            else
                qtmp.ts = Addbackcloverts[i][j] * 10;

            qtmp.cfd = Addbackclovercfd[i][j];
            qtmp.cfdft = Addbackclovercfdft[i][j];
            qtmp.cfds = Addbackclovercfds[i][j];
            qtmp.total = Addbackclovertotal[i][j];
            qtmp.alpha = Addbackcloveralpha[i][j];
            qtmp.proton = Addbackcloverproton[i][j];
            pq.push(qtmp);
        }
    }

    clearoptHit();

    while (!pq.empty())
    {
        qtmp = pq.top(); // 取出堆顶
        pq.pop();        // 弹出堆顶
        det1[hit] = qtmp.det;
        id1[hit] = qtmp.id;
        sr1[hit] = qtmp.sr;
        raw1[hit] = qtmp.raw;
        e1[hit] = qtmp.e;
        dpl[hit] = qtmp.dpl;

        if (qtmp.sr == 250)
            ts1[hit] = qtmp.ts / 8;
        else
            ts1[hit] = qtmp.ts / 10;

        cfd1[hit] = qtmp.cfd;
        cfdft1[hit] = qtmp.cfdft;
        cfds1[hit] = qtmp.cfds;
        total1[hit] = qtmp.total;
        alpha1[hit] = qtmp.alpha;
        proton1[hit] = qtmp.proton;
        hit++;
    }
}

void YHit::FillFile()
{
#ifdef DOPPLERCORRECTION
#ifdef ADDBACK
    EndEvent();
#endif
#endif

    if (hit == 0) // 空事件
        return;
    opf->cd();
    opt->Fill();
}

/**
 * @brief Get whether two crystals are time-coincidence
 * @brief - class Long64_t
 * @brief - unit ns
 *
 * @param tsi The frist crystal is hit time stamp
 * @param sri tsi acquistion card frequency
 * @param tsj The second crystal is hit time stamp
 * @param srj tsj acquistion card frequency
 * @param tscoinwd Two crystals is thought to coincide with the time window
 *
 * @return - true: TMath::Abs(tsi - tsj) <= tscoinwd
 * @return - false: TMath::Abs(tsi - tsj) > tscoinwd
 */
bool YHit::gtcoin(Long64_t tsi, const short sri, Long64_t tsj, const short srj, const Long64_t tscoinwd)
{
    if (sri == 250)
        tsi *= 8;
    else
        tsi *= 10;

    if (srj == 250)
        tsj *= 8;
    else
        tsj *= 10;

    return TMath::Abs(tsi - tsj) <= tscoinwd;
}

/**
 * @brief - Get whether two crystals are belong to one clover and are adjacent
 * @brief - This function is special for this experiment 'clover id: 17 - 36'. In this experiment, the id:17 - 20 are described the small clover which is positioned in a different direction than the other clovers(id: 21 - 36)
 * @brief -  -- beam -->
 * @brief - small clover
 * @brief - 0 | 1
 * @brief - 3 | 2
 * @brief - other clover
 * @brief - 3 | 0
 * @brief - 2 | 1
 * @param idi short: The first crystal id number --greater 16
 * @param idj short: The second crtstal id number --greater 16
 *
 * @return short
 * @return - 0: They are not belong to one clover or are not adjacent
 * @return - 1: They ara adjacent  and perpendicular to the beam
 * @return - 2: They ara adjacent and parallel to the beam
 */
short YHit::gaidadj(short idi, short idj)
{
    // HPGe or LaBr_3
    if (!IsClover(idi))
        return 0;
    if (!IsClover(idj))
        return 0;

    idi -= 17;
    idj -= 17;
    // idi and idj come from different Clover
    if ((idi / 4) != (idj / 4))
        return 0;

    if (TMath::Abs((idi % 4) + (idj % 4)) == 3)
    {
        if (idi / 4 == 0) // small clover
            return 1;
        else
            return 2;
    }
    else if (TMath::Abs((idi % 4) - (idj % 4)) == 1)
    {
        if (idi / 4 == 0)
            return 2;
        else
            return 1;
    }
    else
        return 0; // no adjacent
}

/**
 * @brief - 读入clover信息;
 * @brief - 同一个探测器在1000ns中被击中两次的概率为万分之一(仅带电粒子探测器)；clover没有二次击中
 * ge的波形时间是300us，csi的波形时间是200us；
 * 如果被击中两次，就是pileup事件，直接给出的信息也是错误的
 */
void YHit::AddbackRead()
{
    clearoptAddback();
    for (int i = 0; i < CLOVERNUM; i++)
        Addbackcloverhit[i] = 0;

    short idi = 0;
    short deti = 0;
    short crystali = 0;
    for (short i = 0; i < hit; i++)
    {
        // HPGe || LaBr_3 || invalid messages
        if ((!IsClover(id1[i])) || (dpl[i] < 1e-8))
            continue;

        idi = id1[i] - 17;
        deti = idi / 4;
        crystali = idi % 4;
        Addbackclovertotal[deti][crystali] = total1[i];
        Addbackcloveralpha[deti][crystali] = alpha1[i];
        Addbackcloverproton[deti][crystali] = proton1[i];
        Addbackcloversr[deti][crystali] = sr1[i];
        Addbackcloverdet[deti][crystali] = det1[i];
        Addbackcloverid[deti][crystali] = id1[i];
        Addbackcloverraw[deti][crystali] = raw1[i];
        Addbackcloverdpl[deti][crystali] = dpl[i];
        Addbackclovere[deti][crystali] = e1[i];
        Addbackcloverts[deti][crystali] = ts1[i];
        Addbackclovercfd[deti][crystali] = cfd1[i];
        Addbackclovercfds[deti][crystali] = cfds1[i];
        Addbackclovercfdft[deti][crystali] = cfdft1[i];
    }
    for (int i = 0; i < CLOVERNUM; i++)
        for (int j = 0; j < 4; j++)
            if (Addbackcloverdpl[i][j] > 1e-8)
                Addbackcloverhit[i]++; // prevention greater then 4
}

/**
 * @brief - 对读入一次hit结构的数据分析，在 EVENTTIMEWINDOWSWIDTH （目前选的是1000ns）时间窗口内，某一个探测器被击中次数及其处理方案
 * 击中次数 -=> 处理方案
 * @brief - 统一处理：在该函数中对clover进行逐个循环。对某一个clover被击中的情况处理如下：
 * @brief - 0 -=> 在对dpl能量判断时，可使得idcoinnumMax(某一块晶体符合其他块晶体的最多次数) = 0，不处理，循环下个clover。
 * @brief - 1 -=> 同0情况。
 * @brief - 2 -=> 可以进入一次gtcoin判断，真：idcoinnumMax = 1, 处理这一对晶体；假：idcoinnumMax = 0, 来自不同事件的伽马射线，不处理。
 * @brief - 3 -=> 对着三个晶体判断，若均没有时间符合，则来自三个伽马射线；若有一对符合，则idcoinnumMax = 1, 处理用idijcoin[6]记录的两个晶体，另一个不处理；若有2对{例如(0, 1) and (1, 2)}及以上符合，则idcoinnumMax > 1, 删去探测器信息（同时hit数-1）
 * @brief - 4 -=> 对4个晶体判断，同3。idcoinnumMax = 0 { 4个事件的gamma}、idcoinnumMax = 1 {三个事件或者两个事件的gamma，对来自同一个事件的gamma做出处理}、idcoinnumMax = 2 {删去有关系的三个探测器}、idcoinnumMax = 3 {全有问题}
 */
void YHit::AddbackGo()
{
    for (short i = 0; i < CLOVERNUM; ++i)
    {
        short idcoinnum[4] = {0, 0, 0, 0}; // idcoinnum[colverNumber]
        short idcoinnumMax = 0;

        // idijcoin[0] = idi * 4 + idj; idi != idj -=> idijcoin[0] != 0
        short idijcoin[6] = {0, 0, 0, 0, 0, 0};
        short idijcoinnum = 0;

        // The purpose of this loop is to find out whether any two crystals coincide
        for (short j = 0; j < 4; j++)
        {
            // Floating-point or double-precision numbers are close to zero !! NOT EQUAL ZERO !!
            if (Addbackcloverdpl[i][j] < 1e-8)
                continue;

            for (short k = (j + 1); k < 4; k++)
            {
                if (Addbackcloverdpl[i][k] < 1e-8)
                    continue;

                if (gtcoin(Addbackcloverts[i][j], Addbackcloversr[i][j], Addbackcloverts[i][k], Addbackcloversr[i][k], CLOVERADDBACKWINDOW))
                {
                    idcoinnum[j]++;
                    idcoinnum[k]++;

                    idijcoin[idijcoinnum] = j * 4 + k;
                    idijcoinnum++;
                }
            }
        }

        for (short j = 0; j < 4; j++)
            if (idcoinnumMax <= idcoinnum[j])
                idcoinnumMax = idcoinnum[j];

        if (idcoinnumMax == 1)
        {
            for (short j = 0; j < 6; j++)
                if (idijcoin[j])
                    Addback2crystal(i, (idijcoin[j] / 4), (idijcoin[j] % 4));
        }
        else if (idcoinnumMax > 1)
        {
            for (short j = 0; j < 4; j++)
                if (idcoinnum[j])
                {
                    clearclovercrystal(i, j);
                    Addbackcloverhit[i]--;
                }
        }
    }
}

/**
 * @brief Get the two crystals addback messages, and clear one of them
 *
 * @param deti  clover number
 * @param idi one crystal number of the clover {0, 1, 2, 3}
 * @param idj one crystal number of the clover {0, 1, 2, 3}
 * @return bool
 * @return - true: idi and idj are adjacent, addback is finished, and are initialized to zero on one crystal
 * @return - false: idi and idj are not adjacent, are initialized zero
 */
bool YHit::Addback2crystal(const short deti, const short idi, const short idj)
{
    short AddbackcloverMaxdplE;
    short AddbackcloverMindplE;
    short adjacent = gaidadj(Addbackcloverid[deti][idi], Addbackcloverid[deti][idj]);
    if (adjacent)
    {
        if (Addbackcloverdpl[deti][idi] > Addbackcloverdpl[deti][idj])
        {
            AddbackcloverMaxdplE = idi;
            AddbackcloverMindplE = idj;
        }
        else
        {
            AddbackcloverMaxdplE = idj;
            AddbackcloverMindplE = idi;
        }
        /* use the maximum energy crystal to output other message (fake message except time stamp) */
        Addbackcloverdpl[deti][AddbackcloverMaxdplE] = Addbackcloverdpl[deti][idi] + Addbackcloverdpl[deti][idj];
        Addbackclovere[deti][AddbackcloverMaxdplE] = Addbackclovere[deti][idi] + Addbackclovere[deti][idj];
        /* use the maximum energy crystal to output other message (fake message except time stamp) */

        /*Initialize minimum energy crystal*/
        clearclovercrystal(deti, AddbackcloverMindplE);
        Addbackcloverhit[deti]--;
        /*Initialize minimum energy crystal*/
        return true;
    }
    else
    {
        clearclovercrystal(deti, idi);
        Addbackcloverhit[deti]--;

        clearclovercrystal(deti, idj);
        Addbackcloverhit[deti]--;

        return false;
    }
}

/**
 * @brief determine whether it is high pure germanium
 *
 * @param idi the true id, {1, 2, ..., 15, 16}
 * @return true Yes
 * @return false No
 */
inline bool YHit::IsHPGe(short idi)
{
    return idi < 17;
}

/**
 * @brief determine whether it is clover
 *
 * @param idi the true id, {17, 18, ..., 35, 36}
 * @return true Yes
 * @return false No
 */
inline bool YHit::IsClover(short idi)
{
    return (idi > 16 && idi < 37);
}

/**
 * @brief determine whether it is LaBr_3
 *
 * @param idi the true id, {37, 38, ..., 45, 46}
 * @return true Yes
 * @return false No
 */
inline bool YHit::IsLaBr(short idi)
{
    return idi > 36;
}

/// @brief Initialize hit structure arrays to zero includer 'hit' variable
void YHit::clearoptHit()
{
    /*Initialize hit structure*/
    hit = 0;
    for (short i = 0; i < HITLENGTH; i++)
    {
        total1[i] = 0;
        alpha1[i] = 0;
        proton1[i] = 0;
        sr1[i] = 0;
        det1[i] = 0;
        id1[i] = 0;
        cfd1[i] = 0;
        cfds1[i] = 0;
        raw1[i] = 0;
        e1[i] = 0.0;
        dpl[i] = 0.0;
        ts1[i] = 0;
        cfdft1[i] = false;
    }
    /*Initialize hit structure*/
}

/// @brief Initialize addback arrays to zero
void YHit::clearoptAddback()
{
    /*Initialize addback structure*/
    for (int i = 0; i < CLOVERNUM; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            clearclovercrystal(i, j);
        }
    }
    /*Initialize addback structure*/
}
/**
 * @brief Initialize one array element to zero
 *
 * @param deti colver number
 * @param idi crystal number {0, 1, 2, 3}
 */
void YHit::clearclovercrystal(const short deti, const short idi)
{
    Addbackclovertotal[deti][idi] = 0;
    Addbackcloveralpha[deti][idi] = 0;
    Addbackcloverproton[deti][idi] = 0;
    Addbackcloversr[deti][idi] = 0;
    Addbackcloverdet[deti][idi] = 0;
    Addbackcloverid[deti][idi] = 0;
    Addbackcloverraw[deti][idi] = 0;
    Addbackcloverdpl[deti][idi] = 0.0;
    Addbackclovere[deti][idi] = 0.0;
    Addbackcloverts[deti][idi] = 0;
    Addbackclovercfd[deti][idi] = 0;
    Addbackclovercfds[deti][idi] = 0;
    Addbackclovercfdft[deti][idi] = false;
}

void YHit::ReadPar(const int run)
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 13; j++)
            for (int k = 0; k < 16; k++)
            {
                flagdet[i][j][k] = -1;
                flagid[i][j][k] = -1;
                for (int m = 0; m < 3; m++)
                {
                    cali[i][j][k][m] = 0.;
                }
            }

#ifdef DOPPLERCORRECTION
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 13; j++)
            for (int k = 0; k < 16; k++)
            {
                beta[i][j][k] = 0.0;
                radian[i][j][k] = TMath::PiOver2();
            }
    double beta_tmp, radian_tmp;
#endif

    short cid_tmp, sid_tmp, ch_tmp;
    short det_tmp, id_tmp;
    double a0_tmp, a1_tmp, a2_tmp;
    std::string str_tmp;

#ifdef DOPPLERCORRECTION
    ifstream readpar1(TString::Format("par_total_%d_doppler.dat", (run > 1050) ? 1081 : 1035).Data());
#else
    ifstream readpar1(TString::Format("par_total_%d_nodoppler.dat", (run > 1050) ? 1081 : 1035).Data());
#endif

    if (!readpar1.is_open())
    {
        cerr << "Can't open cali par" << endl;
        exit(1);
    }
    getline(readpar1, str_tmp);
    cout << str_tmp << endl;
    while (!readpar1.eof())
    {

#ifdef DOPPLERCORRECTION
        readpar1 >> cid_tmp >> sid_tmp >> ch_tmp >> det_tmp >> id_tmp >> a0_tmp >> a1_tmp >> a2_tmp >> beta_tmp >> radian_tmp;
#else
        readpar1 >> cid_tmp >> sid_tmp >> ch_tmp >> det_tmp >> id_tmp >> a0_tmp >> a1_tmp >> a2_tmp;
#endif
        if (readpar1.eof())
            break;

        if (det_tmp == -1)
            continue;
        if (id_tmp == -1)
            continue;

        flagdet[cid_tmp][sid_tmp][ch_tmp] = det_tmp;
        flagid[cid_tmp][sid_tmp][ch_tmp] = id_tmp;
        cali[cid_tmp][sid_tmp][ch_tmp][0] = a0_tmp;
        cali[cid_tmp][sid_tmp][ch_tmp][1] = a1_tmp;
        cali[cid_tmp][sid_tmp][ch_tmp][2] = a2_tmp;

        cout << cid_tmp << "\t" << sid_tmp << "\t" << ch_tmp << "\t" << flagdet[cid_tmp][sid_tmp][ch_tmp] << "\t" << flagid[cid_tmp][sid_tmp][ch_tmp];
        for (short i = 0; i < 3; i++)
            cout << "\t" << cali[cid_tmp][sid_tmp][ch_tmp][i];

#ifdef DOPPLERCORRECTION
        radian[cid_tmp][sid_tmp][ch_tmp] = radian_tmp;
        beta[cid_tmp][sid_tmp][ch_tmp] = beta_tmp;
        cout << "\t" << beta_tmp << "\t" << radian_tmp << endl;
#else
        cout << endl;
#endif
    }
}