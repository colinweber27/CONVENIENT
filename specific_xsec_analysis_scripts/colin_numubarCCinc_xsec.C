// Author: Colin Weber (webe1077@umn.edu)
// Date: 8 September 2023
// Purpose: To compare cross sections from data and different generators
//
// Command: cafe xsec_analysis_macro.C

// Includes
// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THnSparse.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TString.h"

// Include common analysis variables
#include "numubarCCinc_vars.h"

// C++ includes
#include <vector>
#include <iostream>
#include <cmath>
#include <random>

// Include ROOTuple
//#include "/nova/app/users/kjvocker/generator_predictions/RootTuple/src/FC_namemangling.h"
//#include "/nova/app/users/kjvocker/generator_predictions/RootTuple/src/RootTuple.h"
//#include "/nova/app/users/kjvocker/generator_predictions/RootTuple/src/interface.h"

// Simulation / Preliminary include
#include "Utilities/rootlogon.C"


bool inSelection(double Tmu, double cosThetamu){
  return((Tmu >= 0.5) && (Tmu <= 2.5) && (cosThetamu >= 0.5));
//  return true;
}


void numubarCCinc_xsec(TString dir = "generator_output")
{
  // Read in files from the tunes we want to compare
  // Initialize pointers-to-TFile to point to the outputs from Convenient
  TFile* genieN18_file  = TFile::Open(dir + "/GENIE:N18_10j_02_11a.20231123005520.2.gntp.ghep.root.RHC.convenient_output.root");
//  TFile* genieAR23_file = TFile::Open(dir + "/GENIE:AR23_20i_00_000.20231123005520.2.gntp.ghep.root.RHC.convenient_output.root";
  TFile* genieG18_file  = TFile::Open(dir + "/GENIE:G18_10a_02_11b.20231123005520.2.gntp.ghep.root.RHC.convenient_output.root");

  TFile* nuwro_file = TFile::Open(dir + "/NuWro:20230821params.txt.20231124141438.2.eventsout.root.RHC.convenient_output.root");

  // Read in fake data
  TFile* fake_data_file = TFile::Open(dir + "/GENIE:AR23_20i_00_000.20231123005520.2.gntp.ghep.root.RHC.convenient_output.root");


  // Modify this section to organize each data file into an array. The array will be looped over, using each file to calculate a cross section.
  // Always set the data file last.
  TFile* file_list[] = {genieN18_file, genieG18_file, nuwro_file, fake_data_file};
  TString file_alias[] = {"GENIE_N18_10j_02_11a", "GENIE_G18_10a_02_11b", "NuWro_20230821params", "FD (GENIE_AR23_20i_00_000)"}; 

  int n_files = sizeof(file_list) / sizeof(file_list[0]);

  // For each analysis variable, initialise a vector to push the generated xsec histograms to
  // Since all ROOT histogram classes are derived from TH1, we initialize the vector to hold TH1*.
  // We also set the Add Directory option to be false so that each new pointer points to a unique histogram.
  TH1::AddDirectory(kFALSE);

  TH1* vecHist_Tmu[n_files];
  TH1* vecHist_CosThetaMu[n_files];
  TH1* vecHist_Eavail[n_files];

  TH1* vecHist_Enu[n_files];
  TH1* vecHist_Q2[n_files];

  // Declare constants for later use, e.g. muon phase space cuts.
  double Tmu_low = 0.5;		// GeV
  double Tmu_high = 2.5;	// GeV
  double CosThetaMu_low = 0.5;

  // Set binning
  float Tmu_edges[]        = {0.0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5};
  float CosThetaMu_edges[] = {0.5, 0.74, 0.80, 0.85, 0.88, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0};
  float Eavail_edges[]     = {0.0, 0.10, 0.30, 0.60, 1.0, 2.0, 3.0};

  float Enu_edges[]        = {0.0, 0.50, 0.75, 1.0, 1.25, 1.50, 1.75, 2.0, 2.50, 3.0, 4.0};
  float Q2_edges[]         = {0.0, 0.10, 0.20, 0.30, 0.40, 0.60, 1.10, 2.8};

  int nTmu_bins        = sizeof(Tmu_edges)/sizeof(Tmu_edges[0]) - 1;
  int nCosThetaMu_bins = sizeof(CosThetaMu_edges)/sizeof(CosThetaMu_edges[0]) - 1;
  int nEavail_bins     = sizeof(Eavail_edges)/sizeof(Eavail_edges[0]) - 1;

  int nEnu_bins        = sizeof(Enu_edges)/sizeof(Enu_edges[0]) - 1;
  int nQ2_bins         = sizeof(Q2_edges)/sizeof(Q2_edges[0]) - 1;


  // Initialise list of colours to use for plotting histograms
  std::vector<int> myColours = {kRed, kCyan+1, kBlue+1, kBlack, kOrange+2, kGreen+1, kMagenta+1, kGray};

  // Loop over the data files.
  for(int file_index = 0; file_index < n_files; file_index++){
    TFile* data_file = file_list[file_index];

    // Declare a TTreeReader to help read the tree
    TTreeReader reader("generator_data", data_file);

    // Declare TTreeReader arrays and values to hold the desired data from the trees. Format is (reader, leaf). 
    // Grab info on CC/NC and neutrino type
    TTreeReaderValue<bool> flagCC(reader, "flagCC");
    TTreeReaderValue<int> nu_in_flavour(reader, "NuIn.Flavor");
    TTreeReaderValue<float> nu_in_e(reader, "NuIn.E_in");

    // Grab leaves for final state particles that make up our topology of interest. It is from these values that we can extract final state particle kinematics.
    TTreeReaderArray<std::vector<float>> electrons(reader, "FS_Electrons");
    TTreeReaderArray<std::vector<float>> gammas(reader, "FS_Gammas");
    TTreeReaderArray<std::vector<float>> muons(reader, "FS_Muons");
    TTreeReaderArray<std::vector<float>> neutrons(reader, "FS_Neutrons");
    TTreeReaderArray<std::vector<float>> nus(reader, "FS_Nus");
    TTreeReaderArray<std::vector<float>> protons(reader, "FS_Protons");
    TTreeReaderArray<std::vector<float>> pips(reader, "FS_PiPs");
    TTreeReaderArray<std::vector<float>> pims(reader, "FS_PiMs");
    TTreeReaderArray<std::vector<float>> pi0s(reader, "FS_Pi0s");
    TTreeReaderArray<std::vector<float>> others(reader, "FS_Others");

    TTreeReaderValue<int> nElectrons(reader, "n_FS_Electrons");
    TTreeReaderValue<int> nGammas(reader, "n_FS_Gammas");
    TTreeReaderValue<int> nMuons(reader, "n_FS_Muons");
    TTreeReaderValue<int> nNeutrons(reader, "n_FS_Neutrons");
    TTreeReaderValue<int> nNus(reader, "n_FS_Nus");
    TTreeReaderValue<int> nProtons(reader, "n_FS_Protons");
    TTreeReaderValue<int> nPips(reader, "n_FS_PiPs");
    TTreeReaderValue<int> nPims(reader, "n_FS_PiMs");
    TTreeReaderValue<int> nPi0s(reader, "n_FS_Pi0s");
    TTreeReaderValue<int> nOthers(reader, "n_FS_Others");

    // Grab leaves holding the NUISANCE normalization factor
    TTreeReaderValue<double> fscalefactor(reader, "fScaleFactor");


    // Load in the flux information
    TH1D* flux = (TH1D*) data_file->Get("FlatTree_FLUX");

    const int nFlux_bins = flux->GetNbinsX();
    double flux_min = flux->GetXaxis()->GetBinLowEdge(1);
    double flux_max = flux->GetXaxis()->GetBinUpEdge(nFlux_bins);

    float flux_array[nFlux_bins+1];
    for(int i = 0; i < nFlux_bins+1; i++){
      flux_array[i] = flux->GetBinLowEdge(i+1);
    }


    // Enu is a flux-averaged cross section as a function of energy (sigma E), not a differential cross section.
    // Q2 is a single-differential, flux-integrated cross section.
    // Tmu, cos theta_mu and Eavail is a flux-integrated triple-differential cross section.
      
    // Initialise histograms
    // Format is (name, title, n_bins, lower limit, upper limit)
    TH1D* hTmu        = new TH1D("hTmu", "Leading Muon 3-momentum", nTmu_bins, Tmu_edges);
    TH1D* hCosThetaMu = new TH1D("hCosThetaMu", "cos #theta_{#mu}", nCosThetaMu_bins, CosThetaMu_edges);
    TH1D* hEavail     = new TH1D("hEavail", "Eavail", nEavail_bins, Eavail_edges);

//  TH2D* hEnu        = new TH2D("hEnu", "Incident Neutrino Energy", nEnu_bins, Enu_edges, nFlux_bins, flux_array);
    TH1D* hEnu        = new TH1D("hEnu", "Incident Neutrino Energy", nEnu_bins, Enu_edges);
    TH1D* hQ2         = new TH1D("hQ2", "4-momentum Transfer Squared", nQ2_bins, Q2_edges);
	
	
    // Loop over events and fill the histograms
    bool first_entry = true;
    double scale_factor;
    while (reader.Next()){
      std::cout << "Event " << reader.GetCurrentEntry() << " (file " << file_index + 1 << "/" << n_files << ")" << '\n';

      // Select numubarCC events only
      if((*nu_in_flavour != -14) || (! *flagCC) || (*nMuons < 1)) continue;
      else{
        // Scale factors are the same for each event, so we only need to set the scale factor once. They depend on the 
        // total inclusive cross section, which is dependent on the generator and neutrino flavour. 
	if (first_entry){
	  scale_factor = *fscalefactor;
	  first_entry = false;
	}

	// Convert the leading muon's 4-momentum into a vector.  By default, the vectors of particles that Convenient saves are arranged in descending energy.
	std::vector<double> mu = {muons[0][0], muons[0][1], muons[0][2], muons[0][3]};

	// Fill histograms for the variables we're interested in
	double Tmu = calcMomentum(mu);
	double CosThetaMu = calcCosTheta(mu);

	if(inSelection(Tmu, CosThetaMu)){
	  hTmu->Fill(Tmu);
	  hCosThetaMu->Fill(CosThetaMu);

	  double Q2 = calcQ2(*nu_in_e, mu);
	  hQ2->Fill(Q2);

	  hEnu->Fill(*nu_in_e, *nu_in_e);

	  // Construct Eavail variable from FS (electrons, gammas, protons, pions and all others, except for muons, neutrons and neutrinos)
	  double Eavail = 0.0;;

	  for(int i = 0; i < *nElectrons; i++){
	    std::vector<double> particle = {electrons[0][0], electrons[0][1], electrons[0][2], electrons[0][3]};
	    Eavail += addtoEavail(particle, "electron");
	  }

          for(int i = 0; i < *nGammas; i++){
            std::vector<double> particle = {gammas[0][0], gammas[0][1], gammas[0][2], gammas[0][3]};
            Eavail += addtoEavail(particle, "gamma");
          }

          for(int i = 0; i < *nProtons; i++){
            std::vector<double> particle = {protons[0][0], protons[0][1], protons[0][2], protons[0][3]};
            Eavail += addtoEavail(particle, "proton");
          }

          for(int i = 0; i < *nPips; i++){
            std::vector<double> particle = {pips[0][0], pips[0][1], pips[0][2], pips[0][3]};
            Eavail += addtoEavail(particle, "pip");
          }

          for(int i = 0; i < *nPims; i++){
            std::vector<double> particle = {pims[0][0], pims[0][1], pims[0][2], pims[0][3]};
            Eavail += addtoEavail(particle, "pim");
          }

          for(int i = 0; i < *nPi0s; i++){
            std::vector<double> particle = {pi0s[0][0], pi0s[0][1], pi0s[0][2], pi0s[0][3]};
            Eavail += addtoEavail(particle, "pi0");
          }

          for(int i = 0; i < *nOthers; i++){
            std::vector<double> particle = {others[0][0], others[0][1], others[0][2], others[0][3]};
            Eavail += addtoEavail(particle, "other");
          }

          hEavail->Fill(Eavail);

	}
      }
    }

    TH1D* hFlux_rebinned = new TH1D("flux_rebinned", "Flux with Enu binning", nEnu_bins, Enu_edges);

    for(int i = 1; i < flux->GetNbinsX(); i++){
      hFlux_rebinned->Fill(flux->GetBinLowEdge(i), flux->GetBinContent(i));
    }

    hFlux_rebinned->Scale(1.0/flux->Integral());

    TCanvas* c = new TCanvas();
    hFlux_rebinned->Draw("hist");
    c->Print("xsec/flux_rebinned.png");
    c->Close();

    hEnu->Divide(hFlux_rebinned);
		
    // Multiply histograms by their scale factors to get cross sections in units of cm^2 / nucleon
    hTmu->Scale(scale_factor);
    hCosThetaMu->Scale(scale_factor);
    hQ2->Scale(scale_factor);
    hEnu->Scale(scale_factor);
    hEavail->Scale(scale_factor);

//    TH1D* hEnu_flat = (TH1D*)hEnu->ProjectionX();

    // Additionally, for Enu we normalise by the total flux, and divide the histograms' energy axes by the flux, bin-by-bin.  We don't divide by the bin width.
    // This is because Enu is a flux-averaged cross section in energy (sigma E), rather than a flux-integrated differential cross section.
    for(int i = 1; i <= hEnu->GetNbinsX(); i++){
       hEnu->SetBinContent(i, hEnu->GetBinContent(i) / hEnu->GetBinWidth(i));
       hEnu->SetBinContent(i, hEnu->GetBinContent(i) / hEnu->GetXaxis()->GetBinCenter(i));
    }

    // For flux-integrations differential cross sections, divide histograms by their bin widths
    for(int i = 1; i <= hTmu->GetNbinsX(); i++){
      hTmu->SetBinContent(i, hTmu->GetBinContent(i) / hTmu->GetBinWidth(i));
    }

    for(int i = 1; i <= hCosThetaMu->GetNbinsX(); i++) {
      hCosThetaMu->SetBinContent(i, hCosThetaMu->GetBinContent(i) / hCosThetaMu->GetBinWidth(i));
    }

    for(int i = 1; i <= hQ2->GetNbinsX(); i++){
      hQ2->SetBinContent(i, hQ2->GetBinContent(i) / hQ2->GetBinWidth(i));
    }

    for(int i = 1; i <= hEavail->GetNbinsX(); i++){
      hEavail->SetBinContent(i, hEavail->GetBinContent(i) / hEavail->GetBinWidth(i));
    }

    // Append histogram to the vector of histograms (i.e. one histogram for each generator/tune used)
    vecHist_Tmu[file_index] = hTmu;
    vecHist_CosThetaMu[file_index] = hCosThetaMu;
    vecHist_Enu[file_index] = hEnu;
    vecHist_Q2[file_index] = hQ2;
    vecHist_Eavail[file_index] = hEavail;
  }

		// Triple-differential, flux-averaged cross sections //

		/*
		// Initialize histograms
		// Format is (name, title, x n_bins, x lower limit, x upper limit, 
		// y n_bins, y lower limit, y upper limit)
		TH3D* h3 = new TH3D("h3", "Leading Muon cos, 3-momentum, nu E \
			spectrum", \
			fake_data_n_bins_cos, fake_data_cos_low, fake_data_cos_high, \
			fake_data_n_bins_p, fake_data_p_low, fake_data_p_high, \
			fake_data_n_bins_E, fake_data_E_low, fake_data_E_high);
	
		// Loop over events and fill histograms
		bool firstEntry = true;
		double scale_factor;
		while(reader.Next()) {
			// Select neutrino type
			if(*nu_in_flavour != 14) continue;
			else {
				// Scale factors are the same for each event, so we only 
				// need to set the scale factor once. They depend on the 
				// total inclusive cross section, which is dependent on the 
				// generator and neutrino flavour. 
				if (firstEntry) {
					scale_factor = *fscalefactor;
					firstEntry = false;
				}	
				// Select NC/CC events
				if(! *flagCC) continue;
				else {
					// Here's where differential xsec variables are defined 
					// and used to make cuts. Kinematic variables are stored 
					// in each event as vectors of vectors, where each 
					// interior vector is a 4-momentum (E, px, py, pz), and 
					// each particle in the event contributes one of these 
					// to the external vector. The external vector is 
					// ordered by decreasing energy. So, to access the 
					// second most-energetic particle's py, call 
					// particle[1][2]. All other differential xsec variables 
					// can be constructed with the information in the files.
					// In this case:
					// If there is at least one muon, and the most energetic 
					// one is above the threshold, then get its 3-momentum 
					// and cos wrt the initial neutrino direction (set to be 
					// (0, 0, 1)). Get the neutrino energy as well. Finally, 
					// fill a histogram with these variables.
					if(*n_muons > 0 && muons[0][0] > muon_threshold_E) {
						float muon_px = muons[0][1];
						float muon_py = muons[0][2];
						float muon_pz = muons[0][3];
						float muon_p = sqrt(pow(muon_px, 2) + \
							pow(muon_py, 2) + pow(muon_pz, 2));

						float muon_cos = muon_pz / muon_p;

						float nu_E = *nu_in_e;

					h3->Fill(muon_cos, muon_p, nu_E);
					}
				}
			}
		}
		
		// Multiply histograms by their scale factors to get cross sections 
		// in units of cm^2 / nucleon
		h3->Scale(scale_factor);

		// Divide histograms by their bin widths to get differential cross 
		// sections
		for(int i = 1; i <= h3->GetXaxis()->GetNbins(); i++) {
			for(int j = 1; j <= h3->GetYaxis()->GetNbins(); j++) {
				for(int k = 1; k <= h3->GetZaxis()->GetNbins(); k++) {
					h3->SetBinContent(i, j, k, h3->GetBinContent(i, j, k) / \
						h3->GetXaxis()->GetBinWidth(i) / \
						h3->GetYaxis()->GetBinWidth(j) / \
						h3->GetZaxis()->GetBinWidth(k));
				}
			}
		}

		// Append the histogram to the vector
		vecHist_Tmu[file_index] = h3;
		*/
		
//########################################################################//
		// Flux-integrated cross sections //
//########################################################################//
		// To do flux-integrated cross sections, the general procedure is:
			// 1. For an n-differential cross section, create an (n+1)-dim. 
			// spectrum of the differential variables and the incident 
			// neutrino energy.
			// 2. Multiply by the NUISANCE scale factor to normalize 
			// generators to each other.
			// 3. Grab the flux histogram used to create the NUISANCE file, 
			// normalize it, and then divide the neutrino energy axis by the 
			// normalized flux histogram. This removes the integrated flux 
			// factor in the NUISANCE scale factor, and ensures that we are 
			// consistent in treating each energy bin separately before 
			// integrating out the flux altogether.
			// 4. Integrate over the incident neutrino energies to get an 
			// n-dim. spectrum. If we stop here, we have a flux-integrated 
			// cross section as a function of the differential variables.
			// 5. Divide by the bin widths of each variable to get a 
			// flux-integrated differential cross section.

		// Single differential, flux-integrated cross section //

		
		// Access the flux and grab the flux binning for constructing the 
		// flux axis of the histogram. This is needed for a flux-integrated 
		// cross section calculation. 
/*		TH1D* flux = (TH1D*) data_file->Get("FlatTree_FLUX");
		int n_flux_bins = flux->GetNbinsX();
		double flux_bins_low = flux->GetBinLowEdge(1);
		double flux_bins_high = flux->GetBinLowEdge(n_flux_bins) + \
			flux->GetBinWidth(n_flux_bins);	

		// Initialize histograms
		// Format is (name, title, x n_bins, x lower limit, x upper limit, 
		// y n_bins, y lower limit, y upper limit) for constant binning.
		// Format is (name, title, x n_bins, array of bin edges, y n_bins, 
		// array of bin edges) for variable binning.
		TH2D* h2 = new TH2D("h2", "Leading Muon 3-momentum spectrum", \
			fake_data_n_bins_p, fake_data_p_low, fake_data_p_high, \
			n_flux_bins, flux_bins_low, flux_bins_high);

		// Loop over events and fill histograms
		bool firstEntry = true;
		double scale_factor;
		while(reader.Next()) {
			// Select neutrino type
			if(*nu_in_flavour != 14) continue;
			else {
				// Scale factors are the same for each event, so we only 
				// need to set the scale factor once. They depend on the 
				// total inclusive cross section, which is dependent on the 
				// generator and neutrino flavour. 
				if (firstEntry) {
					scale_factor = *fscalefactor;
					firstEntry = false;
				}	
				// Select NC/CC events
				if(! *flagCC) continue;
				else {
					// Here's where differential xsec variables are defined 
					// and used to make cuts. Kinematic variables are stored 
					// in each event as vectors of vectors, where each 
					// interior vector is a 4-momentum (E, px, py, pz), and 
					// each particle in the event contributes one of these 
					// to the external vector. The external vector is 
					// ordered by decreasing energy. So, to access the 
					// second most-energetic particle's py, call 
					// particle[1][2]. All other differential xsec variables 
					// can be constructed with the information in the files.
					// In this case:
					// If there is at least one muon, and the most energetic 
					// one is above the threshold, then get its 3-momentum. 
					// Get the neutrino energy as well. Finally, fill a 
					// histogram with these variables
					if(*n_muons > 0 && muons[0][0] > muon_threshold_E) {
						float muon_px = muons[0][1];
						float muon_py = muons[0][2];
						float muon_pz = muons[0][3];
						float muon_p = sqrt(pow(muon_px, 2) + \
							pow(muon_py, 2) + pow(muon_pz, 2));

						float nu_E = *nu_in_e;
					
						h2->Fill(muon_p, nu_E);
					}
				}
			}
		}
		
		// Multiply histograms by their scale factors to get cross sections 
		// in units of cm^2 / nucleon
		h2->Scale(scale_factor);
	
		// Normalize fluxes, and divide the histograms' energy axes by the 
		// flux
		flux->Scale(1.0 / flux->Integral());

		int n_xbins = h2->GetXaxis()->GetNbins();
		for(int i = 1; i <= n_xbins; i++) {
			for(int j = 1; j <= n_flux_bins; j++) {
				h2->SetBinContent(i, j, h2->GetBinContent(i, j) / \
					flux->GetBinContent(j));
			}
		}
	
		// Integrate out the energy by projecting onto the x axis, and 
		// assign to the histogram vector.
		vecHist_Tmu[file_index] = h2->ProjectionX("h1");

		// Divide histograms by their bin widths to get differential cross 
		// sections
		for(int i = 1; i <= vecHist_Tmu[file_index]->GetNbinsX(); i++) {
			vecHist_Tmu[file_index]->SetBinContent(i, \
				vecHist_Tmu[file_index]->GetBinContent(i) / \
				vecHist_Tmu[file_index]->GetBinWidth(i));
		}
*/		

		// Triple-differential, flux-integrated cross sections //

		/*
		// Access the flux and grab the flux binning for constructing the 
		// flux axis of the histogram. This is needed for a flux-integrated 
		// cross section calculation. 
		TH1D* flux = (TH1D*) data_file->Get("FlatTree_FLUX");
		int n_flux_bins = flux->GetNbinsX();
		double flux_bins_low = flux->GetBinLowEdge(1);
		double flux_bins_high = flux->GetBinLowEdge(n_flux_bins) + \
			flux->GetBinWidth(n_flux_bins);	

		// Initialize histograms
		// With 4D histograms (recall, for an n-dim flux-integrated cross 
		// section, we need an n+1 dim histogram), format is
		// THnSparseD* (name, title, n_dim, array of n_bins, array of mins, 
		// array of maxs)
		int n_bins[4] = {fake_data_n_bins_p, fake_data_n_bins_cos, \
			fake_data_n_bins_E, n_flux_bins};
		double mins[4] = {fake_data_p_low, fake_data_cos_low, \
			fake_data_E_low, flux_bins_low};
		double maxs[4] = {fake_data_p_high, fake_data_cos_high, \
			fake_data_E_high, flux_bins_high};
		THnSparseD* h4 = new THnSparseD("h4", \
			"Leading Muon cos, 3-momentum, Nu E spectrum", \
			4, n_bins, mins, maxs);

		// Loop over events and fill histograms
		bool firstEntry = true;
		double scale_factor;
		while(reader.Next()) {
			// Select neutrino type
			if(*nu_in_flavour != 14) continue;
			else {
				// Scale factors are the same for each event, so we only 
				// need to set the scale factor once. They depend on the 
				// total inclusive cross section, which is dependent on the 
				// generator and neutrino flavour. 
				if (firstEntry) {
					scale_factor = *fscalefactor;
					firstEntry = false;
				}	
				// Select NC/CC events
				if(! *flagCC) continue;
				else {
					// Here's where differential xsec variables are defined 
					// and used to make cuts. Kinematic variables are stored 
					// in each event as vectors of vectors, where each 
					// interior vector is a 4-momentum (E, px, py, pz), and 
					// each particle in the event contributes one of these 
					// to the external vector. The external vector is 
					// ordered by decreasing energy. So, to access the 
					// second most-energetic particle's py, call 
					// particle[1][2]. All other differential xsec variables 
					// can be constructed with the information in the files.
					// In this case:
					// If there is at least one muon, and the most energetic 
					// one is above the threshold, then get its 3-momentum. 
					// Get the neutrino energy as well. Finally, fill a 
					// histogram with these variables
					if(*n_muons > 0 && muons[0][0] > muon_threshold_E) {
						float muon_px = muons[0][1];
						float muon_py = muons[0][2];
						float muon_pz = muons[0][3];
						float muon_p = sqrt(pow(muon_px, 2) + \
							pow(muon_py, 2) + pow(muon_pz, 2));

						float nu_E = *nu_in_e;

						float muon_cos = muon_pz / muon_p;

						double fill_vec[4] = {muon_p, muon_cos, nu_E, nu_E};
					
						h4->Fill(fill_vec);
					}
				}
			}
		}
		
		// Multiply histograms by their scale factors to get cross sections 
		// in units of cm^2 / nucleon
		h4->Scale(scale_factor);
	
		// Normalize fluxes, and divide the histograms' energy axes by the 
		// flux
		flux->Scale(1.0 / flux->Integral());

		for(int i = 1; i <= fake_data_n_bins_p; i++) {
			for(int j = 1; j <= fake_data_n_bins_cos; j++) {
				for(int k = 1; k <= fake_data_n_bins_E; k++) {
					for(int l = 1; l <= n_flux_bins; l++) {
						int coord[4] = {i, j, k, l};
						float bin = h4->GetBin(coord);
						h4->SetBinContent(bin, \
						h4->GetBinContent(bin) / \
						flux->GetBinContent(l));
					}
				}
			}
		}
	
		// Integrate out the energy by projecting onto the xy plane, and 
		// attach to the histogram vector.
		vecHist_Tmu[file_index] = h4->Projection(0, 1, 2);

		// Divide histograms by their bin widths to get differential cross 
		// sections
		for(int i = 1; i <= fake_data_n_bins_p; i++) {
			for(int j = 1; j <= fake_data_n_bins_cos; j++) {
				for(int k = 1; k <= fake_data_n_bins_E; k++) {
					vecHist_Tmu[file_index]->SetBinContent(i, j, k, \
						vecHist_Tmu[file_index]->GetBinContent(i, j, k) / \
						vecHist_Tmu[file_index]->GetXaxis()->GetBinWidth(i) / \
						vecHist_Tmu[file_index]->GetYaxis()->GetBinWidth(j) / \
						vecHist_Tmu[file_index]->GetZaxis()->GetBinWidth(k));
				}
			}
		}
		
	}
*/


  // Open file to stored histograms
  TFile histo_file("xsec_histos.root", "RECREATE");

  // Visualise Single differential cross section: Tmu
  TCanvas* cTmu = new TCanvas("cTmu", "T_{#mu}");
	
  TLegend* lTmu = new TLegend(0.45, 0.15, 0.85, 0.30);

  for(int i = 1; i <= vecHist_Tmu[n_files - 1]->GetNbinsX(); i++) {
    vecHist_Tmu[n_files - 1]->SetBinError(i, vecHist_Tmu[n_files - 1]->GetBinContent(i) * 0.1);
  }	

  for(int i = 0; i < n_files; i++) {
    TH1* hTmu_xsec = vecHist_Tmu[i];
    hTmu_xsec->SetLineColor(myColours[i]);
    hTmu_xsec->SetStats(0);

    if(i == 0) {
      hTmu_xsec->SetMaximum(4e-39);
      hTmu_xsec->Draw("hist");
      hTmu_xsec->SetTitle(";T_{#mu} (GeV);d#sigma/dT_{#mu} (cm^{2} GeV^{-1} nucleon^{-1})");
    }
    else if(i == (n_files - 1)) {
      hTmu_xsec->Draw("same e1p");
    }
      else hTmu_xsec->Draw("hist same");

    hTmu_xsec->Write();
    lTmu->AddEntry(hTmu_xsec, file_alias[i]);		
  }

  Preliminary();
  lTmu->Draw();
	
  cTmu->Print("xsec/Tmu_xsec.png");


  // Visualise Single differential cross section: Cos Theta_mu
  TCanvas* cCosThetaMu = new TCanvas("cCosThetaMu", "cos #theta_{#mu}");

  TLegend* lCosThetaMu = new TLegend(0.15, 0.70, 0.55, 0.85);

  for(int i = 1; i <= vecHist_Tmu[n_files - 1]->GetNbinsX(); i++) {
    vecHist_CosThetaMu[n_files - 1]->SetBinError(i, vecHist_CosThetaMu[n_files - 1]->GetBinContent(i) * 0.1);
  }

  for(int i = 0; i < n_files; i++) {
    TH1* hCosThetaMu_xsec = vecHist_CosThetaMu[i];
    hCosThetaMu_xsec->SetLineColor(myColours[i]);
    hCosThetaMu_xsec->SetStats(0);

    if(i == 0) {
      hCosThetaMu_xsec->SetMaximum(80e-39);
      hCosThetaMu_xsec->Draw("hist");
      hCosThetaMu_xsec->SetTitle(";cos #theta_{#mu};d#sigma/d cos #theta_{#mu} (cm^{2} nucleon^{-1})");
    }
    else if(i == (n_files - 1)) {
      hCosThetaMu_xsec->Draw("same e1p");
    }
      else hCosThetaMu_xsec->Draw("hist same");

    hCosThetaMu_xsec->Write();
    lCosThetaMu->AddEntry(hCosThetaMu_xsec, file_alias[i]);
  }

  Preliminary();
  lCosThetaMu->Draw();

  cCosThetaMu->Print("xsec/CosThetaMu_xsec.png");


  // Visualise Single differential cross section: Q2
  TCanvas* cQ2 = new TCanvas("cQ2", "Q^{2}");

  TLegend* lQ2 = new TLegend(0.45, 0.70, 0.85, 0.85);

  for(int i = 1; i <= vecHist_Q2[n_files - 1]->GetNbinsX(); i++) {
    vecHist_Q2[n_files - 1]->SetBinError(i, vecHist_Q2[n_files - 1]->GetBinContent(i) * 0.1);
  }

  for(int i = 0; i < n_files; i++) {
    TH1* hQ2_xsec = vecHist_Q2[i];
    hQ2_xsec->SetLineColor(myColours[i]);
    hQ2_xsec->SetStats(0);

    if(i == 0) {
      hQ2_xsec->SetMaximum(12e-39);
      hQ2_xsec->Draw("hist");
      hQ2_xsec->SetTitle(";Q^{2} (GeV);d#sigma/dQ^{2} (cm^{2} (GeV/c)^{-2} nucleon^{-1})");
    }
    else if(i == (n_files - 1)) {
      hQ2_xsec->Draw("same e1p");
    }
      else hQ2_xsec->Draw("hist same");

    hQ2_xsec->Write();
    lQ2->AddEntry(hQ2_xsec, file_alias[i]);
  }

  Preliminary();
  lQ2->Draw();

  cQ2->Print("xsec/Q2_xsec.png");


  // Visualise Single differential cross section: Enu
  TCanvas* cEnu = new TCanvas("cEnu", "E_{#nu}");

  TLegend* lEnu = new TLegend(0.12, 0.70, 0.42, 0.85);

  for(int i = 1; i <= vecHist_Enu[n_files - 1]->GetNbinsX(); i++) {
    vecHist_Enu[n_files - 1]->SetBinError(i, vecHist_Enu[n_files - 1]->GetBinContent(i) * 0.1);
  }

  for(int i = 0; i < n_files; i++) {
    TH1* hEnu_xsec = vecHist_Enu[i];
    hEnu_xsec->SetLineColor(myColours[i]);
    hEnu_xsec->SetStats(0);

    if(i == 0) {
      hEnu_xsec->SetMaximum(3e-38);
      hEnu_xsec->Draw("hist");
      hEnu_xsec->SetTitle(";E_{#nu} (GeV);#sigma E / E_{#nu} (cm^{2} GeV^{-1} nucleon^{-1})");
    }
    else if(i == (n_files - 1)) {
      hEnu_xsec->Draw("same e1p");
    }
      else hEnu_xsec->Draw("hist same");

    hEnu_xsec->Write();
    lEnu->AddEntry(hEnu_xsec, file_alias[i]);
  }

  Preliminary();
  lEnu->Draw();

  cEnu->Print("xsec/Enu_xsec.png");


  // Visualise Single differential cross section: Eavail
  TCanvas* cEavail = new TCanvas("cEavail", "E_{avail}");

  TLegend* lEavail = new TLegend(0.45, 0.70, 0.85, 0.85);

  for(int i = 1; i <= vecHist_Eavail[n_files - 1]->GetNbinsX(); i++) {
    vecHist_Eavail[n_files - 1]->SetBinError(i, vecHist_Eavail[n_files - 1]->GetBinContent(i) * 0.1);
  }

  for(int i = 0; i < n_files; i++) {
    TH1* hEavail = vecHist_Eavail[i];
    hEavail->SetLineColor(myColours[i]);
    hEavail->SetStats(0);

    if(i == 0) {
      hEavail->SetMaximum(22e-39);
      hEavail->Draw("hist");
      hEavail->SetTitle(";E_{avail} (GeV);d#sigma / dE_{avail} (cm^{2} GeV^{-1} nucleon^{-1})");
    }
    else if(i == (n_files - 1)) {
      hEavail->Draw("same e1p");
    }
      else hEavail->Draw("hist same");

    hEavail->Write();
    lEavail->AddEntry(hEavail, file_alias[i]);
  }

  Preliminary();
  lEavail->Draw();

  cEavail->Print("xsec/Eavail_xsec.png");

 
  histo_file.Close();

}
