// Author: Kevin Vockerodt (k.j.vockerodt@qmul.co.uk), based strongly on 
// CONVENIENT code by Colin Weber
// Date: 11 January  2024
// Modified: 12 April 2024
// Purpose: To read in a .txt file for a NEUT flux and convert it to a 
// histogram in a .root file.

// Command: root -q "make_flux_hist_root.C(
// 		\"fluxfile\", \"outfile\", \"outhisto\")"

// Parameters
// 	fluxfile
// 		The .txt file containing a neutrino flux in the format 
// 		left edge[tab] value\n, where each new line is a new bin.
// 	outfile
// 		The name of the output file. Must end in .root.
// 	outhisto
// 		The name of the output histogram. This is typically set to be the 
// 		neutrino flavor, i.e. numu, nuebar, etc.

// Outputs
// fFlux : TFile*
// 		A file named by "outfile" containing a 1D histogram of the flux from 
// 		"fluxfile" named "outhisto".

// Includes
// ROOT includes
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h" 
#include "TH1.h"
#include "TPRegexp.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

// C++ includes
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


void make_flux_hist_root(TString fluxfile, TString outfile, std::string outhisto) {
  // Read from flux text file, make histogram and save

  TFile* fFlux = new TFile(outfile, "RECREATE");

  std::fstream fFlux_data;
  fFlux_data.open(fluxfile, std::ios::in);
  if(fFlux_data.is_open()){
    std::string line;

    std::vector<float> vecEnergy;
    std::vector<float> vecFlux;    


    while(getline(fFlux_data, line)){
      TStringToken splitstring(line, "\t");

      splitstring.NextToken();
      std::string energy = (std::string)splitstring;
      vecEnergy.push_back(std::stod(energy));

      splitstring.NextToken();
      std::string flux = (std::string)splitstring;
      vecFlux.push_back(std::stod(flux));
    }

    TCanvas* cFlux = new TCanvas();

    int nbins = vecEnergy.size() - 1;
    float* arrEnergy = &vecEnergy[0];
 
    TH1D* hFlux = new TH1D("FlatTree_FLUX", "FlatTree_FLUX", nbins, arrEnergy);

    for(int bin = 1; bin <= nbins; bin++){
      hFlux->SetBinContent(bin, vecFlux[bin-1]);
    }


    hFlux->Draw("hist");
    hFlux->Write(outhisto.c_str());
 
    cFlux->Close();
  }
  else std::cout << "Cannot open flux file " << fluxfile << '\n';

  fFlux->Close();
  fFlux_data.close();
}

