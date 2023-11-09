/*
 * @Date: 2023-11-07 01:42:15
 * @LastEditTime: 2023-11-09 09:14:13
 */
#include "YMatrix.hh"

YMatrix::YMatrix(int irunBegin, int irunEnd) : schargedDetectorFlag(0), sGeFlag(1), sLaBrFlag(2)
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
        fchainFile->Close();
        delete fchainFile;
        fchainFile = nullptr;
        return true;
    }
    else
    {
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
        {
            cerr << "Can't read file: " << schainFileName << endl;
            continue;
        }

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
    if (ltotalEntry < 100LL)
        return;

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

    fopf = new TFile(TString::Format("%s%s%s%s%s%s_%d_%d.root", ROOTFILEPATH, ROOTFILENAME, GEGEFILE, GEGEAMFILE, CUBEMATRIXFILE, CUBEMATRIXAMFILE, irunBegin, irunEnd).Data(), "RECREATE");
    if (fopf->IsOpen())
    {
        cout << "Output file: " << fopf->GetPath() << endl;
    }
    else
    {
        cerr << "Can't create output file: " << TString::Format("%s%s_%d_%d.root", ROOTFILEPATH, ROOTFILENAME, irunBegin, irunEnd) << endl;
        exit(1);
    }

    fopf->mkdir("PAM/TH1/AlphaProtonTotal");
    fopf->mkdir("PAM/TH1/AlphaTotal");
    fopf->mkdir("PAM/TH1/ProtonTotal");
    fopf->mkdir("PAM/TH1/AlphaProton");

#ifdef GEGE
    fopf->mkdir("PAM/TH2/AlphaProtonTotal");
    fopf->mkdir("PAM/TH2/AlphaTotal");
    fopf->mkdir("PAM/TH2/ProtonTotal");
    fopf->mkdir("PAM/TH2/AlphaProton");
#ifdef GEGEAM

    fopf->mkdir("AM/TH1/AlphaProtonTotal");
    fopf->mkdir("AM/TH1/AlphaTotal");
    fopf->mkdir("AM/TH1/ProtonTotal");
    fopf->mkdir("AM/TH1/AlphaProton");

    fopf->mkdir("AM/TH2/AlphaProtonTotal");
    fopf->mkdir("AM/TH2/AlphaTotal");
    fopf->mkdir("AM/TH2/ProtonTotal");
    fopf->mkdir("AM/TH2/AlphaProton");

    fopf->mkdir("PM/TH1/AlphaTotal");
    fopf->mkdir("PM/TH1/AlphaProtonTotal");
    fopf->mkdir("PM/TH1/ProtonTotal");
    fopf->mkdir("PM/TH1/AlphaProton");

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
    for (short i = 0; i < (ALPHAPTCMAX + 1); i++)
        for (short j = 0; j < (PROTONPTCMAX + 1); j++)
            for (short k = 0; k < (TOTALPTCMAX + 1); k++)
            {
                h1ggaptPAM[i][j][k] = nullptr;
                h1ggatPAM[i][k] = nullptr;
                h1ggptPAM[j][k] = nullptr;
                h1ggapPAM[i][j] = nullptr;

#ifdef GEGE
                h2ggaptPAM[i][j][k] = nullptr;
                h2ggatPAM[i][k] = nullptr;
                h2ggptPAM[j][k] = nullptr;
                h2ggapPAM[i][j] = nullptr;
#ifdef GEGEAM
                h1ggaptAM[i][j][k] = nullptr;
                h1ggatAM[i][k] = nullptr;
                h1ggptAM[j][k] = nullptr;
                h1ggapAM[i][j] = nullptr;

                h2ggaptAM[i][j][k] = nullptr;
                h2ggatAM[i][k] = nullptr;
                h2ggptAM[j][k] = nullptr;
                h2ggapAM[i][j] = nullptr;

                h1ggaptPM[i][j][k] = nullptr;
                h1ggatPM[i][k] = nullptr;
                h1ggptPM[j][k] = nullptr;
                h1ggapPM[i][j] = nullptr;

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
    for (short i = 0; i < (ALPHAPTCMAX + 1); i++)
    {
        for (short j = 0; j < (PROTONPTCMAX + 1); j++)
        {
            for (short k = 0; k < (TOTALPTCMAX + 1); k++)
            {
                shggaptname = TString::Format("alpha%d_proton%d_total%d", i, j, k);
                shggatname = TString::Format("alpha%d_total%d", i, k);
                shggptname = TString::Format("proton%d_total%d", j, k);
                shggapname = TString::Format("alpha%d_proton%d", i, j);
                if (h1ggaptPAM[i][j][k] == nullptr)
                {

                    // h1ggaptPAM[i][j][k] = CreateHistogram<YTH1>(shggaptname + "PAM");
                    // h2ggaptPAM[i][j][k] = CreateHistogram<YTH2>(shggaptname + "PAM");
                    CreateHistogram(h1ggaptPAM[i][j][k], shggaptname + "PAM");
#ifdef GEGE
                    CreateHistogram(h2ggaptPAM[i][j][k], shggaptname + "PAM");
#ifdef GEGEAM
                    // h1ggaptAM[i][j][k] = CreateHistogram<YTH1>(shggaptname + "AM");
                    // h2ggaptAM[i][j][k] = CreateHistogram<YTH2>(shggaptname + "AM");
                    // h1ggaptPM[i][j][k] = CreateHistogram<YTH1>(shggaptname + "PM");
                    // h2ggaptPM[i][j][k] = CreateHistogram<YTH2>(shggaptname + "PM");
                    CreateHistogram(h1ggaptAM[i][j][k], shggaptname + "AM");
                    CreateHistogram(h2ggaptAM[i][j][k], shggaptname + "AM");
                    CreateHistogram(h1ggaptPM[i][j][k], shggaptname + "PM");
                    CreateHistogram(h2ggaptPM[i][j][k], shggaptname + "PM");
                    h1ggaptPM[i][j][k]->Sumw2(1);
                    h2ggaptPM[i][j][k]->Sumw2(1);

#endif
#endif

#ifdef CUBEMATRIX
                    // h3ggaptPAM[i][j][k] = CreateHistogram<YTH3>(shggaptname + "PAM");
                    CreateHistogram(h3ggaptPAM[i][j][k], shggaptname + "PAM");
#ifdef CUBEMATRIXAM
                    // h3ggaptAM[i][j][k] = CreateHistogram<YTH3>(shggaptname + "AM");
                    // h3ggaptPM[i][j][k] = CreateHistogram<YTH3>(shggaptname + "PM");
                    CreateHistogram(h3ggaptAM[i][j][k], shggaptname + "AM");
                    CreateHistogram(h3ggaptPM[i][j][k], shggaptname + "PM");
                    h3ggaptPM[i][j][k]->Sumw2(1);
#endif
#endif
                }
                if (h1ggatPAM[i][k] == nullptr)
                {
                    // h1ggatPAM[i][k] = CreateHistogram<YTH1>(shggatname + "PAM");
                    // h2ggatPAM[i][k] = CreateHistogram<YTH2>(shggatname + "PAM");
                    CreateHistogram(h1ggatPAM[i][k], shggatname + "PAM");
#ifdef GEGE
                    CreateHistogram(h2ggatPAM[i][k], shggatname + "PAM");

#ifdef GEGEAM
                    // h1ggatAM[i][k] = CreateHistogram<YTH1>(shggatname + "AM");
                    // h2ggatAM[i][k] = CreateHistogram<YTH2>(shggatname + "AM");
                    // h1ggatPM[i][k] = CreateHistogram<YTH1>(shggatname + "PM");
                    // h2ggatPM[i][k] = CreateHistogram<YTH2>(shggatname + "PM");
                    CreateHistogram(h1ggatAM[i][k], shggatname + "AM");
                    CreateHistogram(h2ggatAM[i][k], shggatname + "AM");
                    CreateHistogram(h1ggatPM[i][k], shggatname + "PM");
                    CreateHistogram(h2ggatPM[i][k], shggatname + "PM");
                    h1ggatPM[i][k]->Sumw2(1);
                    h2ggatPM[i][k]->Sumw2(1);

#endif
#endif

#ifdef CUBEMATRIX
                    // h3ggatPAM[i][k] = CreateHistogram<YTH3>(shggatname + "PAM");
                    CreateHistogram(h3ggatPAM[i][k], shggatname + "PAM");
#ifdef CUBEMATRIXAM
                    // h3ggatAM[i][k] = CreateHistogram<YTH3>(shggatname + "AM");
                    // h3ggatPM[i][k] = CreateHistogram<YTH3>(shggatname + "PM");
                    CreateHistogram(h3ggatAM[i][k], shggatname + "AM");
                    CreateHistogram(h3ggatPM[i][k], shggatname + "PM");
                    h3ggatPM[i][k]->Sumw2(1);

#endif
#endif
                }
                if (h1ggptPAM[j][k] == nullptr)
                {

                    // h1ggptPAM[j][k] = CreateHistogram<YTH1>(shggptname + "PAM");
                    // h2ggptPAM[j][k] = CreateHistogram<YTH2>(shggptname + "PAM");
                    CreateHistogram(h1ggptPAM[j][k], shggptname + "PAM");
#ifdef GEGE
                    CreateHistogram(h2ggptPAM[j][k], shggptname + "PAM");

#ifdef GEGEAM
                    // h1ggptAM[j][k] = CreateHistogram<YTH1>(shggptname + "AM");
                    // h2ggptAM[j][k] = CreateHistogram<YTH2>(shggptname + "AM");
                    // h1ggptPM[j][k] = CreateHistogram<YTH1>(shggptname + "PM");
                    // h2ggptPM[j][k] = CreateHistogram<YTH2>(shggptname + "PM");
                    CreateHistogram(h1ggptAM[j][k], shggptname + "AM");
                    CreateHistogram(h2ggptAM[j][k], shggptname + "AM");
                    CreateHistogram(h1ggptPM[j][k], shggptname + "PM");
                    CreateHistogram(h2ggptPM[j][k], shggptname + "PM");
                    h1ggptPM[j][k]->Sumw2(1);
                    h2ggptPM[j][k]->Sumw2(1);
#endif
#endif

#ifdef CUBEMATRIX
                    // h3ggptPAM[j][k] = CreateHistogram<YTH3>(shggptname + "PAM");
                    CreateHistogram(h3ggptPAM[j][k], shggptname + "PAM");
#ifdef CUBEMATRIXAM
                    // h3ggptAM[j][k] = CreateHistogram<YTH3>(shggptname + "AM");
                    // h3ggptPM[j][k] = CreateHistogram<YTH3>(shggptname + "PM");
                    CreateHistogram(h3ggptAM[j][k], shggptname + "AM");
                    CreateHistogram(h3ggptPM[j][k], shggptname + "PM");
                    h3ggptPM[j][k]->Sumw2(1);
#endif
#endif
                }
                if (h1ggapPAM[i][j] == nullptr)
                {

                    // h1ggapPAM[i][j] = CreateHistogram<YTH1>(shggapname + "PAM");
                    // h2ggapPAM[i][j] = CreateHistogram<YTH2>(shggapname + "PAM");
                    CreateHistogram(h1ggapPAM[i][j], shggapname + "PAM");
#ifdef GEGE
                    CreateHistogram(h2ggapPAM[i][j], shggapname + "PAM");
#ifdef GEGEAM
                    // h1ggapAM[i][j] = CreateHistogram<YTH1>(shggapname + "AM");
                    // h2ggapAM[i][j] = CreateHistogram<YTH2>(shggapname + "AM");
                    // h1ggapPM[i][j] = CreateHistogram<YTH1>(shggapname + "PM");
                    // h2ggapPM[i][j] = CreateHistogram<YTH2>(shggapname + "PM");
                    CreateHistogram(h1ggapAM[i][j], shggapname + "AM");
                    CreateHistogram(h2ggapAM[i][j], shggapname + "AM");
                    CreateHistogram(h1ggapPM[i][j], shggapname + "PM");
                    CreateHistogram(h2ggapPM[i][j], shggapname + "PM");
                    h1ggapPM[i][j]->Sumw2(1);
                    h2ggapPM[i][j]->Sumw2(1);

#endif
#endif

#ifdef CUBEMATRIX
                    // h3ggapPAM[i][j] = CreateHistogram<YTH3>(shggapname + "PAM");
                    CreateHistogram(h3ggapPAM[i][j], shggapname + "PAM");
#ifdef CUBEMATRIXAM
                    // h3ggapAM[i][j] = CreateHistogram<YTH3>(shggapname + "AM");
                    // h3ggapPM[i][j] = CreateHistogram<YTH3>(shggapname + "PM");
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
bool YMatrix::FillHistogram(const double &pde1, const double &pde2, const short &pa, const short &pp, const short &pt, const TString &psoption, const double &pde3)
{
    int ioption = psoption.Atoi();
    int ifapt1 = 0, ifat1 = 0, ifpt1 = 0, ifap1 = 0;
    int ifapt2 = 0, ifat2 = 0, ifpt2 = 0, ifap2 = 0;

    /*------------- PAM ------------*/

    // do apt
    if (((ioption / 1000) % 10) == 1)
    {

        if (h1ggaptPAM[pa][pp][pt] != nullptr)
        {
            ifapt1 = 1;
            h1ggaptPAM[pa][pp][pt]->Fill(pde1);
#ifdef GEGE
            h2ggaptPAM[pa][pp][pt]->Fill(pde1, pde2);
#endif
        }
        else
            ifapt1 = 0;

#ifdef CUBEMATRIX
        if (h3ggaptPAM[pa][pp][pt] != nullptr)
        {
            ifapt1 = 1;
            h3ggaptPAM[pa][pp][pt]->Fill(pde1, pde2, pde3);
        }
        else
            ifapt1 = 0;
#endif
    }

    // do at
    if (((ioption / 100) % 10) == 1)
    {
        if (h1ggatPAM[pa][pt] != nullptr)
        {
            ifat1 = 1;
            h1ggatPAM[pa][pt]->Fill(pde1);
#ifdef GEGE
            h2ggatPAM[pa][pt]->Fill(pde1, pde2);
#endif
        }
        else
            ifat1 = 0;

#ifdef CUBEMATRIX
        if (h3ggatPAM[pa][pt] != nullptr)
        {
            ifat1 = 1;
            h3ggatPAM[pa][pt]->Fill(pde1, pde2, pde3);
        }
        else
            ifat1 = 0;
#endif
    }

    // do pt
    if (((ioption / 10) % 10) == 1)
    {
        if (h1ggptPAM[pp][pt] != nullptr)
        {
            ifpt1 = 1;
            h1ggptPAM[pp][pt]->Fill(pde1);
#ifdef GEGE

            h2ggptPAM[pp][pt]->Fill(pde1, pde2);
#endif
        }
        else
            ifpt1 = 0;

#ifdef CUBEMATRIX
        if (h3ggptPAM[pp][pt] != nullptr)
        {
            ifpt1 = 1;
            h3ggptPAM[pp][pt]->Fill(pde1, pde2, pde3);
        }
        else
            ifpt1 = 0;
#endif
    }

    // do ap
    if (((ioption / 1) % 10) == 1)
    {
        if (h1ggapPAM[pa][pp] != nullptr)
        {
            ifap1 = 1;
            h1ggapPAM[pa][pp]->Fill(pde1);
#ifdef GEGE

            h2ggapPAM[pa][pp]->Fill(pde1, pde2);
#endif
        }
        else
            ifap1 = 0;

#ifdef CUBEMATRIX
        if (h3ggapPAM[pa][pp] != nullptr)
        {
            ifap1 = 1;
            h3ggapPAM[pa][pp]->Fill(pde1, pde2, pde3);
        }
        else
            ifap1 = 0;
#endif
    }

    /*------------- AM ------------*/

    // do apt
    if (((ioption / 1000) % 10) == 2)
    {
#ifdef GEGE
#ifdef GEGEAM
        if (h1ggaptAM[pa][pp][pt] != nullptr)
        {
            ifapt2 = 2;
            h1ggaptAM[pa][pp][pt]->Fill(pde1);
            h2ggaptAM[pa][pp][pt]->Fill(pde1, pde2);
        }
        else
            ifapt2 = 0;
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
        if (h3ggaptAM[pa][pp][pt] != nullptr)
        {
            ifapt2 = 2;
            h3ggaptAM[pa][pp][pt]->Fill(pde1, pde2, pde3);
        }
        else
            ifapt2 = 0;
#endif
#endif
    }

    // do at
    if (((ioption / 100) % 10) == 2)
    {
#ifdef GEGE
#ifdef GEGEAM
        if (h1ggatAM[pa][pt] != nullptr)
        {
            ifat2 = 2;
            h1ggatAM[pa][pt]->Fill(pde1);
            h2ggatAM[pa][pt]->Fill(pde1, pde2);
        }
        else
            ifat2 = 0;
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
        if (h3ggatAM[pa][pt] != nullptr)
        {
            ifat2 = 2;
            h3ggatAM[pa][pt]->Fill(pde1, pde2, pde3);
        }
        else
            ifat2 = 0;
#endif
#endif
    }

    // do pt
    if (((ioption / 10) % 10) == 2)
    {
#ifdef GEGE
#ifdef GEGEAM
        if (h1ggptAM[pp][pt] != nullptr)
        {
            ifpt2 = 2;
            h1ggptAM[pp][pt]->Fill(pde1);
            h2ggptAM[pp][pt]->Fill(pde1, pde2);
        }
        else
            ifpt2 = 0;
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
        if (h3ggptAM[pp][pt] != nullptr)
        {
            ifpt2 = 2;
            h3ggptAM[pp][pt]->Fill(pde1, pde2, pde3);
        }
        else
            ifpt2 = 0;
#endif
#endif
    }

    // do ap
    if (((ioption / 1) % 10) == 2)
    {
#ifdef GEGE
#ifdef GEGEAM
        if (h1ggapAM[pa][pp] != nullptr)
        {
            ifap2 = 2;
            h1ggapAM[pa][pp]->Fill(pde1);
            h2ggapAM[pa][pp]->Fill(pde1, pde2);
        }
        else
            ifap2 = 0;
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
        if (h3ggapAM[pa][pp] != nullptr)
        {
            ifap2 = 2;
            h3ggapAM[pa][pp]->Fill(pde1, pde2, pde3);
        }
        else
            ifap2 = 0;
#endif
#endif
    }

    return (ioption == (((ifapt1 + ifapt2) * 1000) + ((ifat1 + ifat2) * 100) + ((ifpt1 + ifpt2) * 10) + ((ifap1 + ifap2) * 1)));
}

void YMatrix::Process()
{
    Init();

    for (Long64_t jentry = 0; jentry < ltotalEntry; ++jentry)
    {
        fchain->GetEntry(jentry);

        if (jentry % 10000 == 0)
            printf("\rProcessing: %lld | % lld | %.2f%%", jentry, ltotalEntry, (double)jentry / ltotalEntry * 100);

        bfirstLoopHit = true;
        for (short cfirstlp = 0; cfirstlp < hit; cfirstlp++)
        {
            if (WhatDetector(id[cfirstlp]) != schargedDetectorFlag)
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
                if (WhatDetector(id[ccecondlp]) != schargedDetectorFlag)
                    continue;

                ltscSecond = GetTs(ts[ccecondlp], sr[ccecondlp]);
                if (!TcoinLR(ltscSecond, ltsmax, CSICSICOINWINDOWS))
                    continue;
                ltsmin = Min(Min(ltscFirst, ltscSecond), ltsmin);
                ltsmax = Max(Max(ltscFirst, ltscSecond), ltsmax);
                PtcAdd(alpha[ccecondlp], proton[ccecondlp], total[ccecondlp]);
            }

            /* fill gamma to histogram*/
            if (salphaHit > ALPHAPTCMAX || sprotonHit > PROTONPTCMAX || stotalHit > TOTALPTCMAX)
                continue;
            for (short gfristlp = 0; gfristlp < hit; gfristlp++)
            {
                if (WhatDetector(id[gfristlp]) != sGeFlag)
                    continue;

                ltsgFirst = GetTs(ts[gfristlp], sr[gfristlp]);
                if (!TcoinL(ltsgFirst, ltsmin, GECSICOINWINDOWSL))
                    continue;
                if (!TcoinR(ltsgFirst, ltsmax, GECSICOINWINDOWSR))
                    continue;

                for (short gsecondlp = 0; gsecondlp < hit; gsecondlp++)
                {
                    if (gfristlp == gsecondlp)
                        continue;

                    if (WhatDetector(id[gsecondlp]) != sGeFlag)
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
                        if (!FillHistogram(dpl[gfristlp], dpl[gsecondlp], salphaHit, sprotonHit, stotalHit, "1111"))
                            cerr << "can't fill histogram PAM, jentry = " << jentry << endl;
#endif

#ifdef GEGE
#ifdef GEGEAM
#ifndef CUBEMATRIXAM

                    if (bamgFirstgSecond)
                        if (!FillHistogram(dpl[gfristlp], dpl[gsecondlp], salphaHit, sprotonHit, stotalHit, "2222"))
                            cerr << "can't fill histogram AM, jentry = " << jentry << endl;

                    if (!FillPM())
                        cerr << "can't add matrix to AM" << endl;
#endif
#endif
#endif

#ifdef CUBEMATRIX
                    for (short gthirdlp = 0; gthirdlp < hit; gthirdlp++)
                    {
                        if (gsecondlp == gthirdlp || gfristlp == gthirdlp) // had judge gfristlp == gsecondlp in second loop of gamma
                            continue;

                        if (WhatDetector(ts[gthirdlp]) != sGeFlag)
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
                            if (!FillHistogram(dpl[gfristlp], dpl[gsecondlp], salphaHit, sprotonHit, stotalHit, "1111", dpl[gthirdlp]))
                                cerr << "can't fill histogram PAM, jentry = " << jentry << endl;
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
                            if (!FillHistogram(dpl[gfristlp], dpl[gsecondlp], salphaHit, sprotonHit, stotalHit, "2222", dpl[gthirdlp]))
                                cerr << "can't fill histogram AM, jentry = " << jentry << endl;
                        if (!FillPM())
                            cerr << "can't add matrix to AM" << endl;
#endif
                    }
#endif
                }
            }
        }
    }
    cout << endl;
    if (StoreFile())
        cout << "Finish!" << endl;
    else
        cout << "failed" << endl;
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
    int icount = 0;
    for (short i = 0; i < (ALPHAPTCMAX + 1); i++)
        for (short j = 0; j < (PROTONPTCMAX + 1); j++)
            for (short k = 0; k < (TOTALPTCMAX + 1); k++)
            {
                icount++;
#ifdef GEGE
#ifdef GEGEAM
                h1ggaptPM[i][j][k]->Add(h1ggaptPAM[i][j][k], h1ggaptAM[i][j][k], 1, dweight);
                h1ggatPM[i][k]->Add(h1ggatPAM[i][k], h1ggatAM[i][k], 1, dweight);
                h1ggptPM[j][k]->Add(h1ggptPAM[j][k], h1ggptAM[j][k], 1, dweight);
                h1ggapPM[i][j]->Add(h1ggapPAM[i][j], h1ggapAM[i][j], 1, dweight);

                h2ggaptPM[i][j][k]->Add(h2ggaptPAM[i][j][k], h2ggaptAM[i][j][k], 1, dweight);
                h2ggatPM[i][k]->Add(h2ggatPAM[i][k], h2ggatAM[i][k], 1, dweight);
                h2ggptPM[j][k]->Add(h2ggptPAM[j][k], h2ggptAM[j][k], 1, dweight);
                h2ggapPM[i][j]->Add(h2ggapPAM[i][j], h2ggapAM[i][j], 1, dweight);
#endif
#endif

#ifdef CUBEMATRIX
#ifdef CUBEMATRIXAM
                h3ggaptPM[i][j][k]->Add(h3ggaptPAM[i][j][k], h3ggaptAM[i][j][k], 1, dweight);
                h3ggatPM[i][k]->Add(h3ggatPAM[i][k], h3ggatAM[i][k], 1, dweight);
                h3ggptPM[j][k]->Add(h3ggptPAM[j][k], h3ggptAM[j][k], 1, dweight);
                h3ggapPM[i][j]->Add(h3ggapPAM[i][j], h3ggapAM[i][j], 1, dweight);
#endif
#endif
            }

    return (icount == ((ALPHAPTCMAX + 1) * (PROTONPTCMAX + 1) * (TOTALPTCMAX + 1)));
}

bool YMatrix::StoreFile()
{
    if (fopf == nullptr)
        return false;
    for (short i = 0; i < (ALPHAPTCMAX + 1); i++)
        for (short j = 0; j < (PROTONPTCMAX + 1); j++)
            for (short k = 0; k < (TOTALPTCMAX + 1); k++)
            {
                StoreHistogram(h1ggaptPAM[i][j][k], "PAM/TH1/AlphaProtonTotal");
                StoreHistogram(h1ggatPAM[i][k], "PAM/TH1/AlphaTotal");
                StoreHistogram(h1ggptPAM[j][k], "PAM/TH1/ProtonTotal");
                StoreHistogram(h1ggapPAM[i][j], "PAM/TH1/AlphaProton");
#ifdef GEGE
                StoreHistogram(h2ggaptPAM[i][j][k], "PAM/TH2/AlphaProtonTotal");
                StoreHistogram(h2ggatPAM[i][k], "PAM/TH2/AlphaTotal");
                StoreHistogram(h2ggptPAM[j][k], "PAM/TH2/ProtonTotal");
                StoreHistogram(h2ggapPAM[i][j], "PAM/TH2/AlphaProton");
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
#ifdef GEGE
#ifdef GEGEAM
                StoreHistogram(h1ggaptAM[i][j][k], "AM/TH1/AlphaProtonTotal");
                StoreHistogram(h1ggatAM[i][k], "AM/TH1/AlphaTotal");
                StoreHistogram(h1ggptAM[j][k], "AM/TH1/ProtonTotal");
                StoreHistogram(h1ggapAM[i][j], "AM/TH1/AlphaProton");

                StoreHistogram(h2ggaptAM[i][j][k], "AM/TH2/AlphaProtonTotal");
                StoreHistogram(h2ggatAM[i][k], "AM/TH2/AlphaTotal");
                StoreHistogram(h2ggptAM[j][k], "AM/TH2/ProtonTotal");
                StoreHistogram(h2ggapAM[i][j], "AM/TH2/AlphaProton");

                StoreHistogram(h1ggaptPM[i][j][k], "PM/TH1/AlphaProtonTotal");
                StoreHistogram(h1ggatPM[i][k], "PM/TH1/AlphaTotal");
                StoreHistogram(h1ggptPM[j][k], "PM/TH1/ProtonTotal");
                StoreHistogram(h1ggapPM[i][j], "PM/TH1/AlphaProton");

                StoreHistogram(h2ggaptPM[i][j][k], "PM/TH2/AlphaProtonTotal");
                StoreHistogram(h2ggatPM[i][k], "PM/TH2/AlphaTotal");
                StoreHistogram(h2ggptPM[j][k], "PM/TH2/ProtonTotal");
                StoreHistogram(h2ggapPM[i][j], "PM/TH2/AlphaProton");
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
    bamgFirstgSecond = false;
    bpamgFirstgThird = false;
    bamgFirstgThird = false;
    bpamgSecondgThird = false;
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
 * @return -  charged particle detector -=> schargedDetectorFlag
 * @return -  germanium detector (HPGe and CLOVER) -=> sGeFlag
 * @return -  LaBr_3 -=> sLaBrFlag
 */
inline short YMatrix::WhatDetector(const short &psid)
{
    return (psid > 0 ? (psid < 37 ? sGeFlag : sLaBrFlag) : schargedDetectorFlag);
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
