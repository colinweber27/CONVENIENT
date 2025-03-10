// Author: Kevin Vockerodt (adapted from Prabhjot Singh)
// Purpose: This file extracts a histogram of the neutrino flux at the NOvA 
// ND for all beam components by backtracking from the CAF file using 
// simulated cross section information. It applies PPFX weights to tune the 
// spectrum.

// Command: cafe -bq deriveFlux.C output.root

// Parameters
// 	output.root : string
// 		Output filename 

// Returns
// 	fOut : ROOT TFile*
// 		ROOT file containing the histograms of the components of the NOvA ND 
// 		flux

#include "CAFAna/Vars/Vars.h"
#include "CAFAna/Core/Binning.h"
#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Core/SpectrumLoaderBase.h"
#include "3FlavorAna/Vars/NumuVars.h"
#include "CAFAna/XSec/Flux.h"
#include "CAFAna/Weights/XsecTunes.h"

#include "CAFAna/Weights/GenieWeights.h"
#include "CAFAna/Weights/PPFXWeights.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TVector3.h"

#include <iostream>
#include <cmath>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <map>
#include <utility>

using namespace ana;


const NuTruthWeight unweighted_NT = kNuTruthUnweighted;
const NuTruthWeight PPFX_NT = kPPFXFluxCVWgtST;
const NuTruthWeight PPFX_GSF_NT = kPPFXFluxCVWgtST * kXSecCVWgt2020GSFProd51_NT;


const TVector3 * numu_fid_min = new TVector3(-130, -130, 100);
const TVector3 * numu_fid_max = new TVector3( 140, 140, 1000);

const TVector3 * nue_fid_min = new TVector3(-130, -140, 150);
const TVector3 * nue_fid_max = new TVector3( 150, 140, 800);


void deriveFlux(std::string outputfile = "DerivedFluxes.root"){

  std::string samdefRHC = "prod_flatsumdecaf_development_nd_genie_N1810j0211a_nonswap_rhc_nova_v08_full_ndphysics_contain_v1";     // 100% sample - 400 files
  std::string samdefFHC = "prod_flatsumdecaf_development_nd_genie_N1810j0211a_nonswap_fhc_nova_v08_full_ndphysics_contain_v1";     // 100% sample - 400 files

  SpectrumLoaderBase * loaderRHC = new SpectrumLoader(samdefRHC);
  SpectrumLoaderBase * loaderFHC = new SpectrumLoader(samdefFHC);

  const Binning FluxBinning   = Binning::Simple(2400, 0.0, 120.0);

  // Unweighted
/*  Spectrum* sRHC_numubar_unweighted = DeriveFlux(*loaderRHC, FluxBinning, -14, numu_fid_min, numu_fid_max, kNoShift, unweighted_NT);
  Spectrum* sRHC_numu_unweighted    = DeriveFlux(*loaderRHC, FluxBinning,  14, numu_fid_min, numu_fid_max, kNoShift, unweighted_NT);
  Spectrum* sFHC_numubar_unweighted = DeriveFlux(*loaderFHC, FluxBinning, -14, numu_fid_min, numu_fid_max, kNoShift, unweighted_NT);
  Spectrum* sFHC_numu_unweighted    = DeriveFlux(*loaderFHC, FluxBinning,  14, numu_fid_min, numu_fid_max, kNoShift, unweighted_NT);

  Spectrum* sRHC_nuebar_unweighted  = DeriveFlux(*loaderRHC, FluxBinning, -12, nue_fid_min, nue_fid_max, kNoShift, unweighted_NT);
  Spectrum* sRHC_nue_unweighted     = DeriveFlux(*loaderRHC, FluxBinning,  12, nue_fid_min, nue_fid_max, kNoShift, unweighted_NT);
  Spectrum* sFHC_nuebar_unweighted  = DeriveFlux(*loaderFHC, FluxBinning, -12, nue_fid_min, nue_fid_max, kNoShift, unweighted_NT);
  Spectrum* sFHC_nue_unweighted     = DeriveFlux(*loaderFHC, FluxBinning,  12, nue_fid_min, nue_fid_max, kNoShift, unweighted_NT);*/

  // PPFX weighted
  Spectrum* sRHC_numubar_PPFX       = DeriveFlux(*loaderRHC, FluxBinning, -14, numu_fid_min, numu_fid_max, kNoShift, PPFX_NT);
  Spectrum* sRHC_numu_PPFX          = DeriveFlux(*loaderRHC, FluxBinning,  14, numu_fid_min, numu_fid_max, kNoShift, PPFX_NT);
  Spectrum* sFHC_numubar_PPFX       = DeriveFlux(*loaderFHC, FluxBinning, -14, numu_fid_min, numu_fid_max, kNoShift, PPFX_NT);
  Spectrum* sFHC_numu_PPFX          = DeriveFlux(*loaderFHC, FluxBinning,  14, numu_fid_min, numu_fid_max, kNoShift, PPFX_NT);

  Spectrum* sRHC_nuebar_PPFX        = DeriveFlux(*loaderRHC, FluxBinning, -12, nue_fid_min, nue_fid_max, kNoShift, PPFX_NT);
  Spectrum* sRHC_nue_PPFX           = DeriveFlux(*loaderRHC, FluxBinning,  12, nue_fid_min, nue_fid_max, kNoShift, PPFX_NT);
  Spectrum* sFHC_nuebar_PPFX        = DeriveFlux(*loaderFHC, FluxBinning, -12, nue_fid_min, nue_fid_max, kNoShift, PPFX_NT);
  Spectrum* sFHC_nue_PPFX           = DeriveFlux(*loaderFHC, FluxBinning,  12, nue_fid_min, nue_fid_max, kNoShift, PPFX_NT);

  // PPFX and GSF weighted
/*  Spectrum* sRHC_numubar_PPFX_GSF   = DeriveFlux(*loaderRHC, FluxBinning, -14, numu_fid_min, numu_fid_max, kNoShift, PPFX_GSF_NT);
  Spectrum* sRHC_numu_PPFX_GSF      = DeriveFlux(*loaderRHC, FluxBinning,  14, numu_fid_min, numu_fid_max, kNoShift, PPFX_GSF_NT);
  Spectrum* sFHC_numubar_PPFX_GSF   = DeriveFlux(*loaderFHC, FluxBinning, -14, numu_fid_min, numu_fid_max, kNoShift, PPFX_GSF_NT);
  Spectrum* sFHC_numu_PPFX_GSF      = DeriveFlux(*loaderFHC, FluxBinning,  14, numu_fid_min, numu_fid_max, kNoShift, PPFX_GSF_NT);

  Spectrum* sRHC_nuebar_PPFX_GSF    = DeriveFlux(*loaderRHC, FluxBinning, -12, nue_fid_min, nue_fid_max, kNoShift, PPFX_GSF_NT);
  Spectrum* sRHC_nue_PPFX_GSF       = DeriveFlux(*loaderRHC, FluxBinning,  12, nue_fid_min, nue_fid_max, kNoShift, PPFX_GSF_NT);
  Spectrum* sFHC_nuebar_PPFX_GSF    = DeriveFlux(*loaderFHC, FluxBinning, -12, nue_fid_min, nue_fid_max, kNoShift, PPFX_GSF_NT);
  Spectrum* sFHC_nue_PPFX_GSF       = DeriveFlux(*loaderFHC, FluxBinning,  12, nue_fid_min, nue_fid_max, kNoShift, PPFX_GSF_NT);*/


  ////////////// Go ////////////////////////////////////////////////////////
  loaderRHC->Go();
  loaderFHC->Go();

  ////////////// Saving ////////////////////////////////////////////////////
  TFile* fOut = new TFile(outputfile.c_str(), "RECREATE");
  TDirectory* dir_RHC = fOut->mkdir("RHC");
  TDirectory* dir_FHC = fOut->mkdir("FHC");

/*  sRHC_numubar_unweighted->SaveTo(dir_RHC, "RHC_numubar"); 
  sRHC_numu_unweighted   ->SaveTo(dir_RHC, "RHC_numu"); 
  sFHC_numubar_unweighted->SaveTo(dir_FHC, "FHC_numubar"); 
  sFHC_numu_unweighted   ->SaveTo(dir_FHC, "FHC_numu"); 

  sRHC_nuebar_unweighted->SaveTo(dir_RHC, "RHC_nuebar");
  sRHC_nue_unweighted   ->SaveTo(dir_RHC, "RHC_nue");
  sFHC_nuebar_unweighted->SaveTo(dir_FHC, "FHC_nuebar");
  sFHC_nue_unweighted   ->SaveTo(dir_FHC, "FHC_nue");*/


  sRHC_numubar_PPFX->SaveTo(dir_RHC, "RHC_numubar_PPFX"); 
  sRHC_numu_PPFX   ->SaveTo(dir_RHC, "RHC_numu_PPFX"); 
  sFHC_numubar_PPFX->SaveTo(dir_FHC, "FHC_numubar_PPFX"); 
  sFHC_numu_PPFX   ->SaveTo(dir_FHC, "FHC_numu_PPFX"); 

  sRHC_nuebar_PPFX->SaveTo(dir_RHC, "RHC_nuebar_PPFX");
  sRHC_nue_PPFX   ->SaveTo(dir_RHC, "RHC_nue_PPFX");
  sFHC_nuebar_PPFX->SaveTo(dir_FHC, "FHC_nuebar_PPFX");
  sFHC_nue_PPFX   ->SaveTo(dir_FHC, "FHC_nue_PPFX");


/*  sRHC_numubar_PPFX_GSF->SaveTo(dir_RHC, "RHC_numubar_PPFX_GSF"); 
  sRHC_numu_PPFX_GSF   ->SaveTo(dir_RHC, "RHC_numu_PPFX_GSF"); 
  sFHC_numubar_PPFX_GSF->SaveTo(dir_FHC, "FHC_numubar_PPFX_GSF"); 
  sFHC_numu_PPFX_GSF   ->SaveTo(dir_FHC, "FHC_numu_PPFX_GSF"); 

  sRHC_nuebar_PPFX_GSF->SaveTo(dir_RHC, "RHC_nuebar_PPFX_GSF");
  sRHC_nue_PPFX_GSF   ->SaveTo(dir_RHC, "RHC_nue_PPFX_GSF");
  sFHC_nuebar_PPFX_GSF->SaveTo(dir_FHC, "FHC_nuebar_PPFX_GSF");
  sFHC_nue_PPFX_GSF   ->SaveTo(dir_FHC, "FHC_nue_PPFX_GSF");*/

   // Close output file
  fOut->Close();

}


