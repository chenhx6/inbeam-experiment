/***
 * @Date: 2023-11-14 18:39:02
 * @LastEditTime: 2023-11-15 00:24:32
 */
#define TOTALMIN 0
#define TOTALMAX 7
#define ALPHAMIN 0
#define ALPHAMAX 4
#define PROTONMIN 0
#define PROTONMAX 6

bool m4b(const TString &hname, const TString &pathm4b);

void root2m4b()
{
	bool apt[ALPHAMAX + 1][PROTONMAX + 1][TOTALMAX + 1];
	bool at[ALPHAMAX + 1][TOTALMAX + 1];
	bool pt[PROTONMAX + 1][TOTALMAX + 1];
	bool ap[ALPHAMAX + 1][PROTONMAX + 1];
	for (int alphai = ALPHAMIN; alphai <= ALPHAMAX; alphai++)
		for (int protoni = PROTONMIN; protoni <= PROTONMAX; protoni++)
			for (int totali = TOTALMIN; totali <= TOTALMAX; totali++)
			{
				apt[alphai][protoni][totali] = true;
				at[alphai][totali] = true;
				pt[protoni][totali] = true;
				ap[alphai][protoni] = true;
			}

	TString m4bfilepath = "/home/share_data/40Ca106Cd_guos/data2/m4b/PAM/";
	TString dir = "PAM"; // root dir

	TString rootfilepath = "";
	TString rootfilename = "matrix_2PAM_2PM_1051_1106_CSIW40_GECSIWminus50_150_GEW70_AMWminus150_minus100_100_150.root";
	TString hname;

	TFile *ipf = new TFile((rootfilepath + rootfilename).Data(), "READ");
	if (!ipf->IsOpen())
	{
		cerr << "Can't open file" << rootfilepath << rootfilename << endl;
		exit(-1);
	}

	for (int alphai = ALPHAMIN; alphai <= ALPHAMAX; alphai++)
		for (int protoni = PROTONMIN; protoni <= PROTONMAX; protoni++)
			for (int totali = TOTALMIN; totali <= TOTALMAX; totali++)
			{
				if ((alphai + protoni) > totali)
				{
					continue;
				}

				if (apt[alphai][protoni][totali])
				{
					if (ipf->cd(dir + "/TH2/AlphaProtonTotal"))
						cout << "ok" << endl;
					else
						cerr << "error cd " << endl;

					hname = TString::Format("h2alpha%d_proton%d_total%d", alphai, protoni, totali);
					if (m4b(hname + dir, m4bfilepath))
						cout << "Finish " << hname << endl;
					else
						cerr << "Fail " << hname << endl;

					apt[alphai][protoni][totali] = false;
				}

				if (at[alphai][totali])
				{
					ipf->cd(dir + "/TH2/AlphaTotal");
					hname = TString::Format("h2alpha%d_total%d", alphai, totali);
					if (m4b(hname + dir, m4bfilepath))
						cout << "Finish " << hname << endl;
					else
						cerr << "Fail " << hname << endl;

					at[alphai][totali] = false;
				}

				if (pt[protoni][totali])
				{
					ipf->cd(dir + "/TH2/ProtonTotal");
					hname = TString::Format("h2proton%d_total%d", protoni, totali);
					if (m4b(hname + dir, m4bfilepath))
						cout << "Finish " << hname << endl;
					else
						cerr << "Fail " << hname << endl;

					pt[protoni][totali] = false;
				}

				if (ap[alphai][protoni])
				{
					ipf->cd(dir + "/TH2/AlphaProton");
					hname = TString::Format("h2alpha%d_proton%d", alphai, protoni);
					if (m4b(hname + dir, m4bfilepath))
						cout << "Finish " << hname << endl;
					else
						cerr << "Fail " << hname << endl;

					ap[alphai][protoni] = false;
				}
			}

	ipf->Close();
	cout << "Finish All" << endl;
}

// main function
bool m4b(const TString &hname, const TString &pathm4b)
{
	TH2F *h2 = nullptr;
	h2 = (TH2F *)gROOT->FindObject(hname.Data());
	if (h2 == nullptr)
	{
		cerr << "Can't find histogram: " << hname << endl;
		return false;
	}
	TString m4bfilename = pathm4b + hname + ".m4b";

	FILE *m4bfile;
	m4bfile = fopen(m4bfilename.Data(), "wb"); // Truncate write binary
	if (m4bfile == NULL)
	{
		cerr << "Can't open file:" << m4bfilename << endl;
		return false;
	}

	int binx = h2->GetNbinsX();
	int bincontent;
	for (int bini = 1; bini <= binx; bini++)
		for (int binj = 1; binj <= binx; binj++)
		{
			bincontent = (int)h2->GetBinContent(bini, binj);
			fwrite(&bincontent, sizeof(int), 1, m4bfile);
		}

	if (fclose(m4bfile)) // YES -=> 0; No -=> -1
		cerr << "Can't close file: " << m4bfilename << endl;

	return true;
}
