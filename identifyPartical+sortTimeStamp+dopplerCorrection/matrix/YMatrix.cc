/*
 * @Date: 2023-11-07 01:42:15
 * @LastEditTime: 2023-11-22 19:16:36
 */
#include "YMatrix.hh"

YMatrix::YMatrix(int irunBegin, int irunEnd) : kschargedDetectorFlag(0), ksGeFlag(1), ksLaBrFlag(2)
{
    this->irunBegin = irunBegin;
    this->irunEnd = irunEnd;
}

void YMatrix::Init()
{
    fchain = nullptr;
    fchainFile = nullptr;
    fopf = nullptr;
    fchain = new TChain("tree");
    ChainFile();
    fchain->SetBranchAddress("hit", &hit, &b_hit);
    fchain->SetBranchAddress("total", total, &b_total);
    fchain->SetBranchAddress("alpha", alpha, &b_alpha);
    fchain->SetBranchAddress("proton", proton, &b_proton);
    fchain->SetBranchAddress("sr", sr, &b_sr);
    fchain->SetBranchAddress("det", det, &b_det);
    fchain->SetBranchAddress("id", id, &b_id);
    fchain->SetBranchAddress("raw", raw, &b_raw);
    fchain->SetBranchAddress("e", e, &b_e);
    fchain->SetBranchAddress("dpl", dpl, &b_dpl);
    fchain->SetBranchAddress("ts", ts, &b_ts);
    fchain->SetBranchAddress("cfd", cfd, &b_cfd);
    fchain->SetBranchAddress("cfdft", cfdft, &b_cfdft);
    fchain->SetBranchAddress("cfds", cfds, &b_cfds);
    ltotalEntry = fchain->GetEntries();
    OutputFile();
    CreateHistogram();
    cout << "Histogram have be created!" << endl;
}

/**
 * @brief Determine  file which name is parameter is exists
 *
 * @param pschain_file_name  file name
 * @return - true YES
 * @return - false NO
 */
bool YMatrix::FileExist(const TString &pschain_file_name)
{
    fchainFile = new TFile(pschain_file_name.Data(), "READ");

    if (fchainFile->IsOpen())
    {
        if ((fchainFile->GetSize() / 1024 / 1024) < FILESIZEMIN)
        {
            cerr << "Open wrong file: " << pschain_file_name << endl;
            fchainFile->Close();
            delete fchainFile;
            fchainFile = nullptr;
            return false;
        }
        else
        {
            fchainFile->Close();
            delete fchainFile;
            fchainFile = nullptr;
            return true;
        }
    }
    else
    {
        cerr << "Can't read file: " << pschain_file_name << endl;
        fchainFile->Close();
        delete fchainFile;
        fchainFile = nullptr;
        return false;
    }
}

/**
 * @brief concatenate multiple files together
 *
 */
void YMatrix::ChainFile()
{
#ifdef DOPPLERCORRECTION
#define DOPPLERFILENAME DOPPLERFILE
#else
#define DOPPLERFILE ""
#endif

#ifdef ADDBACK
#define ADDBACKFILENAME ADDBACKFILE
#else
#define ADDBACKFILE ""
#endif

    TString schainFileName;
    for (int run = irunBegin; run <= irunEnd; run++)
    {
        schainFileName = TString::Format("%s%s_W%d_%d%s%s.root", RAWFILEPATH, RAWFILENAME, EVENTTIMEWINDOWSWIDTH, run, ADDBACKFILENAME, DOPPLERFILENAME);
        if (!FileExist(schainFileName))
            continue;

        fchain->Add(schainFileName);
        cout << "Read file: " << schainFileName << endl;
    }
#undef ADDBACKFILENAME
#undef DOPPLERFILENAME
}

/**
 * @brief create output file and directory
 *
 */
void YMatrix::OutputFile()
{
#ifdef GEGE
#define GEGEFILE "_2PAM"
#ifdef GEGEAM
#define GEGEAMFILE "_2PM"
#else
#define GEGEAMFILE ""
#endif
#else
#define GEGEFILE ""
#define GEGEAMFILE ""
#endif

#ifdef CUBEMATRIX
#define CUBEMATRIXFILE "_3PAM"
#ifdef CUBEMATRIXAM
#define CUBEMATRIXAMFILE "_3PM"
#else
#define CUBEMATRIXAMFILE ""
#endif
#else
#define CUBEMATRIXFILE ""
#define CUBEMATRIXAMFILE ""
#endif

#if defined(GEGEAM) || defined(CUBEMATRIXAM)
    fopf = new TFile(TString::Format("%s%s%s%s%s%s_%d_%d_CSIW%d_GECSIW%s%d_%d_GEW%d_AMW%s%d_%s%d_%d_%d.root", ROOTFILEPATH, ROOTFILENAME, GEGEFILE, GEGEAMFILE, CUBEMATRIXFILE, CUBEMATRIXAMFILE, irunBegin, irunEnd, CSICSICOINWINDOWS, (GECSICOINWINDOWSL < 0 ? "minus" : ""), TMath::Abs(GECSICOINWINDOWSL), GECSICOINWINDOWSR, GEGECOINWINDOWS, (GEGEAMWINDOWL1 < 0 ? "minus" : ""), TMath::Abs(GEGEAMWINDOWL1), (GEGEAMWINDOWR1 < 0 ? "minus" : ""), TMath::Abs(GEGEAMWINDOWR1), GEGEAMWINDOWL2, GEGEAMWINDOWR2).Data(), "RECREATE");
#else
    fopf = new TFile(TString::Format("%s%s%s%s%s%s_%d_%d_CSIW%d_GECSIW%s%d_%d_GEW%d.root", ROOTFILEPATH, ROOTFILENAME, GEGEFILE, GEGEAMFILE, CUBEMATRIXFILE, CUBEMATRIXAMFILE, irunBegin, irunEnd, CSICSICOINWINDOWS, (GECSICOINWINDOWSL < 0 ? "minus" : ""), TMath::Abs(GECSICOINWINDOWSL), GECSICOINWINDOWSR, GEGECOINWINDOWS).Data(), "RECREATE");
#endif

#undef GEGEFILE
#undef GEGEAMFILE
#undef CUBEMATRIXFILE
#undef CUBEMATRIXAMFILE
    if (fopf->IsOpen())
    {
        cout << "Output file: " << fopf->GetPath() << endl;
    }
    else
    {
        cerr << "Can't create output file: " << TString::Format("%s%s_%d_%d.root", ROOTFILEPATH, ROOTFILENAME, irunBegin, irunEnd) << endl;
        exit(-1);
    }

    fopf->mkdir("PAM/TH1/AlphaProtonTotal");
    fopf->mkdir("PAM/TH1/AlphaTotal");
    fopf->mkdir("PAM/TH1/ProtonTotal");
    fopf->mkdir("PAM/TH1/AlphaProton");

    fopf->mkdir("AM/TH1/AlphaProtonTotal");
    fopf->mkdir("AM/TH1/AlphaTotal");
    fopf->mkdir("AM/TH1/ProtonTotal");
    fopf->mkdir("AM/TH1/AlphaProton");

    fopf->mkdir("PM/TH1/AlphaTotal");
    fopf->mkdir("PM/TH1/AlphaProtonTotal");
    fopf->mkdir("PM/TH1/ProtonTotal");
    fopf->mkdir("PM/TH1/AlphaProton");

#ifdef GEGE
    fopf->mkdir("PAM/TH2/AlphaProtonTotal");
    fopf->mkdir("PAM/TH2/AlphaTotal");
    fopf->mkdir("PAM/TH2/ProtonTotal");
    fopf->mkdir("PAM/TH2/AlphaProton");
#ifdef GEGEAM
    fopf->mkdir("AM/TH2/AlphaProtonTotal");
    fopf->mkdir("AM/TH2/AlphaTotal");
    fopf->mkdir("AM/TH2/ProtonTotal");
    fopf->mkdir("AM/TH2/AlphaProton");

    fopf->mkdir("PM/TH2/AlphaProtonTotal");
    fopf->mkdir("PM/TH2/AlphaTotal");
    fopf->mkdir("PM/TH2/ProtonTotal");
    fopf->mkdir("PM/TH2/AlphaProton");
#endif
#endif

#ifdef CUBEMATRIX
    fopf->mkdir("PAM/TH3/AlphaProtonTotal");
    fopf->mkdir("PAM/TH3/AlphaTotal");
    fopf->mkdir("PAM/TH3/ProtonTotal");
    fopf->mkdir("PAM/TH3/AlphaProton");
#ifdef CUBEMATRIXAM
    fopf->mkdir("AM/TH3/AlphaProtonTotal");
    fopf->mkdir("AM/TH3/AlphaTotal");
    fopf->mkdir("AM/TH3/ProtonTotal");
    fopf->mkdir("AM/TH3/AlphaProton");

    fopf->mkdir("PM/TH3/AlphaProtonTotal");
    fopf->mkdir("PM/TH3/AlphaTotal");
    fopf->mkdir("PM/TH3/ProtonTotal");
    fopf->mkdir("PM/TH3/AlphaProton");
#endif
#endif
}

void YMatrix::InitHistogram()
{
    for (short i = 0; i < (ALPHAPTCMAX - ALPHAPTCMIN + 1); i++)
        for (short j = 0; j < (PROTONPTCMAX - PROTONPTCMIN + 1); j++)
            for (short k = 0; k < (TOTALPTCMAX - TOTALPTCMIN + 1); k++)
            {
                h1ggaptPAM[i][j][k] = nullptr;
                h1ggatPAM[i][k] = nullptr;
                h1ggptPAM[j][k] = nullptr;
                h1ggapPAM[i][j] = nullptr;

                h1ggaptAM[i][j][k] = nullptr;
                h1ggatAM[i][k] = nullptr;
                h1ggptAM[j][k] = nullptr;
                h1ggapAM[i][j] = nullptr;

                h1ggaptPM[i][j][k] = nullptr;
                h1ggatPM[i][k] = nullptr;
                h1ggptPM[j][k] = nullptr;
                h1ggapPM[i][j] = nullptr;
#ifdef GEGE
                h2ggaptPAM[i][j][k] = nullptr;
                h2ggatPAM[i][k] = nullptr;
                h2ggptPAM[j][k] = nullptr;
                h2ggapPAM[i][j] = nullptr;
#ifdef GEGEAM
                h2ggaptAM[i][j][k] = nullptr;
                h2ggatAM[i][k] = nullptr;
                h2ggptAM[j][k] = nullptr;
                h2ggapAM[i][j] = nullptr;

                h2ggaptPM[i][j][k] = nullptr;
                h2ggatPM[i][k] = nullptr;
                h2ggptPM[j][k] = nullptr;
                h2ggapPM[i][j] = nullptr;
#endif
#endif
#ifdef CUBEMATRIX
                h3ggaptPAM[i][j][k] = nullptr;
                h3ggatPAM[i][k] = nullptr;
                h3ggptPAM[j][k] = nullptr;
                h3ggapPAM[i][j] = nullptr;
#ifdef CUBEMATRIXAM
                h3ggaptAM[i][j][k] = nullptr;
                h3ggatAM[i][k] = nullptr;
                h3ggptAM[j][k] = nullptr;
                h3ggapAM[i][j] = nullptr;

                h3ggaptPM[i][j][k] = nullptr;
                h3ggatPM[i][k] = nullptr;
                h3ggptPM[j][k] = nullptr;
                h3ggapPM[i][j] = nullptr;
#endif
#endif
            }
}

/**
 * @brief This function is aimed to create a series of histogram
 *
 */
void YMatrix::CreateHistogram()
{
    InitHistogram();

    TString shggaptname;
    TString shggatname;
    TString shggptname;
    TString shggapname;
    for (short i = 0; i < (ALPHAPTCMAX - ALPHAPTCMIN + 1); i++)
    {
        for (short j = 0; j < (PROTONPTCMAX - PROTONPTCMIN + 1); j++)
        {
            for (short k = 0; k < (TOTALPTCMAX - TOTALPTCMIN + 1); k++)
            {
                if ((i + j) > k)
                    continue;

                shggaptname = TString::Format("alpha%d_proton%d_total%d", (i + ALPHAPTCMIN), (j + PROTONPTCMIN), (k + TOTALPTCMIN));
                shggatname = TString::Format("alpha%d_total%d", (i + ALPHAPTCMIN), (k + TOTALPTCMIN));
                shggptname = TString::Format("proton%d_total%d", (j + PROTONPTCMIN), (k + TOTALPTCMIN));
                shggapname = TString::Format("alpha%d_proton%d", (i + ALPHAPTCMIN), (j + PROTONPTCMIN));
                if (h1ggaptPAM[i][j][k] == nullptr)
                {
                    CreateHistogram(h1ggaptPAM[i][j][k], shggaptname + "PAM");
                    CreateHistogram(h1ggaptAM[i][j][k], shggaptname + "AM");
                    CreateHistogram(h1ggaptPM[i][j][k], shggaptname + "PM");
                    h1ggaptPM[i][j][k]->Sumw2(1);
#ifdef GEGE
                    CreateHistogram(h2ggaptPAM[i][j][k], shggaptname + "PAM");
#ifdef GEGEAM
                    CreateHistogram(h2ggaptAM[i][j][k], shggaptname + "AM");
                    CreateHistogram(h2ggaptPM[i][j][k], shggaptname + "PM");
                    h2ggaptPM[i][j][k]->Sumw2(1);
#endif
#endif
#ifdef CUBEMATRIX
                    CreateHistogram(h3ggaptPAM[i][j][k], shggaptname + "PAM");
#ifdef CUBEMATRIXAM
                    CreateHistogram(h3ggaptAM[i][j][k], shggaptname + "AM");
                    CreateHistogram(h3ggaptPM[i][j][k], shggaptname + "PM");
                    h3ggaptPM[i][j][k]->Sumw2(1);
#endif
#endif
                }
                if (h1ggatPAM[i][k] == nullptr)
                {
                    CreateHistogram(h1ggatPAM[i][k], shggatname + "PAM");
                    CreateHistogram(h1ggatAM[i][k], shggatname + "AM");
                    CreateHistogram(h1ggatPM[i][k], shggatname + "PM");
                    h1ggatPM[i][k]->Sumw2(1);
#ifdef GEGE
                    CreateHistogram(h2ggatPAM[i][k], shggatname + "PAM");
#ifdef GEGEAM
                    CreateHistogram(h2ggatAM[i][k], shggatname + "AM");
                    CreateHistogram(h2ggatPM[i][k], shggatname + "PM");
                    h2ggatPM[i][k]->Sumw2(1);
#endif
#endif
#ifdef CUBEMATRIX
                    CreateHistogram(h3ggatPAM[i][k], shggatname + "PAM");
#ifdef CUBEMATRIXAM
                    CreateHistogram(h3ggatAM[i][k], shggatname + "AM");
                    CreateHistogram(h3ggatPM[i][k], shggatname + "PM");
                    h3ggatPM[i][k]->Sumw2(1);
#endif
#endif
                }
                if (h1ggptPAM[j][k] == nullptr)
                {
                    CreateHistogram(h1ggptPAM[j][k], shggptname + "PAM");
                    CreateHistogram(h1ggptAM[j][k], shggptname + "AM");
                    CreateHistogram(h1ggptPM[j][k], shggptname + "PM");
                    h1ggptPM[j][k]->Sumw2(1);
#ifdef GEGE
                    CreateHistogram(h2ggptPAM[j][k], shggptname + "PAM");
#ifdef GEGEAM
                    CreateHistogram(h2ggptAM[j][k], shggptname + "AM");
                    CreateHistogram(h2ggptPM[j][k], shggptname + "PM");
                    h2ggptPM[j][k]->Sumw2(1);
#endif
#endif
#ifdef CUBEMATRIX
                    CreateHistogram(h3ggptPAM[j][k], shggptname + "PAM");
#ifdef CUBEMATRIXAM
                    CreateHistogram(h3ggptAM[j][k], shggptname + "AM");
                    CreateHistogram(h3ggptPM[j][k], shggptname + "PM");
                    h3ggptPM[j][k]->Sumw2(1);
#endif
#endif
                }
                if (h1ggapPAM[i][j] == nullptr)
                {
                    CreateHistogram(h1ggapPAM[i][j], shggapname + "PAM");
                    CreateHistogram(h1ggapAM[i][j], shggapname + "AM");
                    CreateHistogram(h1ggapPM[i][j], shggapname + "PM");
                    h1ggapPM[i][j]->Sumw2(1);
#ifdef GEGE
                    CreateHistogram(h2ggapPAM[i][j], shggapname + "PAM");
#ifdef GEGEAM
                    CreateHistogram(h2ggapAM[i][j], shggapname + "AM");
                    CreateHistogram(h2ggapPM[i][j], shggapname + "PM");
                    h2ggapPM[i][j]->Sumw2(1);
#endif
#endif
#ifdef CUBEMATRIX
                    CreateHistogram(h3ggapPAM[i][j], shggapname + "PAM");
#ifdef CUBEMATRIXAM
                    CreateHistogram(h3ggapAM[i][j], shggapname + "AM");
                    CreateHistogram(h3ggapPM[i][j], shggapname + "PM");
                    h3ggapPM[i][j]->Sumw2(1);
#endif
#endif
                }
            }
        }
    }
}

/**
 * @brief - overloaded funtion YTH1, YTH2, or YTH3
 * @brief - This function is aimed to create one histogram
 * @brief -  * -=> using to have address varibale, don't change the address
 * @brief - *& -=> using to nullptr, change the pointor address
 * @param ph1 the pointer of histogram
 * @param pshname  histogram named h1 + hname or h2 + hname or h3 + hname (There is no "+" in the histogram name)
 * @return - true crate
 * @return - false no crate
 */
bool YMatrix::CreateHistogram(YTH1 *&phistogram, const TString &pshname)
{
    phistogram = new YTH1(("h1" + pshname).Data(), (pshname + "_projection").Data(), EBINNUMBER, EMIN, EMAX);
    if (phistogram != nullptr)
        return true;
    else
    {
        cerr << "Can't create " << phistogram->Class_Name() << "named " << pshname << " histogram!" << endl;
        return false;
    }
}
bool YMatrix::CreateHistogram(YTH2 *&phistogram, const TString &pshname)
{
    phistogram = new YTH2(("h2" + pshname).Data(), (pshname + "_2matrix").Data(), EBINNUMBER, EMIN, EMAX, EBINNUMBER, EMIN, EMAX);
    if (phistogram != nullptr)
        return true;
    else
    {
        cerr << "Can't create " << phistogram->Class_Name() << "named " << pshname << " histogram!" << endl;
        return false;
    }
}
bool YMatrix::CreateHistogram(YTH3 *&phistogram, const TString &pshname)
{
    phistogram = new YTH3(("h3" + pshname).Data(), (pshname + "_3matrix").Data(), EBINNUMBER, EMIN, EMAX, EBINNUMBER, EMIN, EMAX, EBINNUMBER, EMIN, EMAX);
    if (phistogram != nullptr)
        return true;
    else
    {
        cerr << "Can't create " << phistogram->Class_Name() << "named " << pshname << " histogram!" << endl;
        return false;
    }
}

/**
 * @brief Fill histogram[alpha][proton][total], histogram[alpha][total], histogram[proton][total], or histogram[alpha][proton]
 * @brief A_{n}^{2} = C_{n}^{2} *2; A_{n}^{3} = C_{n}^{3} * 6;
 * @param pde1
 * @param pde2
 * @param pa hit alpha number
 * @param pp hit proton number
 * @param pt hit total number
 * @param poption class: TString. Select which histogram to fill, EXPLAINED BELOW
 * @param pde3 default value parameter, ONLY USED BY CUBE MATRIX
 * @param - The option is intended to selete which histogram to fill. FOUR digits: thousands, hundreds, tens, ones
 * @param - 0 no fill
 * @param - 1 fill prompt accidental matrix(PAM)
 * @param - 2 fill accidental matrix(AM)
 * @param - "1000" -=> apt  PAM[alpha][proton][total]; "2000" -=> AM[alpha][proton][total]
 * @param - "100" -=> at PAM[alpha][total]; "200" -=> AM[alpha][total]
 * @param - "10" -=> pt PAM[proton][total]; "20" -=> AM[proton][total]
 * @param - "1" -=> ap PAM[alpha][proton]; "2" -=> AM[alpha][proton]
 * @param - For example:
 * @param - "1111" does "1000", "100", "10", and "1"; "0101" does "100" and "1"; "0001"  only does "1"; "0000" does nothing
 * @param - "2222" does "2000", "200", "20", and "2"; "2101" does "2000", "100",  and "1"; "0002"  only does "2"; "0000" does nothing
 * @return - true sucessfully fill
 * @return - false failed fill
 */
bool YMatrix::FillHistogram(const bool &pbsym, const double &pde1, const double &pde2, const short &pa, const short &pp, const short &pt, const TString &psoption, const double &pde3)
{
    int ioption = psoption.Atoi();
    int ifapt1 = 0, ifat1 = 0, ifpt1 = 0, ifap1 = 0;
    int ifapt2 = 0, ifat2 = 0, ifpt2 = 0, ifap2 = 0;
    short alphaCode = pa - ALPHAPTCMIN;
    short protonCode = pp - PROTONPTCMIN;
    short totalCode = pt - TOTALPTCMIN;

    /*------------- PAM ------------*/

    // do apt
    if (((ioption / 1000) % 10) == 1)
    {

        if (h1ggaptPAM[alphaCode][protonCode][totalCode] != nullptr)
        {
            ifapt1 = 1;
            h1ggaptPAM[alphaCode][protonCode][totalCode]->Fill(pde1);
            if (pbsym)
                h1ggaptPAM[alphaCode][protonCode][totalCode]->Fill(pde2);
#ifdef GEGE
            h2ggaptPAM[alphaCode][protonCode][totalCode]->Fill(pde1, pde2);
            if (pbsym)
                h2ggaptPAM[alphaCode][protonCode][totalCode]->Fill(pde2, pde1);
#endif
#ifdef CUBEMATRIX
            h3ggaptPAM[alphaCode][protonCode][totalCode]->Fill(pde1, pde2, pde3);
            if (pbsym)
            {
                h3ggaptPAM[alphaCode][protonCode][totalCode]->Fill(pde1, pde3, pde2);
                h3ggaptPAM[alphaCode][protonCode][totalCode]->Fill(pde2, pde1, pde3);
                h3ggaptPAM[alphaCode][protonCode][totalCode]->Fill(pde2, pde3, pde1);
                h3ggaptPAM[alphaCode][protonCode][totalCode]->Fill(pde3, pde1, pde2);
                h3ggaptPAM[alphaCode][protonCode][totalCode]->Fill(pde3, pde2, pde1);
            }
#endif
        }
        else
            ifapt1 = 0;
    }

    // do at
    if (((ioption / 100) % 10) == 1)
    {
        if (h1ggatPAM[alphaCode][totalCode] != nullptr)
        {
            ifat1 = 1;
            h1ggatPAM[alphaCode][totalCode]->Fill(pde1);
            if (pbsym)
                h1ggatPAM[alphaCode][totalCode]->Fill(pde2);
#ifdef GEGE
            h2ggatPAM[alphaCode][totalCode]->Fill(pde1, pde2);
            if (pbsym)
                h2ggatPAM[alphaCode][totalCode]->Fill(pde2, pde1);
#endif

#ifdef CUBEMATRIX
            h3ggatPAM[alphaCode][totalCode]->Fill(pde1, pde2, pde3);
            if (pbsym)
            {
                h3ggatPAM[alphaCode][totalCode]->Fill(pde1, pde3, pde2);
                h3ggatPAM[alphaCode][totalCode]->Fill(pde2, pde1, pde3);
                h3ggatPAM[alphaCode][totalCode]->Fill(pde2, pde3, pde1);
                h3ggatPAM[alphaCode][totalCode]->Fill(pde3, pde1, pde2);
                h3ggatPAM[alphaCode][totalCode]->Fill(pde3, pde2, pde1);
            }
#endif
        }
        else
            ifat1 = 0;
    }

    // do pt
    if (((ioption / 10) % 10) == 1)
    {
        if (h1ggptPAM[protonCode][totalCode] != nullptr)
        {
            ifpt1 = 1;
            h1ggptPAM[protonCode][totalCode]->Fill(pde1);
            if (pbsym)
                h1ggptPAM[protonCode][totalCode]->Fill(pde2);
#ifdef GEGE
            h2ggptPAM[protonCode][totalCode]->Fill(pde1, pde2);
            if (pbsym)
                h2ggptPAM[protonCode][totalCode]->Fill(pde2, pde1);
#endif
#ifdef CUBEMATRIX
            h3ggptPAM[protonCode][totalCode]->Fill(pde1, pde2, pde3);
            if (pbsym)
            {
                h3ggptPAM[protonCode][totalCode]->Fill(pde1, pde3, pde2);
                h3ggptPAM[protonCode][totalCode]->Fill(pde2, pde1, pde3);
                h3ggptPAM[protonCode][totalCode]->Fill(pde2, pde3, pde1);
                h3ggptPAM[protonCode][totalCode]->Fill(pde3, pde1, pde2);
                h3ggptPAM[protonCode][totalCode]->Fill(pde3, pde2, pde1);
            }
#endif
        }
        else
            ifpt1 = 0;
    }

    // do ap
    if (((ioption / 1) % 10) == 1)
    {
        if (h1ggapPAM[alphaCode][protonCode] != nullptr)
        {
            ifap1 = 1;
            h1ggapPAM[alphaCode][protonCode]->Fill(pde1);
            if (pbsym)
                h1ggapPAM[alphaCode][protonCode]->Fill(pde2);
#ifdef GEGE
            h2ggapPAM[alphaCode][protonCode]->Fill(pde1, pde2);
            if (pbsym)
                h2ggapPAM[alphaCode][protonCode]->Fill(pde2, pde1);
#endif
#ifdef CUBEMATRIX
            h3ggapPAM[alphaCode][protonCode]->Fill(pde1, pde2, pde3);
            if (pbsym)
            {
                h3ggapPAM[alphaCode][protonCode]->Fill(pde1, pde3, pde2);
                h3ggapPAM[alphaCode][protonCode]->Fill(pde2, pde1, pde3);
                h3ggapPAM[alphaCode][protonCode]->Fill(pde2, pde3, pde1);
                h3ggapPAM[alphaCode][protonCode]->Fill(pde3, pde1, pde2);
                h3ggapPAM[alphaCode][protonCode]->Fill(pde3, pde2, pde1);
            }
#endif
        }
        else
            ifap1 = 0;
    }

    /*------------- AM ------------*/

    // do apt
    if (((ioption / 1000) % 10) == 2)
    {

        if (h1ggaptAM[alphaCode][protonCode][totalCode] != nullptr)
        {
            ifapt2 = 2;
            h1ggaptAM[alphaCode][protonCode][totalCode]->Fill(pde1);
            if (pbsym)
                h1ggaptAM[alphaCode][protonCode][totalCode]->Fill(pde2);
#ifdef GEGE
#ifdef GEGEAM
            h2ggaptAM[alphaCode][protonCode][totalCode]->Fill(pde1, pde2);
            if (pbsym)
                h2ggaptAM[alphaCode][protonCode][totalCode]->Fill(pde2, pde1);
#endif
#endif
#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
            h3ggaptAM[alphaCode][protonCode][totalCode]->Fill(pde1, pde2, pde3);
            if (pbsym)
            {
                h3ggaptAM[alphaCode][protonCode][totalCode]->Fill(pde1, pde3, pde2);
                h3ggaptAM[alphaCode][protonCode][totalCode]->Fill(pde2, pde1, pde3);
                h3ggaptAM[alphaCode][protonCode][totalCode]->Fill(pde2, pde3, pde1);
                h3ggaptAM[alphaCode][protonCode][totalCode]->Fill(pde3, pde1, pde2);
                h3ggaptAM[alphaCode][protonCode][totalCode]->Fill(pde3, pde2, pde1);
            }
#endif
#endif
        }
        else
            ifapt2 = 0;
    }

    // do at
    if (((ioption / 100) % 10) == 2)
    {

        if (h1ggatAM[alphaCode][totalCode] != nullptr)
        {
            ifat2 = 2;
            h1ggatAM[alphaCode][totalCode]->Fill(pde1);
            if (pbsym)
                h1ggatAM[alphaCode][totalCode]->Fill(pde2);
#ifdef GEGE
#ifdef GEGEAM
            h2ggatAM[alphaCode][totalCode]->Fill(pde1, pde2);
            if (pbsym)
                h2ggatAM[alphaCode][totalCode]->Fill(pde2, pde1);
#endif
#endif
#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
            h3ggatAM[alphaCode][totalCode]->Fill(pde1, pde2, pde3);
            if (pbsym)
            {
                h3ggatAM[alphaCode][totalCode]->Fill(pde1, pde3, pde2);
                h3ggatAM[alphaCode][totalCode]->Fill(pde2, pde1, pde3);
                h3ggatAM[alphaCode][totalCode]->Fill(pde2, pde3, pde1);
                h3ggatAM[alphaCode][totalCode]->Fill(pde3, pde1, pde2);
                h3ggatAM[alphaCode][totalCode]->Fill(pde3, pde2, pde1);
            }
#endif
#endif
        }
        else
            ifat2 = 0;
    }

    // do pt
    if (((ioption / 10) % 10) == 2)
    {

        if (h1ggptAM[protonCode][totalCode] != nullptr)
        {
            ifpt2 = 2;
            h1ggptAM[protonCode][totalCode]->Fill(pde1);
            if (pbsym)
                h1ggptAM[protonCode][totalCode]->Fill(pde2);
#ifdef GEGE
#ifdef GEGEAM
            h2ggptAM[protonCode][totalCode]->Fill(pde1, pde2);
            if (pbsym)
                h2ggptAM[protonCode][totalCode]->Fill(pde2, pde1);
#endif
#endif
#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
            h3ggptAM[protonCode][totalCode]->Fill(pde1, pde2, pde3);
            if (pbsym)
            {
                h3ggptAM[protonCode][totalCode]->Fill(pde1, pde3, pde2);
                h3ggptAM[protonCode][totalCode]->Fill(pde2, pde1, pde3);
                h3ggptAM[protonCode][totalCode]->Fill(pde2, pde3, pde1);
                h3ggptAM[protonCode][totalCode]->Fill(pde3, pde1, pde2);
                h3ggptAM[protonCode][totalCode]->Fill(pde3, pde2, pde1);
            }
#endif
#endif
        }
        else
            ifpt2 = 0;
    }

    // do ap
    if (((ioption / 1) % 10) == 2)
    {

        if (h1ggapAM[alphaCode][protonCode] != nullptr)
        {
            ifap2 = 2;
            h1ggapAM[alphaCode][protonCode]->Fill(pde1);
            if (pbsym)
                h1ggapAM[alphaCode][protonCode]->Fill(pde2);
#ifdef GEGE
#ifdef GEGEAM
            h2ggapAM[alphaCode][protonCode]->Fill(pde1, pde2);
            if (pbsym)
                h2ggapAM[alphaCode][protonCode]->Fill(pde2, pde1);
#endif
#endif
#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
            h3ggapAM[alphaCode][protonCode]->Fill(pde1, pde2, pde3);
            if (pbsym)
            {
                h3ggapAM[alphaCode][protonCode]->Fill(pde1, pde3, pde2);
                h3ggapAM[alphaCode][protonCode]->Fill(pde2, pde1, pde3);
                h3ggapAM[alphaCode][protonCode]->Fill(pde2, pde3, pde1);
                h3ggapAM[alphaCode][protonCode]->Fill(pde3, pde1, pde2);
                h3ggapAM[alphaCode][protonCode]->Fill(pde3, pde2, pde1);
            }
#endif
#endif
        }
        else
            ifap2 = 0;
    }

    return (ioption == (((ifapt1 + ifapt2) * 1000) + ((ifat1 + ifat2) * 100) + ((ifpt1 + ifpt2) * 10) + ((ifap1 + ifap2) * 1)));
}

/**
 * @brief - Take advantage
 * @brief 1. sorting time stamp from smallest to largest
 * @brief 2. The gamma gamma coincident time window is symmetrical
 * @brief -  one can see it in loop
 *
 */
void YMatrix::Process()
{
    Init();
    bool WD1 = (TMath::Abs(GEGEAMWINDOWL1) == TMath::Abs(GEGEAMWINDOWR2));
    bool WD2 = (TMath::Abs(GEGEAMWINDOWR1) == TMath::Abs(GEGEAMWINDOWL2));
    bool SYMMETRICAL = WD1 && WD2;

    if (SYMMETRICAL)
        cout << "The gamma gamma coincident time window is symmetrical" << endl;
    else
        cout << "The gamma gamma coincident time window is not symmetrical" << endl;

    short gsecondlp_begining;
    short gthirdlp_begining;
    for (Long64_t jentry = 0; jentry < ltotalEntry; ++jentry)
    {
        fchain->GetEntry(jentry);

        if (jentry % 10000 == 0)
        {
            cout << TString::Format("Processing: %lld | % lld | %.2f%%", jentry, ltotalEntry, (double)jentry / ltotalEntry * 100) << endl;
            cout << "\033[1A\033[K";
        }
        bfirstLoopHit = true;
        for (short cfirstlp = 0; cfirstlp < hit; cfirstlp++)
        {
            if (WhatDetector(id[cfirstlp]) != kschargedDetectorFlag)
                continue;
            // get the begining of charged particle chain
            ltscFirst = GetTs(ts[cfirstlp], sr[cfirstlp]);
            if (bfirstLoopHit)
            {
                ClearHitVar();
                PtcAdd(alpha[cfirstlp], proton[cfirstlp], total[cfirstlp]);
                ltsmin = ltscFirst;
                ltsmax = ltscFirst;
                bfirstLoopHit = false;
            }
            else
            {
                if (ltscFirst > ltsmax) // the begining of next chain charged particle
                {
                    ClearHitVar();
                    PtcAdd(alpha[cfirstlp], proton[cfirstlp], total[cfirstlp]);
                    ltsmin = ltscFirst;
                    ltsmax = ltscFirst;
                }
                else
                {
                    continue;
                }
            }
            // Finding charged partical use candied haws on a stick
            for (short ccecondlp = (cfirstlp + 1); ccecondlp < hit; ccecondlp++)
            {
                if (WhatDetector(id[ccecondlp]) != kschargedDetectorFlag)
                    continue;
                ltscSecond = GetTs(ts[ccecondlp], sr[ccecondlp]);
                if (!TcoinLR(ltscSecond, ltsmax, CSICSICOINWINDOWS))
                    continue;
                ltsmin = Min(Min(ltscFirst, ltscSecond), ltsmin);
                ltsmax = Max(Max(ltscFirst, ltscSecond), ltsmax);
                PtcAdd(alpha[ccecondlp], proton[ccecondlp], total[ccecondlp]);

                cfirstlp = ccecondlp; // TIME STAMP ADVANTAGE
            }

            /* fill gamma to histogram*/
            if ((salphaHit < ALPHAPTCMIN) || (ALPHAPTCMAX < salphaHit))
                continue;
            else if ((sprotonHit < PROTONPTCMIN) || (PROTONPTCMAX < sprotonHit))
                continue;
            else if ((stotalHit < TOTALPTCMIN) || (TOTALPTCMAX < stotalHit))
                continue;

            for (short gfristlp = 0; gfristlp < hit; gfristlp++)
            {
                if (WhatDetector(id[gfristlp]) != ksGeFlag)
                    continue;

                ltsgFirst = GetTs(ts[gfristlp], sr[gfristlp]);
                if (!TcoinL(ltsgFirst, ltsmin, GECSICOINWINDOWSL))
                    continue;
                if (!TcoinR(ltsgFirst, ltsmax, GECSICOINWINDOWSR))
                    continue;

                if (SYMMETRICAL)
                    gsecondlp_begining = gfristlp + 1;
                else
                    gsecondlp_begining = 0;
                for (short gsecondlp = gsecondlp_begining; gsecondlp < hit; gsecondlp++) // TIME WINDOW SYMMETRICAL
                {
                    if (gfristlp == gsecondlp)
                        continue;

                    if (WhatDetector(id[gsecondlp]) != ksGeFlag)
                        continue;

                    ltsgSecond = GetTs(ts[gsecondlp], sr[gsecondlp]);
                    if (!TcoinL(ltsgSecond, ltsmin, GECSICOINWINDOWSL))
                        continue;

                    if (!TcoinR(ltsgSecond, ltsmax, GECSICOINWINDOWSR))
                        continue;

                    if (TcoinLR(ltsgFirst, ltsgSecond, GEGECOINWINDOWS)) // prompt accidental  coincidence
                        bpamgFirstgSecond = true;

                    if (TcoinLR(ltsgFirst, ltsgSecond, GEGEAMWINDOWL1, GEGEAMWINDOWR1)) // accidental  coincidence
                        bamgFirstgSecond = true;
                    else if (TcoinLR(ltsgFirst, ltsgSecond, GEGEAMWINDOWL2, GEGEAMWINDOWR2))
                        bamgFirstgSecond = true;
#ifndef CUBEMATRIX
                    if (bpamgFirstgSecond)
                    {
                        if (!FillHistogram(SYMMETRICAL, dpl[gfristlp], dpl[gsecondlp], salphaHit, sprotonHit, stotalHit, "1111"))
                            cerr << "can't fill histogram PAM, jentry = " << jentry << endl;

                        ClearHitgpam();
                    }

#endif

#ifndef CUBEMATRIXAM
                    if (bamgFirstgSecond)
                    {
                        if (!FillHistogram(SYMMETRICAL, dpl[gfristlp], dpl[gsecondlp], salphaHit, sprotonHit, stotalHit, "2222"))
                            cerr << "can't fill histogram AM, jentry = " << jentry << endl;

                        ClearHitgam();
                    }

#endif

#ifdef CUBEMATRIX
                    if (SYMMETRICAL)
                        gthirdlp_begining = gsecondlp + 1;
                    else
                        gthirdlp_begining = 0;

                    for (short gthirdlp = gthirdlp_begining; gthirdlp < hit; gthirdlp++)
                    {
                        if (gsecondlp == gthirdlp || gfristlp == gthirdlp) // had judge gfristlp == gsecondlp in second loop of gamma
                            continue;

                        if (WhatDetector(id[gthirdlp]) != ksGeFlag)
                            continue;
                        ltsgThird = GetTs(ts[gthirdlp], sr[gthirdlp]);
                        if (!TcoinL(ltsgThird, ltsmin, GECSICOINWINDOWSL))
                            continue;
                        if (!TcoinR(ltsgThird, ltsmax, GECSICOINWINDOWSR))
                            continue;

                        if (TcoinLR(ltsgFirst, ltsgThird, GEGECOINWINDOWS))
                            bpamgFirstgThird = true;
                        if (TcoinLR(ltsgSecond, ltsgThird, GEGECOINWINDOWS))
                            bpamgSecondgThird = true;
                        if (bpamgFirstgSecond && bpamgFirstgThird && bpamgSecondgThird) // had judge bpamgFirstgSecond in second loop of gamma
                        {
                            if (!FillHistogram(SYMMETRICAL, dpl[gfristlp], dpl[gsecondlp], salphaHit, sprotonHit, stotalHit, "1111", dpl[gthirdlp]))
                                cerr << "can't fill histogram PAM, jentry = " << jentry << endl;

                            ClearHitgpam();
                        }

#ifdef CUBEMATRIXAM
                        if (TcoinLR(ltsgFirst, ltsgThird, GEGEAMWINDOWL1, GEGEAMWINDOWR1))
                            bamgFirstgThird = true;
                        else if (TcoinLR(ltsgFirst, ltsgThird, GEGEAMWINDOWL2, GEGEAMWINDOWR2))
                            bamgFirstgThird = true;

                        if (TcoinLR(ltsgSecond, ltsgThird, GEGEAMWINDOWL1, GEGEAMWINDOWR1))
                            bamgSecondgThird = true;
                        else if (TcoinLR(ltsgSecond, ltsgThird, GEGEAMWINDOWL2, GEGEAMWINDOWR2))
                            bamgSecondgThird = true;

                        if (bamgFirstgSecond && bamgFirstgThird && bamgSecondgThird)
                        {
                            if (!FillHistogramSYM(SYMMETRICAL, dpl[gfristlp], dpl[gsecondlp], salphaHit, sprotonHit, stotalHit, "2222", dpl[gthirdlp]))
                                cerr << "can't fill histogram PAM, jentry = " << jentry << endl;

                            ClearHitgam();
                        }

#endif
                    }
#endif
                }
            }
        }
    }

    cout << endl;
    cout << "GET PM!" << endl;
    if (!FillPM())
        cerr << "can't add matrix to PM" << endl;
    if (StoreFile())
        cout << "Finish!" << endl;
    else
        cout << "Fail!" << endl;
    if (fopf != nullptr)
        fopf->Close();
}

/**
 * @brief PM has transferred the error, using the function TH1::Sumw2(1). There are two ways to transferred error, namely using the function for AM or PAM, but considering the error is sqrt(n), n is the bincontent, so I choose to use the funtion for PM
 *
 * @return - true good fill
 * @return  - false bad fill
 */
bool YMatrix::FillPM()
{
    if (h1ggaptPM[0][0] == nullptr)
        return false;

    double dPAMT = GEGECOINWINDOWS * 2.;
    double dAMT = TMath::Abs(GEGEAMWINDOWR1 - GEGEAMWINDOWL1) + TMath::Abs(GEGEAMWINDOWR2 - GEGEAMWINDOWL2);
    double dweight = -dPAMT / dAMT;

    bool getaptPM[(ALPHAPTCMAX - ALPHAPTCMIN + 1)][(PROTONPTCMAX - PROTONPTCMIN + 1)][(TOTALPTCMAX - TOTALPTCMIN + 1)];
    bool getatPM[(ALPHAPTCMAX - ALPHAPTCMIN + 1)][(TOTALPTCMAX - TOTALPTCMIN + 1)];
    bool getptPM[(PROTONPTCMAX - PROTONPTCMIN + 1)][(TOTALPTCMAX - TOTALPTCMIN + 1)];
    bool getapPM[(ALPHAPTCMAX - ALPHAPTCMIN + 1)][(PROTONPTCMAX - PROTONPTCMIN + 1)];

    for (short i = 0; i < (ALPHAPTCMAX - ALPHAPTCMIN + 1); i++)
        for (short j = 0; j < (PROTONPTCMAX - PROTONPTCMIN + 1); j++)
            for (short k = 0; k < (TOTALPTCMAX - TOTALPTCMIN + 1); k++)
            {
                getaptPM[i][j][k] = true;
                getatPM[i][k] = true;
                getptPM[j][k] = true;
                getapPM[i][j] = true;
            }

    for (short i = 0; i < (ALPHAPTCMAX - ALPHAPTCMIN + 1); i++)
        for (short j = 0; j < (PROTONPTCMAX - PROTONPTCMIN + 1); j++)
            for (short k = 0; k < (TOTALPTCMAX - TOTALPTCMIN + 1); k++)
            {
                if ((i + j) > k)
                    continue;
                printf("\r GET PM process: alpha%hd_proton%hd_total%hd", (i + ALPHAPTCMIN), (j + PROTONPTCMIN), (k + TOTALPTCMIN));
                if (getaptPM[i][j][k] && (h1ggaptPAM[i][j][k] != nullptr))
                {
                    getaptPM[i][j][k] = false;
                    h1ggaptPM[i][j][k]->Add(h1ggaptPAM[i][j][k], h1ggaptAM[i][j][k], 1, dweight);

#ifdef GEGE
#ifdef GEGEAM
                    h2ggaptPM[i][j][k]->Add(h2ggaptPAM[i][j][k], h2ggaptAM[i][j][k], 1, dweight);
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
                    h3ggaptPM[i][j][k]->Add(h3ggaptPAM[i][j][k], h3ggaptAM[i][j][k], 1, dweight);
#endif
#endif
                }
                if (getatPM[i][k] && (h1ggatPAM[i][k] != nullptr))
                {
                    getatPM[i][k] = false;
                    h1ggatPM[i][k]->Add(h1ggatPAM[i][k], h1ggatAM[i][k], 1, dweight);

#ifdef GEGE
#ifdef GEGEAM
                    h2ggatPM[i][k]->Add(h2ggatPAM[i][k], h2ggatAM[i][k], 1, dweight);
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
                    h3ggatPM[i][k]->Add(h3ggatPAM[i][k], h3ggatAM[i][k], 1, dweight);
#endif
#endif
                }
                if (getptPM[j][k] && (h1ggptPAM[j][k] != nullptr))
                {
                    getptPM[j][k] = false;
                    h1ggptPM[j][k]->Add(h1ggptPAM[j][k], h1ggptAM[j][k], 1, dweight);

#ifdef GEGE
#ifdef GEGEAM
                    h2ggptPM[j][k]->Add(h2ggptPAM[j][k], h2ggptAM[j][k], 1, dweight);
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
                    h3ggptPM[j][k]->Add(h3ggptPAM[j][k], h3ggptAM[j][k], 1, dweight);
#endif
#endif
                }
                if (getapPM[i][j] && (h1ggapPAM[i][j] != nullptr))
                {
                    getapPM[i][j] = false;

                    h1ggapPM[i][j]->Add(h1ggapPAM[i][j], h1ggapAM[i][j], 1, dweight);

#ifdef GEGE
#ifdef GEGEAM
                    h2ggapPM[i][j]->Add(h2ggapPAM[i][j], h2ggapAM[i][j], 1, dweight);
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
                    h3ggptPM[j][k]->Add(h3ggptPAM[j][k], h3ggptAM[j][k], 1, dweight);
                    h3ggapPM[i][j]->Add(h3ggapPAM[i][j], h3ggapAM[i][j], 1, dweight);
#endif
#endif
                }
            }
    cout << endl;
    return true;
}

bool YMatrix::StoreFile()
{
    if (fopf == nullptr)
        return false;

    for (short i = 0; i < (ALPHAPTCMAX - ALPHAPTCMIN + 1); i++)
        for (short j = 0; j < (PROTONPTCMAX - PROTONPTCMIN + 1); j++)
            for (short k = 0; k < (TOTALPTCMAX - TOTALPTCMIN + 1); k++)
            {
                if ((i + j) > k)
                    continue;

                // This function will store and delete historgram
                // so one may not use bool variable to judge whether it has been stored
                StoreHistogram(h1ggaptPAM[i][j][k], "PAM/TH1/AlphaProtonTotal");
                StoreHistogram(h1ggatPAM[i][k], "PAM/TH1/AlphaTotal");
                StoreHistogram(h1ggptPAM[j][k], "PAM/TH1/ProtonTotal");
                StoreHistogram(h1ggapPAM[i][j], "PAM/TH1/AlphaProton");

                StoreHistogram(h1ggaptAM[i][j][k], "AM/TH1/AlphaProtonTotal");
                StoreHistogram(h1ggatAM[i][k], "AM/TH1/AlphaTotal");
                StoreHistogram(h1ggptAM[j][k], "AM/TH1/ProtonTotal");
                StoreHistogram(h1ggapAM[i][j], "AM/TH1/AlphaProton");

                StoreHistogram(h1ggaptPM[i][j][k], "PM/TH1/AlphaProtonTotal");
                StoreHistogram(h1ggatPM[i][k], "PM/TH1/AlphaTotal");
                StoreHistogram(h1ggptPM[j][k], "PM/TH1/ProtonTotal");
                StoreHistogram(h1ggapPM[i][j], "PM/TH1/AlphaProton");

#ifdef GEGE
                StoreHistogram(h2ggaptPAM[i][j][k], "PAM/TH2/AlphaProtonTotal");
                StoreHistogram(h2ggatPAM[i][k], "PAM/TH2/AlphaTotal");
                StoreHistogram(h2ggptPAM[j][k], "PAM/TH2/ProtonTotal");
                StoreHistogram(h2ggapPAM[i][j], "PAM/TH2/AlphaProton");
#ifdef GEGEAM
                StoreHistogram(h2ggaptAM[i][j][k], "AM/TH2/AlphaProtonTotal");
                StoreHistogram(h2ggatAM[i][k], "AM/TH2/AlphaTotal");
                StoreHistogram(h2ggptAM[j][k], "AM/TH2/ProtonTotal");
                StoreHistogram(h2ggapAM[i][j], "AM/TH2/AlphaProton");

                StoreHistogram(h2ggaptPM[i][j][k], "PM/TH2/AlphaProtonTotal");
                StoreHistogram(h2ggatPM[i][k], "PM/TH2/AlphaTotal");
                StoreHistogram(h2ggptPM[j][k], "PM/TH2/ProtonTotal");
                StoreHistogram(h2ggapPM[i][j], "PM/TH2/AlphaProton");
#endif
#endif

#ifdef CUBEMATRIX
                StoreHistogram(h3ggaptPAM[i][j][k], "PAM/TH3/AlphaProtonTotal");
                StoreHistogram(h3ggatPAM[i][k], "PAM/TH3/AlphaTotal");
                StoreHistogram(h3ggptPAM[j][k], "PAM/TH3/ProtonTotal");
                StoreHistogram(h3ggapPAM[i][j], "PAM/TH3/AlphaProton");
#ifdef CUBEMATRIXAM
                StoreHistogram(h3ggaptAM[i][j][k], "AM/TH3/AlphaProtonTotal");
                StoreHistogram(h3ggatAM[i][k], "AM/TH3/AlphaTotal");
                StoreHistogram(h3ggptAM[j][k], "AM/TH3/ProtonTotal");
                StoreHistogram(h3ggapAM[i][j], "AM/TH3/AlphaProton");

                StoreHistogram(h3ggaptPM[i][j][k], "PM/TH3/AlphaProtonTotal");
                StoreHistogram(h3ggatPM[i][k], "PM/TH3/AlphaTotal");
                StoreHistogram(h3ggptPM[j][k], "PM/TH3/ProtonTotal");
                StoreHistogram(h3ggapPM[i][j], "PM/TH3/AlphaProton");
#endif
#endif
            }
    return true;
}

/**
 * @brief Initialize some varibale to zero or false
 *
 */
inline void YMatrix::ClearHitVar()
{
    salphaHit = 0;
    sprotonHit = 0;
    stotalHit = 0;

    bpamgFirstgSecond = false;
    bpamgFirstgThird = false;
    bpamgSecondgThird = false;

    bamgFirstgSecond = false;
    bamgFirstgThird = false;
    bamgSecondgThird = false;
}

inline void YMatrix::ClearHitgpam()
{
    bpamgFirstgSecond = false;
    bpamgFirstgThird = false;
    bpamgSecondgThird = false;
}

inline void YMatrix::ClearHitgam()
{
    bamgFirstgSecond = false;
    bamgFirstgThird = false;
    bamgSecondgThird = false;
}

/**
 * @brief  record charged particle
 *
 * @param pa alpha number in one hit
 * @param pp proton number in one hit
 * @param pt total number in one hit
 *
 */
inline void YMatrix::PtcAdd(const short &pa, const short &pp, const short &pt)
{
    salphaHit += pa;
    sprotonHit += pp;
    stotalHit += pt;
}

/**
 * @brief - This function is intended to fill one gamma to TH1, TH2 or TH3
 * @brief - overloaded three YTH1, YTH2, and YTH3
 * @param ph - pointer variable, addree of histogram
 * @param psdir - Histogram is stored to this path
 */
inline void YMatrix::StoreHistogram(YTH1 *&ph, const TString &psdir)
{

    if (ph != nullptr)
    {
        if (ph->GetEntries() > LEASTENTRY)
        {
            fopf->cd(psdir.Data());
            ph->Write();
        }
        delete ph;
        ph = nullptr;
    }
}

inline void YMatrix::StoreHistogram(YTH2 *&ph, const TString &psdir)
{

    if (ph != nullptr)
    {
        if (ph->GetEntries() > LEASTENTRY)
        {
            fopf->cd(psdir.Data());
            ph->Write();
        }
        delete ph;
        ph = nullptr;
    }
}

inline void YMatrix::StoreHistogram(YTH3 *&ph, const TString &psdir)
{

    if (ph != nullptr)
    {
        if (ph->GetEntries() > LEASTENTRY)
        {
            fopf->cd(psdir.Data());
            ph->Write();
        }
        delete ph;
        ph = nullptr;
    }
}

/**
 * @brief - The units changes from 10ns or 8ns to 1ns
 * @brief - (pssr == 250 ? (plts * 8) : (plts * 10))
 * @param plts  original time stamp
 * @param pssr module frequency
 * @return - Long64_t: time stamp with 1ns units
 */
inline Long64_t YMatrix::GetTs(const Long64_t &plts, const short &pssr)
{
    return (pssr == 250 ? (plts * 8) : (plts * 10));
}

/**
 * @brief determine TMath::Abs(parameter0 - parameter1) <= parameter2
 *
 * @param pltsi units 1ns
 * @param pltsj units 1ns
 * @param pltwd pltsi and pltsj are considered coincidence in this time windows (pltwd)
 * @return - true YES
 * @return - false NO
 */
inline bool YMatrix::TcoinLR(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwd)
{
    return TMath::Abs(pltsi - pltsj) <= pltwd;
}

/**
 * @brief determine (parameter0 - parameter1) >= parameter2 && (parameter0 - parameter1) <= parameter3
 *
 * @param pltsi units 1ns
 * @param pltsj units 1ns
 * @param pltwdL left window edge
 * @param pltwdR right window edge
 * @return - true YES
 * @return - false NO
 */
inline bool YMatrix::TcoinLR(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwdL, const Long64_t &pltwdR)
{
    return (TcoinL(pltsi, pltsj, pltwdL) && TcoinR(pltsi, pltsj, pltwdR));
}

/**
 * @brief - determine (parameter0 - parameter1) >= parameter2
 * @brief - !!! MUST BE USED WITH YMatrix::TcoinR(const Long64_t pltsi, const Long64_t pltsj, const Long64_t pltwdR)
 * @param pltsi units 1ns
 * @param pltsj units 1ns
 * @param pltwd pltsi and pltsj are considered coincidence in this time windows (pltwdL)
 * @return - true YES
 * @return - false NO
 */
inline bool YMatrix::TcoinL(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwdL)
{
    return (pltsi - pltsj) >= pltwdL;
}

/**
 * @brief - determine (parameter0 - parameter1) <= parameter2
 * @brief - !!! MUST BE USED WITH YMatrix::TcoinL(const Long64_t pltsi, const Long64_t pltsj, const Long64_t pltwdL)
 * @param pltsi units 1ns
 * @param pltsj units 1ns
 * @param pltwd pltsi and pltsj are considered coincidence in this time windows (pltwd)
 * @return - true YES
 * @return - false NO
 */
inline bool YMatrix::TcoinR(const Long64_t &pltsi, const Long64_t &pltsj, const Long64_t &pltwdR)
{
    return (pltsi - pltsj) <= pltwdR;
}

/**
 * @brief - judge which one is maximum
 * @brief - (pltsi > pltsj ? pltsi : pltsj)
 * @param pltsi units 1ns
 * @param pltsj units 1ns
 * @return Long64_t The maximun number
 */
inline Long64_t YMatrix::Max(const Long64_t &pltsi, const Long64_t &pltsj)
{
    return (pltsi > pltsj ? pltsi : pltsj);
}

/**
 * @brief - judge which one is minimum
 *@brief - (pltsi < pltsj ? pltsi : pltsj)
 * @param pltsi units 1ns
 * @param pltsj units 1ns
 * @return Long64_t The minimum number
 */
inline Long64_t YMatrix::Min(const Long64_t &pltsi, const Long64_t &pltsj)
{
    return (pltsi < pltsj ? pltsi : pltsj);
}

/**
 * @brief - determine whether this detector is germanium
 * @brief - id -=> germaniun : 1 - 36; LaBr_3 : 37 - 46; charged particle detector : 0
 * @brief - det -=> CsI + APD : 1 - 96; other : 97 - 125 (have null series number); gamma detector : 0
 * @param psid id number
 * @return -  charged particle detector -=> kschargedDetectorFlag
 * @return -  germanium detector (HPGe and CLOVER) -=> ksGeFlag
 * @return -  LaBr_3 -=> ksLaBrFlag
 */
inline short YMatrix::WhatDetector(const short &psid)
{
    return (psid > 0 ? (psid < 37 ? ksGeFlag : ksLaBrFlag) : kschargedDetectorFlag);
}

/**
 * @brief Identify whether histogram named para0  exists
 *
 * @param pshname (TString) histogram name
 * @return true YES
 * @return false NO
 */
inline bool YMatrix::HistogramExist(const TString &pshname)
{
    return (gROOT->FindObjectAny(("h1" + pshname + "PAM").Data()) != nullptr);
}

YMatrix::~YMatrix()
{
}
