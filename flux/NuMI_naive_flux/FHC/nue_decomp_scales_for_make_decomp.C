#include "CAFAna/Core/HistAxis.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Cuts/TimingCuts.h"
#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Cuts/NueCutsSecondAna.h"
#include "3FlavorAna/Cuts/NueCuts2020.h"
#include "3FlavorAna/Cuts/NumuCuts.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "3FlavorAna/Decomp/BENDecomp.h"
#include "CAFAna/Decomp/IDecomp.h"
#include "3FlavorAna/Decomp/MichelDecomp.h"
#include "3FlavorAna/Decomp/NumuDecomp.h"
#include "CAFAna/Decomp/ProportionalDecomp.h"
#include "CAFAna/Extrap/ModularExtrap.h"
#include "CAFAna/Prediction/PredictionExtrap.h"
#include "CAFAna/Prediction/PredictionNoExtrap.h"
#include "CAFAna/Weights/GenieWeights.h"
#include "CAFAna/Vars/HistAxes.h"
#include "3FlavorAna/Vars/NueVars.h"
#include "3FlavorAna/Vars/NueVarsExtra.h"
#include "CAFAna/Weights/PPFXWeights.h"
#include "CAFAna/Vars/Vars.h"
#include "CAFAna/Weights/XsecTunes.h"

#include "StandardRecord/Proxy/SRProxy.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "TPad.h"

#include <iostream>
#include <memory>

using namespace ana;

//To be used with files produced with naming scheme /CAFAna/nue/Ana2018/Decomp/make_michel.C

void nue_decomp_scales_for_make_decomp(bool makeCombo = false, bool makeProp = false)
{
  //These are root file that contain the decompositions. Were made using make_michel.C with updated 2020 cuts. 
  //std::string filename = "nue_dcmp.FHC2020.root";
  std::string filename = "nue_dcmp.RHC2020.root";
  TFile* fin = new TFile(filename.c_str(), "read");
  
  //Only used for FHC in the analysis
  if(makeCombo){
    std::cout << "Extracting Combo scales...";
    std::string dname="nue_decomps/michelbenDCMP";
    MichelDecomp* michel = LoadFromFile<MichelDecomp>(fin, dname).release();


    double pot = michel->Data_Component().POT();
    std::cout << "Michel decomp POT is: " <<  pot << std::endl;




    TH1* data = michel->Data_Component().ToTH1(pot);
    TH1* nue_nom = michel->MC_NueComponent().ToTH1(pot);
    TH1* numu_nom = michel->MC_NumuComponent().ToTH1(pot);
    TH1* antinue_nom = michel->MC_AntiNueComponent().ToTH1(pot);
    TH1* antinumu_nom = michel->MC_AntiNumuComponent().ToTH1(pot);
    TH1* nc_nom = michel->MC_NCComponent().ToTH1(pot);
    

    
    TH1* nue_ratio = (TH1*)((michel->NueComponent().ToTH1(pot))->Clone("nue"));
    nue_ratio->Divide(nue_nom);
    
    TH1* antinue_ratio = (TH1*)((michel->AntiNueComponent().ToTH1(pot))->Clone("antinue"));
    antinue_ratio->Divide(antinue_nom);
    
    TH1* antinumu_ratio = (TH1*)((michel->AntiNumuComponent().ToTH1(pot))->Clone("antinumu"));
    antinumu_ratio->Divide(antinumu_nom);
    
    TH1* numu_ratio = (TH1*)((michel->NumuComponent().ToTH1(pot))->Clone("numu"));
    numu_ratio->Divide(numu_nom);
    
    TH1* nc_ratio = (TH1*)((michel->NCComponent().ToTH1(pot))->Clone("nc"));
    nc_ratio->Divide(nc_nom);
    
    TFile* outputFile = new TFile("nue2020_fhc_combo_decomp_ratios.root", "update");
    outputFile->cd();
    nue_ratio->Write();
    numu_ratio->Write();
    nc_ratio->Write();
    antinue_ratio->Write();
    antinumu_ratio->Write();
    outputFile->Close();
    delete outputFile;
  }
  //Used for RHC in the analysis
  if(makeProp){

    std::string dname="nue_decomps/propDCMP";

    ana::ProportionalDecomp* prop = ana::LoadFromFile<ana::ProportionalDecomp>(fin,dname).release();

    double pot = prop->Data_Component().POT();
    std::cout << "Proportional decomp POT is: " <<  pot << std::endl;



    TH1* data = prop->Data_Component().ToTH1(pot);
    TH1* nue_nom = prop->MC_NueComponent().ToTH1(pot);
    TH1* numu_nom = prop->MC_NumuComponent().ToTH1(pot);
    TH1* antinue_nom = prop->MC_AntiNueComponent().ToTH1(pot);
    TH1* antinumu_nom = prop->MC_AntiNumuComponent().ToTH1(pot);
    TH1* nc_nom = prop->MC_NCComponent().ToTH1(pot);


    TH1* nue_ratio = (TH1*)((prop->NueComponent().ToTH1(pot))->Clone("nue"));
    nue_ratio->Divide(nue_nom);

    TH1* antinue_ratio = (TH1*)((prop->AntiNueComponent().ToTH1(pot))->Clone("antinue"));
    antinue_ratio->Divide(antinue_nom);

    TH1* antinumu_ratio = (TH1*)((prop->AntiNumuComponent().ToTH1(pot))->Clone("antinumu"));
    antinumu_ratio->Divide(antinumu_nom);

    TH1* numu_ratio = (TH1*)((prop->NumuComponent().ToTH1(pot))->Clone("numu"));
    numu_ratio->Divide(numu_nom);

    TH1* nc_ratio = (TH1*)((prop->NCComponent().ToTH1(pot))->Clone("nc"));
    nc_ratio->Divide(nc_nom);

    TFile* outputFile = new TFile("nue2020_rhc_prop_decomp_ratios.root", "update");
    outputFile->cd();
    nue_ratio->Write();
    numu_ratio->Write();
    nc_ratio->Write();
    antinue_ratio->Write();
    antinumu_ratio->Write();
    outputFile->Close();
    delete outputFile;

  }
  


  


}
