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


void nue_decomp_scales(bool makeFile = false)
{

  const HistAxis axisNue = {"Nue Energy / Selection Bin", kNue2020Binning, kNue2020AnaBin};      
  std::string filename = "nue2020_decomp_spectra_calibdown.root";

  if(makeFile){


    SpectrumLoader loader_data("def_snapshot prod_caf_R19-11-18-prod5reco.d.f.h_nd_numi_fhc_period235910_v1 with limit 200");
    SpectrumLoader loader_mc("def_snapshot prod_caf_R19-11-18-prod5reco.d.h_nd_genie_N1810j0211a_nonswap_fhc_nova_v08_full_v1 with limit 200");

    loader_mc.SetSpillCut(kStandardSpillCuts);
    loader_data.SetSpillCut(kStandardSpillCuts);

    BENDecomp* decompBEN = new BENDecomp(loader_mc, loader_data, axisNue, kNue2020ND, kNoShift, kNoShift, kPPFXFluxCVWgt*kXSecCVWgt2020);
    MichelDecomp* decompMichel = new MichelDecomp(loader_mc, loader_data, axisNue, kNue2020ND, decompBEN, kNoShift, kNoShift, kPPFXFluxCVWgt*kXSecCVWgt2020);
    ProportionalDecomp* decompProportional = new ProportionalDecomp(loader_mc, loader_data, axisNue, kNue2020ND, kNoShift, kNoShift, kPPFXFluxCVWgt*kXSecCVWgt2020);

    Spectrum* data = new Spectrum(loader_data, axisNue, kNue2020ND, kNoShift);
    PredictionNoExtrap* mc_nom = new PredictionNoExtrap(loader_mc, kNullLoader, kNullLoader, axisNue.GetLabels()[0], axisNue.GetBinnings()[0], axisNue.GetVars()[0], 
							kNue2020ND, kNoShift, kPPFXFluxCVWgt*kXSecCVWgt2020);

    loader_mc.Go();
    loader_data.Go();
    TFile* fout = new TFile(filename.c_str(), "recreate");
   
    data->SaveTo(fout, "data");
    mc_nom->SaveTo(fout, "mc_nom");
    decompBEN->SaveTo(fout, "decompBEN");
    decompMichel->SaveTo(fout, "decompMichel");
    decompProportional->SaveTo(fout, "decompProportional");

    delete fout;
  }
  else {

    TFile* fin = new TFile(filename.c_str(), "read");
    std::cout << "Extracting scales...";
  
    Spectrum* spec_data = Spectrum::LoadFrom(fin, "data").release();
    double pot = spec_data->POT();
    std::cout << pot << std::endl;
    PredictionNoExtrap* mc_nom = PredictionNoExtrap::LoadFrom(fin, "mc_nom").release();
    MichelDecomp* michel = MichelDecomp::LoadFrom(fin, "decompMichel").release();

    TH1* data = spec_data->ToTH1(pot);
    TH1* nue_nom = mc_nom->ComponentCC(12, 12).Unoscillated().ToTH1(pot);
    TH1* numu_nom = mc_nom->ComponentCC(14, 14).Unoscillated().ToTH1(pot);
    TH1* antinue_nom = mc_nom->ComponentCC(-12, -12).Unoscillated().ToTH1(pot);
    TH1* antinumu_nom = mc_nom->ComponentCC(-14, -14).Unoscillated().ToTH1(pot);
    TH1* nc_nom = mc_nom->ComponentNC().ToTH1(pot);

    // Michel

    TH1* nue_ratio = (TH1*)((michel->NueComponent().ToTH1(pot))->Clone("combo_beam_nue"));
    nue_ratio->Divide(nue_nom);

    TH1* antinue_ratio = (TH1*)((michel->AntiNueComponent().ToTH1(pot))->Clone("combo_beam_antinue"));
    antinue_ratio->Divide(antinue_nom);

    TH1* antinumu_ratio = (TH1*)((michel->AntiNumuComponent().ToTH1(pot))->Clone("combo_beam_antinumu"));
    antinumu_ratio->Divide(antinumu_nom);

    TH1* numu_ratio = (TH1*)((michel->NumuComponent().ToTH1(pot))->Clone("combo_beam_numu"));
    numu_ratio->Divide(numu_nom);

    TH1* nc_ratio = (TH1*)((michel->NCComponent().ToTH1(pot))->Clone("combo_beam_nc"));
    nc_ratio->Divide(nc_nom);

    TFile* outputFile = new TFile("nue2017_decomp_ratios.root", "update");
    outputFile->cd();
    nue_ratio->Write();
    numu_ratio->Write();
    nc_ratio->Write();
    antinue_ratio->Write();
    antinumu_ratio->Write();
    outputFile->Close();
    delete outputFile;

    // TCanvas *c0 = new TCanvas("c0", "c0", 200, 10, 800, 600);
    // c0->cd();
    // nue_ratio->GetYaxis()->SetRangeUser(0., 2.);
    // nue_ratio->SetLineColor(kPink);
    // nue_ratio->Draw("hist");
    // numu_ratio->SetLineColor(kGreen+2);
    // numu_ratio->Draw("hist same");
    // nc_ratio->SetLineColor(kBlue);
    // nc_ratio->Draw("hist same");
    // c0->Update();
    // c0->Print("michel_ratios.pdf");




  }


}

