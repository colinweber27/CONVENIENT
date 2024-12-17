//////////////////////////////////////////////////////////////////
/// \brief  Macro to create fake data spectra for the numubar cc inc analysis to be used in the unfolding studies
/// \author Prabhjot Singh (prabhjot.singh@qmul.ac.uk) and Connor Johnson (conjohn1@rams.colostate.edu)
/// \date   05 Feb 2022
//
// Command to run on gpvm: cafe -bq -l 1 --numubarccinc makeFakeDataFluxes.C
//
// Command to run on grid: submit_cafana.py -n 40 --numubarccinc -r development -o /pnfs/nova/scratch/users/colweber /exp/nova/app/users/colweber/Convenient/flux/NuMI/RHC/DeriveFlux50MeVwidth/makeFakeDataFluxes.C
//////////////////////////////////////////////////////////////////

#include "CAFAna/Vars/Vars.h"
#include "CAFAna/Core/Binning.h"
#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Core/SpectrumLoaderBase.h"
#include "3FlavorAna/Vars/NumuVars.h"
#include "CAFAna/XSec/Flux.h"

#include "CAFAna/Weights/GenieWeights.h"
#include "CAFAna/Weights/PPFXWeights.h"

#include "NDAna/numubarcc_inc/NumubarCCIncBins.h"
#include "NDAna/numubarcc_inc/NumubarCCIncVars.h"
#include "NDAna/numubarcc_inc/NumubarCCIncCuts.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

#include <iostream>
#include <cmath>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <map>
#include <utility>

using namespace ana;

/*std::map<std::string, std::pair<ana::HistAxis, ana::NuTruthHistAxis>> unfoldingVars{
  {"fakedata_eavail",          {ana::xsec::numubarcc::kRecoMuKEVsCosVsEavailStandardAxis, ana::xsec::numubarcc::kTrueMuKEVsCosVsEavailStandardAxis_NT}},
  {"fakedata_enu",             {ana::xsec::numubarcc::kRecoNeutrinoEAxis, ana::xsec::numubarcc::kTrueNeutrinoEAxis_NT}},
  {"fakedata_q2",              {ana::xsec::numubarcc::kRecoQ2Axis,        ana::xsec::numubarcc::kTrueQ2Axis_NT}}
};

std::map<std::string, ana::Cut> fakeDataCuts{
  {"fakedata_eavail", ana::xsec::numubarcc::kAllNumuBarCCCuts},
  {"fakedata_enu",    ana::xsec::numubarcc::kAllNumuBarCCCutsIncPhase},
  {"fakedata_q2",     ana::xsec::numubarcc::kAllNumuBarCCCutsIncPhase}
};*/

const ana::NuTruthWeight   ppfxXSecWeights_NT = ana::xsec::numubarcc::std_wgt_NT;
const ana::Weight          ppfxXSecWeights    = ana::WeightFromNuTruthWeight(ppfxXSecWeights_NT, 1);

const std::vector<std::pair<const std::string, const ana::Weight>> weights{
  {"unwgt",             ana::kUnweighted},
  {"ppfxwgt",           ana::xsec::numubarcc::flux_wgt},
  {"ppfxxsecwgt",       ppfxXSecWeights},
  {"nonovatune",        ana::xsec::numubarcc::std_non_nova_wgt},
  {"cv2020",            ana::xsec::numubarcc::std_CV2020_wgt}
};

const std::vector<std::pair<const std::string, const ana::NuTruthWeight>> weights_NT{
  {"unwgt",             ana::kNuTruthUnweighted},
  {"ppfxwgt",           ana::xsec::numubarcc::flux_wgt_NT},
  {"ppfxxsecwgt",       ana::xsec::numubarcc::std_wgt_NT},
  {"nonovatune",        ana::xsec::numubarcc::std_non_nova_wgt_NT},
  {"cv2020",            ana::xsec::numubarcc::std_CV2020_wgt_NT}
};

void makeFakeDataFluxes(std::string outputfile = "fSpecsFakeData.root"){

  std::string fakedata_def  = "prod_flatsumdecaf_development_nd_genie_N1810j0211a_nonswap_rhc_nova_v08_full_ndphysics_contain_v1";     // 100% sample - 400 files
  std::string nominal_def   = "prod_flatsumdecaf_development_nd_genie_N1810j0211a_nonswap_rhc_nova_v08_full_ndphysics_contain_v1";     // 100% sample - 400 files

  SpectrumLoaderBase * fdloader   = new SpectrumLoader(fakedata_def);
  SpectrumLoaderBase * nomloader  = new SpectrumLoader(nominal_def);

  std::map<std::string, std::map<std::string, Spectrum*> > fakeDataSpectrumsReco;
  std::map<std::string, std::map<std::string, Spectrum*> > fakeDataSpectrumsTrue;

  ////////////// Fake data /////////////////////////////////////////////////
  /*for(std::pair<std::string, std::pair<ana::HistAxis, ana::NuTruthHistAxis>> unfoldingVar : unfoldingVars){
    for(std::pair<const string, const ana::Weight> weight : weights){
      ana::HistAxis& recoAxis  = unfoldingVar.second.first;
      Spectrum * fakeDataReco  = new Spectrum(*fdloader, recoAxis, fakeDataCuts.at(unfoldingVar.first),  kNoShift, weight.second);
      fakeDataSpectrumsReco    [unfoldingVar.first][weight.first] = fakeDataReco;
    }//end of loop over list of weights

    // Loop over NuTruth
    for(std::pair<const string, const ana::NuTruthWeight> weight_NT : weights_NT){
      Spectrum* fakeDataTrue;
      if(unfoldingVar.first == "fakedata_eavail"){
        std::cout << unfoldingVar.first  << std::endl;
        fakeDataTrue = new Spectrum(*fdloader, unfoldingVar.second.second, ana::xsec::numubarcc::kIsTrueSig_NT, kNoShift, weight_NT.second); // Now axis and signal are from NuTruth
        fakeDataSpectrumsTrue  [unfoldingVar.first][weight_NT.first] = fakeDataTrue;
      }
      else{
        std::cout << unfoldingVar.first << std::endl;
        fakeDataTrue = new Spectrum(*fdloader, unfoldingVar.second.second, ana::xsec::numubarcc::kIsTrueSigIncPhase_NT, kNoShift, weight_NT.second); // Now axis and signal are from NuTruth
        fakeDataSpectrumsTrue  [unfoldingVar.first][weight_NT.first] = fakeDataTrue;
      }

    }//end of loop over list of weights
  }//end of loop over unfolding variables*/

  ////////////// Fluxes ////////////////////////////////////////////////////
  const int     wanted_pdg    = -14;
  const Binning FluxBinning   = Binning::Simple(480, 0.0, 120.0);

  // Lets try the loop so that we can make have fluxes for all type of weights

  std::map<std::string, std::map<std::string, Spectrum*> > fluxSpectrums;

  for(std::pair<const string, const ana::NuTruthWeight> weight_NT : weights_NT){
    Spectrum * fluxCV    = ana::DeriveFlux(*nomloader, FluxBinning, wanted_pdg, ana::xsec::numubarcc::numucc_fiducial_min, ana::xsec::numubarcc::numucc_fiducial_max, kNoShift, weight_NT.second);

    fluxSpectrums["CV"][weight_NT.first] = fluxCV;

  }//end of loop over nu truth weights

  ////////////// Go() //////////////////////////////////////////////////////
  // fdloader  ->Go();
  nomloader ->Go();

  ////////////// Saving ////////////////////////////////////////////////////
  TFile * fOut = new TFile(outputfile.c_str(), "RECREATE");
  TDirectory * dir;

  /* for(std::pair<std::string, std::map<std::string, Spectrum*> > fakeDataReco : fakeDataSpectrumsReco){
    // Fake Data Reco
    dir = fOut  ->mkdir(fakeDataReco.first.c_str());
    dir = dir   ->mkdir("reco");
    dir         ->cd();

    std::map<std::string, Spectrum*>& weightSpecs = fakeDataReco.second;
    for(std::pair<std::string, Spectrum*> weightSpec : weightSpecs)
      weightSpec.second->SaveTo(dir->mkdir(weightSpec.first.c_str()), weightSpec.first.c_str());
  }

  // Fake Data Truth
  for(std::pair<std::string, std::map<std::string, Spectrum*> > fakeDataTrue : fakeDataSpectrumsTrue){
    dir = fOut->GetDirectory(fakeDataTrue.first.c_str(), true);
    dir = dir ->mkdir("true");
    dir ->cd();

    std::map<std::string, Spectrum*>& weightSpecs = fakeDataTrue.second;
    for(std::pair<std::string, Spectrum*> weightSpec : weightSpecs)
      weightSpec.second->SaveTo(dir->mkdir(weightSpec.first.c_str()), weightSpec.first.c_str());
  }*/

  // Flux
  for(std::pair<std::string, std::map<std::string, Spectrum*> > fakeDataTrue : fluxSpectrums){

    dir = fOut  ->mkdir("flux");
    dir = dir   ->mkdir(fakeDataTrue.first.c_str());
    dir         ->cd();

    std::map<std::string, Spectrum*>& weightSpecs = fakeDataTrue.second;
    for(std::pair<std::string, Spectrum*> weightSpec : weightSpecs)
      weightSpec.second->SaveTo(dir->mkdir(weightSpec.first.c_str()), weightSpec.first.c_str());
  }//end of loop over various fake data spectra

   // Close output file
  fOut    ->Close();
  std::cout << " "                                       << std::endl;
  std::cout << "Output file name is " << fOut->GetName() << std::endl;
  std::cout << " "                                       << std::endl;

}//end of main function makeFakeDataFluxes

