/*
 * @Date: 2023-11-07 01:42:15
 * @LastEditTime: 2023-11-21 21:06:09
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

/**
 * @brief This function is aimed to create a series of histogram
 *
 */
void YMatrix::CreateHistogram()
{
    // th3count = 0;

    InitBP();

    for (short i = 0; i < (ALPHAPTCMAX - ALPHAPTCMIN + 1); i++)
    {
        for (short j = 0; j < (PROTONPTCMAX - PROTONPTCMIN + 1); j++)
        {
            for (short k = 0; k < (TOTALPTCMAX - TOTALPTCMIN + 1); k++)
            {
                if ((i + j) > k)
                    continue;

                shggaptname = TString::Format("alpha%hd_proton%hd_total%hd", (i + ALPHAPTCMIN), (j + PROTONPTCMIN), (k + TOTALPTCMIN));
                shggatname = TString::Format("alpha%hd_total%hd", (i + ALPHAPTCMIN), (k + TOTALPTCMIN));
                shggptname = TString::Format("proton%hd_total%hd", (j + PROTONPTCMIN), (k + TOTALPTCMIN));
                shggapname = TString::Format("alpha%hd_proton%hd", (i + ALPHAPTCMIN), (j + PROTONPTCMIN));
                if (bapt[i][j][k])
                {
                    bapt[i][j][k] = false;
                    CreateHistogram(h1ggaptPAM, shggaptname + "PAM");
                    CreateHistogram(h1ggaptAM, shggaptname + "AM");
                    CreateHistogram(h1ggaptPM, shggaptname + "PM");
                    h1ggaptPM = (YTH1 *)gROOT->FindObject(("h1" + shggaptname + "PM").Data());
                    if (h1ggaptPM != nullptr)
                        h1ggaptPM->Sumw2(1);
                    else
                        cerr << "Can't find h1" << shggaptname << "PM" << endl;
                    h1ggaptPM = nullptr;
#ifdef GEGE
                    CreateHistogram(h2ggaptPAM, shggaptname + "PAM");
#ifdef GEGEAM
                    CreateHistogram(h2ggaptAM, shggaptname + "AM");
                    CreateHistogram(h2ggaptPM, shggaptname + "PM");
                    h2ggaptPM = (YTH2 *)gROOT->FindObject(("h2" + shggaptname + "PM").Data());
                    if (h2ggaptPM != nullptr)
                        h2ggaptPM->Sumw2(1);
                    else
                        cerr << "Can't find h2" << shggaptname << "PM" << endl;
                    h2ggaptPM = nullptr;
#endif
#endif
#ifdef CUBEMATRIX
                    CreateHistogram(h3ggaptPAM, shggaptname + "PAM");
#ifdef CUBEMATRIXAM
                    CreateHistogram(h3ggaptAM, shggaptname + "AM");
                    CreateHistogram(h3ggaptPM, shggaptname + "PM");
                    h3ggaptPM = (YTH3 *)gROOT->FindObject(("h3" + shggaptname + "PM").Data());
                    if (h3ggaptPM != nullptr)
                        h3ggaptPM->Sumw2(1);
                    else
                        cerr << "Can't find h3" << shggaptname << "PM" << endl;
                    h3ggaptPM = nullptr;
#endif
#endif
                }
                if (bat[i][k])
                {
                    bat[i][k] = false;
                    CreateHistogram(h1ggatPAM, shggatname + "PAM");
                    CreateHistogram(h1ggatAM, shggatname + "AM");
                    CreateHistogram(h1ggatPM, shggatname + "PM");
                    h1ggatPM = (YTH1 *)gROOT->FindObject(("h1" + shggatname + "PM").Data());
                    if (h1ggatPM != nullptr)
                        h1ggatPM->Sumw2(1);
                    else
                        cerr << "Can't find h1" << shggatname << "PM" << endl;
                    h1ggatPM = nullptr;
#ifdef GEGE
                    CreateHistogram(h2ggatPAM, shggatname + "PAM");
#ifdef GEGEAM
                    CreateHistogram(h2ggatAM, shggatname + "AM");
                    CreateHistogram(h2ggatPM, shggatname + "PM");
                    h2ggatPM = (YTH2 *)gROOT->FindObject(("h2" + shggatname + "PM").Data());
                    if (h2ggatPM != nullptr)
                        h2ggatPM->Sumw2(1);
                    else
                        cerr << "Can't find h2" << shggatname << "PM" << endl;
                    h2ggatPM = nullptr;
#endif
#endif
#ifdef CUBEMATRIX
                    CreateHistogram(h3ggatPAM, shggatname + "PAM");
#ifdef CUBEMATRIXAM
                    CreateHistogram(h3ggatAM, shggatname + "AM");
                    CreateHistogram(h3ggatPM, shggatname + "PM");
                    h3ggatPM = (YTH3 *)gROOT->FindObject(("h3" + shggatname + "PM").Data());
                    if (h3ggatPM != nullptr)
                        h3ggatPM->Sumw2(1);
                    else
                        cerr << "Can't find h3" << shggatname << "PM" << endl;
                    h3ggatPM = nullptr;
#endif
#endif
                }
                if (bpt[j][k])
                {
                    bpt[j][k] = false;
                    CreateHistogram(h1ggptPAM, shggptname + "PAM");
                    CreateHistogram(h1ggptAM, shggptname + "AM");
                    CreateHistogram(h1ggptPM, shggptname + "PM");
                    h1ggptPM = (YTH1 *)gROOT->FindObject(("h1" + shggptname + "PM").Data());
                    if (h1ggptPM != nullptr)
                        h1ggptPM->Sumw2(1);
                    else
                        cerr << "Can't find h1" << shggptname << "PM" << endl;
                    h1ggptPM = nullptr;
#ifdef GEGE
                    CreateHistogram(h2ggptPAM, shggptname + "PAM");
#ifdef GEGEAM
                    CreateHistogram(h2ggptAM, shggptname + "AM");
                    CreateHistogram(h2ggptPM, shggptname + "PM");
                    h2ggptPM = (YTH2 *)gROOT->FindObject(("h2" + shggptname + "PM").Data());
                    if (h2ggptPM != nullptr)
                        h2ggptPM->Sumw2(1);
                    else
                        cerr << "Can't find h2" << shggptname << "PM" << endl;
                    h2ggptPM = nullptr;
#endif
#endif
#ifdef CUBEMATRIX
                    CreateHistogram(h3ggptPAM, shggptname + "PAM");
#ifdef CUBEMATRIXAM
                    CreateHistogram(h3ggptAM, shggptname + "AM");
                    CreateHistogram(h3ggptPM, shggptname + "PM");
                    h3ggptPM = (YTH3 *)gROOT->FindObject(("h3" + shggptname + "PM").Data());
                    if (h3ggptPM != nullptr)
                        h3ggptPM->Sumw2(1);
                    else
                        cerr << "Can't find h3" << shggptname << "PM" << endl;
                    h3ggptPM = nullptr;
#endif
#endif
                }
                if (bap[i][j])
                {
                    bap[i][j] = false;
                    CreateHistogram(h1ggapPAM, shggapname + "PAM");
                    CreateHistogram(h1ggapAM, shggapname + "AM");
                    CreateHistogram(h1ggapPM, shggapname + "PM");
                    h1ggapPM = (YTH1 *)gROOT->FindObject(("h1" + shggapname + "PM").Data());
                    if (h1ggapPM != nullptr)
                        h1ggapPM->Sumw2(1);
                    else
                        cerr << "Can't find h1" << shggapname << "PM" << endl;
                    h1ggapPM = nullptr;
#ifdef GEGE
                    CreateHistogram(h2ggapPAM, shggapname + "PAM");
#ifdef GEGEAM
                    CreateHistogram(h2ggapAM, shggapname + "AM");
                    CreateHistogram(h2ggapPM, shggapname + "PM");
                    h2ggapPM = (YTH2 *)gROOT->FindObject(("h2" + shggapname + "PM").Data());
                    if (h2ggapPM != nullptr)
                        h2ggapPM->Sumw2(1);
                    else
                        cerr << "Can't find h2" << shggapname << "PM" << endl;
                    h2ggapPM = nullptr;
#endif
#endif
#ifdef CUBEMATRIX
                    CreateHistogram(h3ggapPAM, shggapname + "PAM");
#ifdef CUBEMATRIXAM
                    CreateHistogram(h3ggapAM, shggapname + "AM");
                    CreateHistogram(h3ggapPM, shggapname + "PM");
                    h3ggapPM = (YTH3 *)gROOT->FindObject(("h3" + shggapname + "PM").Data());
                    if (h3ggapPM != nullptr)
                        h3ggapPM->Sumw2(1);
                    else
                        cerr << "Can't find h3" << shggapname << "PM" << endl;
                    h3ggapPM = nullptr;
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
 * @param ph1 the pointer of histogram, getting the class name only
 * @param pshname  histogram named h1 + hname or h2 + hname or h3 + hname (There is no "+" in the histogram name)
 * @return - true crate
 * @return - false no crate
 */
bool YMatrix::CreateHistogram(YTH1 *phistogram, const TString &pshname)
{
    phistogram = new YTH1(("h1" + pshname).Data(), (pshname + "_projection").Data(), EBINNUMBER, EMIN, EMAX);
    if (phistogram != nullptr)
    {
        return true;
    }
    else
    {
        cerr << "Can't create " << phistogram->Class_Name() << "named " << pshname << " histogram!" << endl;
        return false;
    }
}
bool YMatrix::CreateHistogram(YTH2 *phistogram, const TString &pshname)
{
    phistogram = new YTH2(("h2" + pshname).Data(), (pshname + "_2matrix").Data(), EBINNUMBER, EMIN, EMAX, EBINNUMBER, EMIN, EMAX);
    if (phistogram != nullptr)
    {
        return true;
    }
    else
    {
        cerr << "Can't create " << phistogram->Class_Name() << "named " << pshname << " histogram!" << endl;
        return false;
    }
}
bool YMatrix::CreateHistogram(YTH3 *phistogram, const TString &pshname)
{
    phistogram = new YTH3(("h3" + pshname).Data(), (pshname + "_3matrix").Data(), EBINNUMBER, EMIN, EMAX, EBINNUMBER, EMIN, EMAX, EBINNUMBER, EMIN, EMAX);
    if (phistogram != nullptr)
    {
        return true;
    }
    else
    {
        cerr << "Can't create " << phistogram->Class_Name() << "named " << pshname << " histogram!" << endl;
        return false;
    }
}

/**
 * @brief Identify whether histogram named para0  exists
 *
 * @param pshname (TString) histogram name
 * @return true YES
 * @return false NO
 */
bool YMatrix::FindHistogram(YTH1 *&ph, const TString &pshname)
{
    ph = (YTH1 *)gROOT->FindObjectAny(("h1" + pshname).Data());
    if (ph != nullptr)
        return true;
    else
    {
        cerr << "Can't find YTH1 histogram" << pshname << endl;
        return false;
    }
}

bool YMatrix::FindHistogram(YTH2 *&ph, const TString &pshname)
{
    ph = (YTH2 *)gROOT->FindObjectAny(("h2" + pshname).Data());
    if (ph != nullptr)
        return true;
    else
    {
        cerr << "Can't find YTH2 histogram" << pshname << endl;
        return false;
    }
}

bool YMatrix::FindHistogram(YTH3 *&ph, const TString &pshname)
{
    ph = (YTH3 *)gROOT->FindObjectAny(("h3" + pshname).Data());
    if (ph != nullptr)
        return true;
    else
    {
        cerr << "Can't find YTH3 histogram" << pshname << endl;
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
    // cout << pa << "\t" << pp << "\t" << pt << endl;
    // cout << pde1 << "\t" << pde2 << "\t" << pde3 << endl;

    shggaptname = TString::Format("alpha%hd_proton%hd_total%hd", pa, pp, pt);
    shggatname = TString::Format("alpha%hd_total%hd", pa, pt);
    shggptname = TString::Format("proton%hd_total%hd", pp, pt);
    shggapname = TString::Format("alpha%hd_proton%hd", pa, pp);
    /*------------- PAM ------------*/
    // cout << shggaptname << "\t" << shggatname << "\t" << shggptname << "\t" << shggapname << endl;
    // do apt
    if (((ioption / 1000) % 10) == 1)
    {
        if (FindHistogram(h1ggaptPAM, (shggaptname + "PAM")))
        {
            ifapt1 = 1;
            h1ggaptPAM->Fill(pde1);
            // cout << pa << "1\t" << pp << "\t" << pt << endl;

            if (pbsym)
                h1ggaptPAM->Fill(pde2);
#ifdef GEGE
            // cout << pa << "2\t" << pp << "\t" << pt << endl;

            FindHistogram(h2ggaptPAM, (shggaptname + "PAM"));
            h2ggaptPAM->Fill(pde1, pde2);

            if (pbsym)
                h2ggaptPAM->Fill(pde2, pde1);
#endif
#ifdef CUBEMATRIX
            // cout << pa << "3\t" << pp << "\t" << pt << endl;

            FindHistogram(h3ggaptPAM, (shggaptname + "PAM"));
            h3ggaptPAM->Fill(pde1, pde2, pde3);
            // cout << pa << "3_\t" << pp << "\t" << pt << endl;

            if (pbsym)
            {
                h3ggaptPAM->Fill(pde1, pde3, pde2);
                h3ggaptPAM->Fill(pde2, pde1, pde3);
                h3ggaptPAM->Fill(pde2, pde3, pde1);
                h3ggaptPAM->Fill(pde3, pde1, pde2);
                h3ggaptPAM->Fill(pde3, pde2, pde1);
            }
#endif
        }
        else
            ifapt1 = 0;
    }

    // do at
    if (((ioption / 100) % 10) == 1)
    {
        if (FindHistogram(h1ggatPAM, (shggatname + "PAM")))
        {
            ifat1 = 1;
            h1ggatPAM->Fill(pde1);

            if (pbsym)
                h1ggatPAM->Fill(pde2);
#ifdef GEGE
            FindHistogram(h2ggatPAM, (shggatname + "PAM"));
            h2ggatPAM->Fill(pde1, pde2);

            if (pbsym)
                h2ggatPAM->Fill(pde2, pde1);
#endif

#ifdef CUBEMATRIX
            FindHistogram(h3ggatPAM, (shggatname + "PAM"));
            h3ggatPAM->Fill(pde1, pde2, pde3);

            if (pbsym)
            {
                h3ggatPAM->Fill(pde1, pde3, pde2);
                h3ggatPAM->Fill(pde2, pde1, pde3);
                h3ggatPAM->Fill(pde2, pde3, pde1);
                h3ggatPAM->Fill(pde3, pde1, pde2);
                h3ggatPAM->Fill(pde3, pde2, pde1);
            }
#endif
        }
        else
            ifat1 = 0;
    }

    // do pt
    if (((ioption / 10) % 10) == 1)
    {
        if (FindHistogram(h1ggptPAM, (shggptname + "PAM")))
        {
            ifpt1 = 1;
            h1ggptPAM->Fill(pde1);

            if (pbsym)
                h1ggptPAM->Fill(pde2);
#ifdef GEGE
            FindHistogram(h2ggptPAM, (shggptname + "PAM"));
            h2ggptPAM->Fill(pde1, pde2);

            if (pbsym)
                h2ggptPAM->Fill(pde2, pde1);
#endif
#ifdef CUBEMATRIX
            FindHistogram(h3ggptPAM, (shggptname + "PAM"));
            h3ggptPAM->Fill(pde1, pde2, pde3);

            if (pbsym)
            {
                h3ggptPAM->Fill(pde1, pde3, pde2);
                h3ggptPAM->Fill(pde2, pde1, pde3);
                h3ggptPAM->Fill(pde2, pde3, pde1);
                h3ggptPAM->Fill(pde3, pde1, pde2);
                h3ggptPAM->Fill(pde3, pde2, pde1);
            }
#endif
        }
        else
            ifpt1 = 0;
    }

    // do ap
    if (((ioption / 1) % 10) == 1)
    {
        if (FindHistogram(h1ggapPAM, (shggapname + "PAM")))
        {
            ifap1 = 1;
            h1ggapPAM->Fill(pde1);

            if (pbsym)
                h1ggapPAM->Fill(pde2);
#ifdef GEGE
            FindHistogram(h2ggapPAM, (shggapname + "PAM"));
            h2ggapPAM->Fill(pde1, pde2);

            if (pbsym)
                h2ggapPAM->Fill(pde2, pde1);
#endif
#ifdef CUBEMATRIX
            FindHistogram(h3ggapPAM, (shggapname + "PAM"));
            h3ggapPAM->Fill(pde1, pde2, pde3);

            if (pbsym)
            {
                h3ggapPAM->Fill(pde1, pde3, pde2);
                h3ggapPAM->Fill(pde2, pde1, pde3);
                h3ggapPAM->Fill(pde2, pde3, pde1);
                h3ggapPAM->Fill(pde3, pde1, pde2);
                h3ggapPAM->Fill(pde3, pde2, pde1);
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

        if (FindHistogram(h1ggaptAM, (shggaptname + "AM")))
        {
            ifapt2 = 2;
            h1ggaptAM->Fill(pde1);
            if (pbsym)
                h1ggaptAM->Fill(pde2);
#ifdef GEGE
#ifdef GEGEAM
            FindHistogram(h2ggaptAM, (shggaptname + "AM"));
            h2ggaptAM->Fill(pde1, pde2);
            if (pbsym)
                h2ggaptAM->Fill(pde2, pde1);
#endif
#endif
#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
            FindHistogram(h3ggaptAM, (shggaptname + "AM"));
            h3ggaptAM->Fill(pde1, pde2, pde3);

            if (pbsym)
            {
                h3ggaptAM->Fill(pde1, pde3, pde2);
                h3ggaptAM->Fill(pde2, pde1, pde3);
                h3ggaptAM->Fill(pde2, pde3, pde1);
                h3ggaptAM->Fill(pde3, pde1, pde2);
                h3ggaptAM->Fill(pde3, pde2, pde1);
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

        if (FindHistogram(h1ggatAM, (shggatname + "AM")))
        {
            ifat2 = 2;
            h1ggatAM->Fill(pde1);

            if (pbsym)
                h1ggatAM->Fill(pde2);
#ifdef GEGE
#ifdef GEGEAM
            FindHistogram(h2ggatAM, (shggatname + "AM"));
            h2ggatAM->Fill(pde1, pde2);

            if (pbsym)
                h2ggatAM->Fill(pde2, pde1);
#endif
#endif
#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
            FindHistogram(h3ggatAM, (shggatname + "AM"));
            h3ggatAM->Fill(pde1, pde2, pde3);

            if (pbsym)
            {
                h3ggatAM->Fill(pde1, pde3, pde2);
                h3ggatAM->Fill(pde2, pde1, pde3);
                h3ggatAM->Fill(pde2, pde3, pde1);
                h3ggatAM->Fill(pde3, pde1, pde2);
                h3ggatAM->Fill(pde3, pde2, pde1);
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

        if (FindHistogram(h1ggptAM, (shggptname + "AM")))
        {
            ifpt2 = 2;
            h1ggptAM->Fill(pde1);

            if (pbsym)
                h1ggptAM->Fill(pde2);
#ifdef GEGE
#ifdef GEGEAM
            FindHistogram(h2ggptAM, (shggptname + "AM"));
            h2ggptAM->Fill(pde1, pde2);

            if (pbsym)
                h2ggptAM->Fill(pde2, pde1);
#endif
#endif
#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
            FindHistogram(h3ggptAM, (shggptname + "AM"));
            h3ggptAM->Fill(pde1, pde2, pde3);

            if (pbsym)
            {
                h3ggptAM->Fill(pde1, pde3, pde2);
                h3ggptAM->Fill(pde2, pde1, pde3);
                h3ggptAM->Fill(pde2, pde3, pde1);
                h3ggptAM->Fill(pde3, pde1, pde2);
                h3ggptAM->Fill(pde3, pde2, pde1);
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

        if (FindHistogram(h1ggapAM, (shggapname + "AM")))

        {
            ifap2 = 2;
            h1ggapAM->Fill(pde1);

            if (pbsym)
                h1ggapAM->Fill(pde2);
#ifdef GEGE
#ifdef GEGEAM
            FindHistogram(h2ggapAM, (shggapname + "AM"));
            h2ggapAM->Fill(pde1, pde2);

            if (pbsym)
                h2ggapAM->Fill(pde2, pde1);
#endif
#endif
#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
            FindHistogram(h3ggapAM, (shggapname + "AM"));
            h3ggapAM->Fill(pde1, pde2, pde3);

            if (pbsym)
            {
                h3ggapAM->Fill(pde1, pde3, pde2);
                h3ggapAM->Fill(pde2, pde1, pde3);
                h3ggapAM->Fill(pde2, pde3, pde1);
                h3ggapAM->Fill(pde3, pde1, pde2);
                h3ggapAM->Fill(pde3, pde2, pde1);
            }
#endif
#endif
        }
        else
            ifap2 = 0;
    }

    InitPtr();
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
                InitHitVar();
                PtcAdd(alpha[cfirstlp], proton[cfirstlp], total[cfirstlp]);
                ltsmin = ltscFirst;
                ltsmax = ltscFirst;
                bfirstLoopHit = false;
            }
            else
            {
                if (ltscFirst > ltsmax) // the begining of next chain charged particle
                {
                    InitHitVar();
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

                // This is a problem detector
                if (id[gfristlp] == 1)
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

                    // This is a problem detector
                    if (id[gsecondlp] == 1)
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
                        InitHitgpam();
                    }

#endif

#ifndef CUBEMATRIXAM
                    if (bamgFirstgSecond)
                    {
                        if (!FillHistogram(SYMMETRICAL, dpl[gfristlp], dpl[gsecondlp], salphaHit, sprotonHit, stotalHit, "2222"))
                            cerr << "can't fill histogram AM, jentry = " << jentry << endl;
                        InitHitgam();
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

                        // This is a problem detector
                        if (id[gthirdlp] == 1)
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

                            InitHitgpam();
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
                            if (!FillHistogram(SYMMETRICAL, dpl[gfristlp], dpl[gsecondlp], salphaHit, sprotonHit, stotalHit, "2222", dpl[gthirdlp]))
                                cerr << "can't fill histogram PAM, jentry = " << jentry << endl;

                            InitHitgam();
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

    double dPAMT = GEGECOINWINDOWS * 2.;
    double dAMT = TMath::Abs(GEGEAMWINDOWR1 - GEGEAMWINDOWL1) + TMath::Abs(GEGEAMWINDOWR2 - GEGEAMWINDOWL2);
    double dweight = -dPAMT / dAMT;

    InitBP();
    for (short i = 0; i < (ALPHAPTCMAX - ALPHAPTCMIN + 1); i++)
        for (short j = 0; j < (PROTONPTCMAX - PROTONPTCMIN + 1); j++)
            for (short k = 0; k < (TOTALPTCMAX - TOTALPTCMIN + 1); k++)
            {
                if ((i + j) > k)
                    continue;

                shggaptname = TString::Format("alpha%hd_proton%hd_total%hd", (i + ALPHAPTCMIN), (j + PROTONPTCMIN), (k + TOTALPTCMIN));
                shggatname = TString::Format("alpha%hd_total%hd", (i + ALPHAPTCMIN), (k + TOTALPTCMIN));
                shggptname = TString::Format("proton%hd_total%hd", (j + PROTONPTCMIN), (k + TOTALPTCMIN));
                shggapname = TString::Format("alpha%hd_proton%hd", (i + ALPHAPTCMIN), (j + PROTONPTCMIN));
                printf("\r GET PM process: alpha%hd_proton%hd_total%hd", (i + ALPHAPTCMIN), (j + PROTONPTCMIN), (k + TOTALPTCMIN));
                if (bapt[i][j][k])
                {
                    bapt[i][j][k] = false;
                    if (FindHistogram(h1ggaptPM, shggaptname + "PM"))
                    {
                        FindHistogram(h1ggaptPAM, shggaptname + "PAM");
                        FindHistogram(h1ggaptAM, shggaptname + "AM");
                        h1ggaptPM->Add(h1ggaptPAM, h1ggaptAM, 1, dweight);
                    }
#ifdef GEGE
#ifdef GEGEAM
                    if (FindHistogram(h2ggaptPM, shggaptname + "PM"))
                    {
                        FindHistogram(h2ggaptPAM, shggaptname + "PAM");
                        FindHistogram(h2ggaptAM, shggaptname + "AM");
                        h2ggaptPM->Add(h2ggaptPAM, h2ggaptAM, 1, dweight);
                    }
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
                    if (FindHistogram(h3ggaptPM, shggaptname + "PM"))
                    {
                        FindHistogram(h3ggaptPAM, shggaptname + "PAM");
                        FindHistogram(h3ggaptAM, shggaptname + "AM");
                        h3ggaptPM->Add(h3ggaptPAM, h3ggaptAM, 1, dweight);
                    }
#endif
#endif
                }
                if (bat[i][k])
                {
                    bat[i][k] = false;

                    if (FindHistogram(h1ggatPM, shggatname + "PM"))
                    {
                        FindHistogram(h1ggatPAM, shggatname + "PAM");
                        FindHistogram(h1ggatAM, shggatname + "AM");
                        h1ggatPM->Add(h1ggatPAM, h1ggatAM, 1, dweight);
                    }
#ifdef GEGE
#ifdef GEGEAM
                    if (FindHistogram(h2ggatPM, shggatname + "PM"))
                    {
                        FindHistogram(h2ggatPAM, shggatname + "PAM");
                        FindHistogram(h2ggatAM, shggatname + "AM");
                        h2ggatPM->Add(h2ggatPAM, h2ggatAM, 1, dweight);
                    }
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
                    if (FindHistogram(h3ggatPM, shggatname + "PM"))
                    {
                        FindHistogram(h3ggatPAM, shggatname + "PAM");
                        FindHistogram(h3ggatAM, shggatname + "AM");
                        h3ggatPM->Add(h3ggatPAM, h3ggatAM, 1, dweight);
                    }
#endif
#endif
                }
                if (bpt[j][k])
                {
                    bpt[j][k] = false;

                    if (FindHistogram(h1ggptPM, shggptname + "PM"))
                    {
                        FindHistogram(h1ggptPAM, shggptname + "PAM");
                        FindHistogram(h1ggptAM, shggptname + "AM");
                        h1ggptPM->Add(h1ggptPAM, h1ggptAM, 1, dweight);
                    }
#ifdef GEGE
#ifdef GEGEAM
                    if (FindHistogram(h2ggptPM, shggptname + "PM"))
                    {
                        FindHistogram(h2ggptPAM, shggptname + "PAM");
                        FindHistogram(h2ggptAM, shggptname + "AM");
                        h2ggptPM->Add(h2ggptPAM, h2ggptAM, 1, dweight);
                    }
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
                    if (FindHistogram(h3ggptPM, shggptname + "PM"))
                    {
                        FindHistogram(h3ggptPAM, shggptname + "PAM");
                        FindHistogram(h3ggptAM, shggptname + "AM");
                        h3ggptPM->Add(h3ggptPAM, h3ggptAM, 1, dweight);
                    }
#endif
#endif
                }
                if (bap[i][j])
                {
                    bap[i][j] = false;

                    if (FindHistogram(h1ggapPM, shggapname + "PM"))
                    {
                        FindHistogram(h1ggapPAM, shggapname + "PAM");
                        FindHistogram(h1ggapAM, shggapname + "AM");
                        h1ggapPM->Add(h1ggapPAM, h1ggapAM, 1, dweight);
                    }
#ifdef GEGE
#ifdef GEGEAM
                    if (FindHistogram(h2ggapPM, shggapname + "PM"))
                    {
                        FindHistogram(h2ggapPAM, shggapname + "PAM");
                        FindHistogram(h2ggapAM, shggapname + "AM");
                        h2ggapPM->Add(h2ggapPAM, h2ggapAM, 1, dweight);
                    }
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
                    if (FindHistogram(h3ggapPM, shggapname + "PM"))
                    {
                        FindHistogram(h3ggapPAM, shggapname + "PAM");
                        FindHistogram(h3ggapAM, shggapname + "AM");
                        h3ggapPM->Add(h3ggapPAM, h3ggapAM, 1, dweight);
                    }
#endif
#endif
                }
                InitPtr();
            }
    cout << endl;
    return true;
}

bool YMatrix::StoreFile()
{
    if (fopf == nullptr)
        return false;
    InitBP();

    for (short i = 0; i < (ALPHAPTCMAX - ALPHAPTCMIN + 1); i++)
        for (short j = 0; j < (PROTONPTCMAX - PROTONPTCMIN + 1); j++)
            for (short k = 0; k < (TOTALPTCMAX - TOTALPTCMIN + 1); k++)
            {
                if ((i + j) > k)
                    continue;

                shggaptname = TString::Format("alpha%hd_proton%hd_total%hd", (i + ALPHAPTCMIN), (j + PROTONPTCMIN), (k + TOTALPTCMIN));
                shggatname = TString::Format("alpha%hd_total%hd", (i + ALPHAPTCMIN), (k + TOTALPTCMIN));
                shggptname = TString::Format("proton%hd_total%hd", (j + PROTONPTCMIN), (k + TOTALPTCMIN));
                shggapname = TString::Format("alpha%hd_proton%hd", (i + ALPHAPTCMIN), (j + PROTONPTCMIN));
                // This function will store and delete historgram
                // so one may not use bool variable to judge whether it has been stored
                if (bapt[i][j][k])
                {
                    bapt[i][j][k] = false;
                    FindHistogram(h1ggaptPAM, shggaptname + "PAM");
                    FindHistogram(h1ggaptAM, shggaptname + "AM");
                    FindHistogram(h1ggaptPM, shggaptname + "PM");

                    StoreHistogram(h1ggaptPAM, "PAM/TH1/AlphaProtonTotal");
                    StoreHistogram(h1ggaptAM, "AM/TH1/AlphaProtonTotal");
                    StoreHistogram(h1ggaptPM, "PM/TH1/AlphaProtonTotal");
#ifdef GEGE
                    FindHistogram(h2ggaptPAM, shggaptname + "PAM");
                    StoreHistogram(h2ggaptPAM, "PAM/TH2/AlphaProtonTotal");
#ifdef GEGEAM
                    FindHistogram(h2ggaptAM, shggaptname + "AM");
                    FindHistogram(h2ggaptPM, shggaptname + "PM");
                    StoreHistogram(h2ggaptAM, "AM/TH2/AlphaProtonTotal");
                    StoreHistogram(h2ggaptPM, "PM/TH2/AlphaProtonTotal");
#endif
#endif
#ifdef CUBEMATRIX
                    FindHistogram(h3ggaptPAM, shggaptname + "PAM");
                    StoreHistogram(h3ggaptPAM, "PAM/TH3/AlphaProtonTotal");
#ifdef CUBEMATRIXAM
                    FindHistogram(h3ggaptAM, shggaptname + "AM");
                    FindHistogram(h3ggaptPM, shggaptname + "PM");
                    StoreHistogram(h3ggaptAM, "AM/TH3/AlphaProtonTotal");
                    StoreHistogram(h3ggaptPM, "PM/TH3/AlphaProtonTotal");
#endif
#endif
                }
                if (bat[i][k])
                {
                    bat[i][k] = false;
                    FindHistogram(h1ggatPAM, shggatname + "PAM");
                    FindHistogram(h1ggatAM, shggatname + "AM");
                    FindHistogram(h1ggatPM, shggatname + "PM");

                    StoreHistogram(h1ggatPAM, "PAM/TH1/AlphaProtonTotal");
                    StoreHistogram(h1ggatAM, "AM/TH1/AlphaProtonTotal");
                    StoreHistogram(h1ggatPM, "PM/TH1/AlphaProtonTotal");
#ifdef GEGE
                    FindHistogram(h2ggatPAM, shggatname + "PAM");
                    StoreHistogram(h2ggatPAM, "PAM/TH2/AlphaProtonTotal");
#ifdef GEGEAM
                    FindHistogram(h2ggatAM, shggatname + "AM");
                    FindHistogram(h2ggatPM, shggatname + "PM");
                    StoreHistogram(h2ggatAM, "AM/TH2/AlphaProtonTotal");
                    StoreHistogram(h2ggatPM, "PM/TH2/AlphaProtonTotal");
#endif
#endif
#ifdef CUBEMATRIX
                    FindHistogram(h3ggatPAM, shggatname + "PAM");
                    StoreHistogram(h3ggatPAM, "PAM/TH3/AlphaProtonTotal");
#ifdef CUBEMATRIXAM
                    FindHistogram(h3ggatAM, shggatname + "AM");
                    FindHistogram(h3ggatPM, shggatname + "PM");
                    StoreHistogram(h3ggatAM, "AM/TH3/AlphaProtonTotal");
                    StoreHistogram(h3ggatPM, "PM/TH3/AlphaProtonTotal");
#endif
#endif
                }
                if (bpt[j][k])
                {
                    bpt[j][k] = false;
                    FindHistogram(h1ggptPAM, shggptname + "PAM");
                    FindHistogram(h1ggptAM, shggptname + "AM");
                    FindHistogram(h1ggptPM, shggptname + "PM");

                    StoreHistogram(h1ggptPAM, "PAM/TH1/AlphaProtonTotal");
                    StoreHistogram(h1ggptAM, "AM/TH1/AlphaProtonTotal");
                    StoreHistogram(h1ggptPM, "PM/TH1/AlphaProtonTotal");
#ifdef GEGE
                    FindHistogram(h2ggptPAM, shggptname + "PAM");
                    StoreHistogram(h2ggptPAM, "PAM/TH2/AlphaProtonTotal");
#ifdef GEGEAM
                    FindHistogram(h2ggptAM, shggptname + "AM");
                    FindHistogram(h2ggptPM, shggptname + "PM");
                    StoreHistogram(h2ggptAM, "AM/TH2/AlphaProtonTotal");
                    StoreHistogram(h2ggptPM, "PM/TH2/AlphaProtonTotal");
#endif
#endif
#ifdef CUBEMATRIX
                    FindHistogram(h3ggptPAM, shggptname + "PAM");
                    StoreHistogram(h3ggptPAM, "PAM/TH3/AlphaProtonTotal");
#ifdef CUBEMATRIXAM
                    FindHistogram(h3ggptAM, shggptname + "AM");
                    FindHistogram(h3ggptPM, shggptname + "PM");
                    StoreHistogram(h3ggptAM, "AM/TH3/AlphaProtonTotal");
                    StoreHistogram(h3ggptPM, "PM/TH3/AlphaProtonTotal");
#endif
#endif
                }
                if (bap[i][j])
                {
                    bap[i][j] = false;
                    FindHistogram(h1ggapPAM, shggapname + "PAM");
                    FindHistogram(h1ggapAM, shggapname + "AM");
                    FindHistogram(h1ggapPM, shggapname + "PM");

                    StoreHistogram(h1ggapPAM, "PAM/TH1/AlphaProtonTotal");
                    StoreHistogram(h1ggapAM, "AM/TH1/AlphaProtonTotal");
                    StoreHistogram(h1ggapPM, "PM/TH1/AlphaProtonTotal");
#ifdef GEGE
                    FindHistogram(h2ggapPAM, shggapname + "PAM");
                    StoreHistogram(h2ggapPAM, "PAM/TH2/AlphaProtonTotal");
#ifdef GEGEAM
                    FindHistogram(h2ggapAM, shggapname + "AM");
                    FindHistogram(h2ggapPM, shggapname + "PM");
                    StoreHistogram(h2ggapAM, "AM/TH2/AlphaProtonTotal");
                    StoreHistogram(h2ggapPM, "PM/TH2/AlphaProtonTotal");
#endif
#endif
#ifdef CUBEMATRIX
                    FindHistogram(h3ggapPAM, shggapname + "PAM");
                    StoreHistogram(h3ggapPAM, "PAM/TH3/AlphaProtonTotal");
#ifdef CUBEMATRIXAM
                    FindHistogram(h3ggapAM, shggapname + "AM");
                    FindHistogram(h3ggapPM, shggapname + "PM");
                    StoreHistogram(h3ggapAM, "AM/TH3/AlphaProtonTotal");
                    StoreHistogram(h3ggapPM, "PM/TH3/AlphaProtonTotal");
#endif
#endif
                }
                InitPtr();
            }
    return true;
}

inline void YMatrix::InitBP()
{
    InitBool();
    InitPtr();
}

inline void YMatrix::InitPtr()
{

    h1ggaptPAM = nullptr;
    h1ggatPAM = nullptr;
    h1ggptPAM = nullptr;
    h1ggapPAM = nullptr;

    h1ggaptAM = nullptr;
    h1ggatAM = nullptr;
    h1ggptAM = nullptr;
    h1ggapAM = nullptr;

    h1ggaptPM = nullptr;
    h1ggatPM = nullptr;
    h1ggptPM = nullptr;
    h1ggapPM = nullptr;

#ifdef GEGE
    h2ggaptPAM = nullptr;
    h2ggatPAM = nullptr;
    h2ggptPAM = nullptr;
    h2ggapPAM = nullptr;
#ifdef GEGEAM
    h2ggaptAM = nullptr;
    h2ggatAM = nullptr;
    h2ggptAM = nullptr;
    h2ggapAM = nullptr;

    h2ggaptPM = nullptr;
    h2ggatPM = nullptr;
    h2ggptPM = nullptr;
    h2ggapPM = nullptr;
#endif
#endif

#ifdef CUBEMATRIX
    h3ggaptPAM = nullptr;
    h3ggatPAM = nullptr;
    h3ggptPAM = nullptr;
    h3ggapPAM = nullptr;
#ifdef CUBEMATRIXAM
    h3ggaptAM = nullptr;
    h3ggatAM = nullptr;
    h3ggptAM = nullptr;
    h3ggapAM = nullptr;

    h3ggaptPM = nullptr;
    h3ggatPM = nullptr;
    h3ggptPM = nullptr;
    h3ggapPM = nullptr;
#endif
#endif
}

inline void YMatrix::InitBool()
{
    for (short i = 0; i < (ALPHAPTCMAX - ALPHAPTCMIN + 1); i++)
        for (short j = 0; j < (PROTONPTCMAX - PROTONPTCMIN + 1); j++)
            for (short k = 0; k < (TOTALPTCMAX - TOTALPTCMIN + 1); k++)
            {
                bapt[i][j][k] = true;
                bat[i][k] = true;
                bpt[j][k] = true;
                bap[i][j] = true;
            }
}

/**
 * @brief Initialize some varibale to zero or false
 *
 */
inline void YMatrix::InitHitVar()
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

inline void YMatrix::InitHitgpam()
{
    bpamgFirstgSecond = false;
    bpamgFirstgThird = false;
    bpamgSecondgThird = false;
}

inline void YMatrix::InitHitgam()
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

YMatrix::~YMatrix()
{
}
