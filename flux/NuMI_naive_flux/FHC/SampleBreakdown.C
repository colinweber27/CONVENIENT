// Author: Copied from Miranda Elkins
// Date: Copied from folder dated 19 April 2023
// Purpose: To show the components of the NuMI beam at the NOvA ND
//
// Command: cafe -bq -l 1 SampleBreakdown.C

#include "CAFAna/Vars/Vars.h"
#include "CAFAna/Weights/GenieWeights.h"
#include "CAFAna/Weights/PPFXWeights.h"
#include "CAFAna/Vars/HistAxes.h"
#include "CAFAna/Weights/XsecTunes.h"

#include "3FlavorAna/Cuts/NueCuts2020.h"
#include "3FlavorAna/Cuts/NumuCuts2020.h"
#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Cuts/TruthCuts.h"
#include "3FlavorAna/Cuts/NumuCuts.h"
//#include "3FlavorAna/Cuts/BeamNueCuts.h"
//#include "CAFAna/Cuts/BeamCuts.h"

#include "CAFAna/Core/HistAxis.h"
#include "CAFAna/Core/Loaders.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Cuts/SpillCuts.h"

#include "Utilities/rootlogon.C"
#include "TDirectory.h"
#include "TLegend.h"
#include "TCanvas.h"
#include <iostream>
#include "Utilities/func/ROOTHelpers.h"
#include "TGaxis.h"
#include "SampleCuts.h"
using namespace ana;


void CornerLabels(std::string Str) {
  TLatex* CornLab = new TLatex(.1, .93, Str.c_str());
  CornLab->SetTextColor(kGray+1);
  CornLab->SetNDC();
  CornLab->SetTextSize (1.5/30.);
  CornLab->SetTextAlign(11);
  CornLab->Draw();
}

void SampleBreakdown(const bool isRHC = false, const bool makeSpecs = true, const bool makePlots = true){
  
  
  const Weight wei = kXSecCVWgt2020* kPPFXFluxCVWgt;
  Cut cut = kNumu2020ND;

  std::string horn_current; //= "FHC";  
  if(isRHC) horn_current = "RHC";
  else horn_current = "FHC";

  HistAxis axis ("CVN 2D", kNue2020Binning,  kNue2020AnaBin);
  Binning MEBinning = Binning::Simple(3,0,3);
  HistAxis MEAxis("N_{Michels}",MEBinning,kNMichels);
  

  //Total spectra
  if(makeSpecs){
    //=============Set your definitions and spill cuts ======================

    //RHC 
    // SpectrumLoader loaderData("prod_caf_R19-11-18-prod5reco.g_nd_numi_rhc_full_v1_goodruns");
    // SpectrumLoader loaderMC("prod_caf_R19-11-18-prod5reco.d_nd_genie_N1810j0211a_nonswap_rhc_nova_v08_full_v1"); 

	//FHC
	//SpectrumLoader loaderData("prod_caf_R19-11-18-prod5reco.d.f.h.l_nd_numi_fhc_full_v1_goodruns");
    SpectrumLoader loaderMC("prod_caf_R19-11-18-prod5reco.d.h.l_nd_genie_N1810j0211a_nonswap_fhc_nova_v08_full_v1"); 


    // loaderData.SetSpillCut(kStandardSpillCuts);
    loaderMC.SetSpillCut(kStandardDQCuts);


    //============= Set up the spectra we want ==============================

    //Michel is only needed for FHC. Should automate to run for FHC only or make a separate set of FHC and RHC scripts
    // Spectrum fData(loaderData, axis, cut, kNoShift, wei);
    /* Spectrum fMichelsNumu (loaderMC, MEAxis, cut && kIsNumuCC, kNoShift, wei);
    Spectrum fMichelsNC (loaderMC, MEAxis, cut && kIsNC, kNoShift, wei);
    Spectrum fMichelsNue (loaderMC, MEAxis, cut && kIsBeamNue, kNoShift, wei);
    Spectrum fMichelsData (loaderData, MEAxis, cut, kNoShift, wei);*/

    //Antinue in specified horn current broken into components
    Spectrum fAntiNueCCFromPiMinus ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
				    kIsFromPiMinusDk && kIsBeamNue && kIsAntiNu && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNueCCFromKaMinus ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
				    kIsFromKaMinusDk && kIsBeamNue && kIsAntiNu && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNueCCFromPiPlus  ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,  
				    kIsFromPiPlusDk && kIsBeamNue && kIsAntiNu && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNueCCFromKaPlus  ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,  
				    kIsFromKaPlusDk && kIsBeamNue && kIsAntiNu && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNueCCFromKaLong  ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,  
				    kIsFromKaLongDk && kIsBeamNue && kIsAntiNu && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNueCCFromOtherTot ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,
				     kIsVtxCont && !kIsFromKaLongDk && !kIsFromKaPlusDk && !kIsFromKaMinusDk
					&& !kIsFromPiPlusDk && !kIsFromPiMinusDk && kIsBeamNue && kIsAntiNu, kNoShift, wei);
    Spectrum fAntiNueCCFromRock ("True neutrino Energy (GeV)", ana::Binning::Simple(400,0,20), loaderMC, kTrueNeutrinoE, !kIsVtxCont && kIsAntiNu && kIsBeamNue, kNoShift, wei);
    

    //Nue in specified horn current broken into components 
    Spectrum fNueCCFromPiMinus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
				   kIsFromPiMinusDk && !kIsAntiNu && kIsBeamNue && kIsVtxCont, kNoShift, wei);
    Spectrum fNueCCFromKaMinus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
				   kIsFromKaMinusDk && !kIsAntiNu && kIsBeamNue && kIsVtxCont, kNoShift, wei);
    Spectrum fNueCCFromPiPlus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
				  kIsFromPiPlusDk && !kIsAntiNu && kIsBeamNue && kIsVtxCont, kNoShift, wei);
    Spectrum fNueCCFromKaPlus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
				  kIsFromKaPlusDk  && !kIsAntiNu && kIsBeamNue && kIsVtxCont, kNoShift, wei);
    Spectrum fNueCCFromKaLong    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
				  kIsFromKaLongDk && !kIsAntiNu && kIsBeamNue && kIsVtxCont, kNoShift, wei);
    Spectrum fNueCCFromOtherTot    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
				    !kIsFromKaLongDk && !kIsFromKaPlusDk && !kIsFromKaMinusDk 
				    && !kIsFromPiPlusDk && !kIsFromPiMinusDk && kIsBeamNue && !kIsAntiNu && kIsVtxCont, kNoShift, wei);
    Spectrum fNueCCFromRock ("True neutrino Energy (GeV)", ana::Binning::Simple(400,0,20), loaderMC, kTrueNeutrinoE, 
			     !kIsVtxCont && !kIsAntiNu && kIsBeamNue, kNoShift, wei);


    //Antinumu in specified horn current broken into components
    Spectrum fAntiNumuSelCCFromPiPlus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,
					  kIsFromPiPlusDk && kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNumuSelCCFromPiMinus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,
					   kIsFromPiMinusDk && kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNumuSelCCFromKaPlus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
					  kIsFromKaPlusDk && kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNumuSelCCFromKaMinus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,
					   kIsFromKaMinusDk && kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNumuSelCCFromKaLong    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,
					  kIsFromKaLongDk  && kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNumuSelCCFromOtherTot ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
					 !kIsFromPiPlusDk && !kIsFromPiMinusDk && !kIsFromKaPlusDk
					 && !kIsFromKaMinusDk && !kIsFromKaLongDk && kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fAntiNumuSelCCFromRock ("True neutrino Energy (GeV)", ana::Binning::Simple(400,0,20), loaderMC, kTrueNeutrinoE, !kIsVtxCont && kIsAntiNu && kIsNumuCC, kNoShift, wei);


    //Numu in specified horn current broken into components
    Spectrum fNumuSelCCFromPiPlus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
				      kIsFromPiPlusDk  && !kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fNumuSelCCFromPiMinus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,
				       kIsFromPiMinusDk  && !kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fNumuSelCCFromKaPlus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,
				      kIsFromKaPlusDk  && !kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fNumuSelCCFromKaMinus    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,
				       kIsFromKaMinusDk  && !kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fNumuSelCCFromKaLong    ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE,
				      kIsFromKaLongDk  && !kIsAntiNu && kIsNumuCC && kIsVtxCont, kNoShift, wei);
    Spectrum fNumuSelCCFromOtherTot ("True Neutrino Energy (GeV)", ana::Binning::Simple(400, 0, 20), loaderMC, kTrueNeutrinoE, 
				     !kIsAntiNu && kIsNumuCC &&!kIsFromPiPlusDk && !kIsFromPiMinusDk 
				     && !kIsFromKaPlusDk && !kIsFromKaMinusDk && !kIsFromKaLongDk && kIsVtxCont, kNoShift, wei);
    Spectrum fNumuSelCCFromRock ("True neutrino Energy (GeV)", ana::Binning::Simple(400,0,20), loaderMC, kTrueNeutrinoE,
				 !kIsVtxCont && !kIsAntiNu && kIsNumuCC, kNoShift, wei);

    
    //====================== GO GO GO Fill Spectra ==========================
    loaderMC.Go();
    // loaderData.Go();
    
    



    //===================== Making the TH1s to save to root files ===========
    // double POTNumu = fData.POT(); //scale to data pot
	double POTNumu = 2.647 * std::pow(10,20);
	
    //michel stuff 
    // TH1D * hMichelsData = fMichelsData.ToTH1(POTNumu);
    // TH1D * hMichelsNC = fMichelsNC.ToTH1(POTNumu);
    // TH1D * hMichelsNumu = fMichelsNumu.ToTH1(POTNumu);
    // TH1D * hMichelsNue = fMichelsNue.ToTH1(POTNumu);
    // TH1D * hData = fData.ToTH1(POTNumu);

    //antinue
    TH1D * hAntiNueCCFromPiMinus = fAntiNueCCFromPiMinus.ToTH1(POTNumu);
    TH1D * hAntiNueCCFromPiPlus = fAntiNueCCFromPiPlus.ToTH1(POTNumu);
    TH1D * hAntiNueCCFromKaMinus = fAntiNueCCFromKaMinus.ToTH1(POTNumu);
    TH1D * hAntiNueCCFromKaPlus = fAntiNueCCFromKaPlus.ToTH1(POTNumu);
    TH1D * hAntiNueCCFromKaLong = fAntiNueCCFromKaLong.ToTH1(POTNumu);
    TH1D * hAntiNueCCFromOtherTot = fAntiNueCCFromOtherTot.ToTH1(POTNumu);
    TH1D * hAntiNueCCFromRock = fAntiNueCCFromRock.ToTH1(POTNumu);

    //nue
    TH1D * hNueCCFromPiMinus = fNueCCFromPiMinus.ToTH1(POTNumu);
    TH1D * hNueCCFromPiPlus = fNueCCFromPiPlus.ToTH1(POTNumu);
    TH1D * hNueCCFromKaMinus = fNueCCFromKaMinus.ToTH1(POTNumu);
    TH1D * hNueCCFromKaPlus = fNueCCFromKaPlus.ToTH1(POTNumu);
    TH1D * hNueCCFromKaLong = fNueCCFromKaLong.ToTH1(POTNumu);
    TH1D * hNueCCFromOtherTot = fNueCCFromOtherTot.ToTH1(POTNumu);
    TH1D * hNueCCFromRock = fNueCCFromRock.ToTH1(POTNumu);

    //antinumu
    TH1D * hAntiNumuSelCCFromPiPlus = fAntiNumuSelCCFromPiPlus.ToTH1(POTNumu);
    TH1D * hAntiNumuSelCCFromPiMinus = fAntiNumuSelCCFromPiMinus.ToTH1(POTNumu);
    TH1D * hAntiNumuSelCCFromKaPlus = fAntiNumuSelCCFromKaPlus.ToTH1(POTNumu);
    TH1D * hAntiNumuSelCCFromKaMinus = fAntiNumuSelCCFromKaMinus.ToTH1(POTNumu);
    TH1D * hAntiNumuSelCCFromKaLong = fAntiNumuSelCCFromKaLong.ToTH1(POTNumu);
    TH1D * hAntiNumuSelCCFromOtherTot = fAntiNumuSelCCFromOtherTot.ToTH1(POTNumu);
    TH1D * hAntiNumuSelCCFromRock = fAntiNumuSelCCFromRock.ToTH1(POTNumu);

    //numu
    TH1D * hNumuSelCCFromPiPlus = fNumuSelCCFromPiPlus.ToTH1(POTNumu);
    TH1D * hNumuSelCCFromPiMinus = fNumuSelCCFromPiMinus.ToTH1(POTNumu);
    TH1D * hNumuSelCCFromKaPlus = fNumuSelCCFromKaPlus.ToTH1(POTNumu);
    TH1D * hNumuSelCCFromKaMinus = fNumuSelCCFromKaMinus.ToTH1(POTNumu);
    TH1D * hNumuSelCCFromKaLong = fNumuSelCCFromKaLong.ToTH1(POTNumu);
    TH1D * hNumuSelCCFromOtherTot = fNumuSelCCFromOtherTot.ToTH1(POTNumu);
    TH1D * hNumuSelCCFromRock = fNumuSelCCFromRock.ToTH1(POTNumu);


    //========================== Saving to a root file =========================
    std::string filename = "fhc_true_energy_breakdown_hists_tot.root";
    TFile * file = new TFile(filename.c_str(),"Recreate");
    TDirectory * compsDir = file->mkdir("MCComponents");

    // std::string varName = "fData";
    // TDirectory * compDir = compsDir->mkdir(varName.c_str() );
    //fData.SaveTo(compDir);
    // varName = "hDataMichel";
    // compDir = compsDir->mkdir(varName.c_str() );
    //fMichelsData.SaveTo(compDir);


    //Michel
    // hData->Write("hData");
    /* hMichelsData->Write("hMichelsData");
    hMichelsNue->Write("hMichelsNue");
    hMichelsNumu->Write("hMichelsNumu");
    hMichelsNC->Write("hMichelsNC");*/

    //Nue
    hNueCCFromPiPlus->Write("hNueCCFromPiPlus");
    hNueCCFromPiMinus->Write("hNueCCFromPiMinus");
    hNueCCFromKaPlus->Write("hNueCCFromKaPlus");
    hNueCCFromKaMinus->Write("hNueCCFromKaMinus");
    hNueCCFromKaLong->Write("hNueCCFromKaLong");
    hNueCCFromOtherTot->Write("hNueCCFromOtherTot");
    hNueCCFromRock->Write("hNueCCFromRock");

    //Antinue
    hAntiNueCCFromKaPlus->Write("hAntiNueCCFromKaPlus");
    hAntiNueCCFromKaMinus->Write("hAntiNueCCFromKaMinus");
    hAntiNueCCFromKaLong->Write("hAntiNueCCFromKaLong");
    hAntiNueCCFromOtherTot->Write("hAntiNueCCFromOtherTot");
    hAntiNueCCFromPiPlus->Write("hAntiNueCCFromPiPlus");
    hAntiNueCCFromPiMinus->Write("hAntiNueCCFromPiMinus");
    hAntiNueCCFromRock->Write("hAntiNueCCFromRock");

    //Antinumu
    hAntiNumuSelCCFromPiPlus->Write("hAntiNumuSelCCFromPiPlus");
    hAntiNumuSelCCFromPiMinus->Write("hAntiNumuSelCCFromPiMinus");
    hAntiNumuSelCCFromKaPlus->Write("hAntiNumuSelCCFromKaPlus");
    hAntiNumuSelCCFromKaMinus->Write("hAntiNumuSelCCFromKaMinus");
    hAntiNumuSelCCFromKaLong->Write("hAntiNumuSelCCFromKaLong");
    hAntiNumuSelCCFromOtherTot->Write("hAntiNumuSelCCFromOtherTot");
    hAntiNumuSelCCFromRock->Write("hAntiNumuSelCCFromRock");

    //Numu
    hNumuSelCCFromPiPlus->Write("hNumuSelCCFromPiPlus");
    hNumuSelCCFromPiMinus->Write("hNumuSelCCFromPiMinus");
    hNumuSelCCFromKaPlus->Write("hNumuSelCCFromKaPlus");
    hNumuSelCCFromKaMinus->Write("hNumuSelCCFromKaMinus");
    hNumuSelCCFromKaLong->Write("hNumuSelCCFromKaLong");
    hNumuSelCCFromOtherTot->Write("hNumuSelCCFromOtherTot");
    hNumuSelCCFromRock->Write("hNumuSelCCFromRock");
    
    //save and close                                                                                                                                        
    file->Print();
    file->Close();
  }

  if(makePlots){
    
    TFile inF("fhc_true_energy_breakdown_hists_tot.root");

    //===================== Loading the TH1 and specs ========================

    // TH1D * iData;
    // inF.GetObject("hData", iData);
    // iData->SetDirectory(0);
    
    //AntiNue
    TH1D * iAntiNueTot;
    inF.GetObject("hAntiNueCCFromPiPlus", iAntiNueTot);
    iAntiNueTot->SetDirectory(0);

    TH1D * iAntiNueCCFromPiPlus;
    inF.GetObject("hAntiNueCCFromPiPlus", iAntiNueCCFromPiPlus);
    iAntiNueCCFromPiPlus->SetDirectory(0);

    TH1D * iAntiNueCCFromPiMinus;
    inF.GetObject("hAntiNueCCFromPiMinus", iAntiNueCCFromPiMinus);
    iAntiNueCCFromPiMinus->SetDirectory(0);

    TH1D * iAntiNueCCFromKaPlus;
    inF.GetObject("hAntiNueCCFromKaPlus", iAntiNueCCFromKaPlus);
    iAntiNueCCFromKaPlus->SetDirectory(0);

    TH1D * iAntiNueCCFromKaMinus;
    inF.GetObject("hAntiNueCCFromKaMinus", iAntiNueCCFromKaMinus);
    iAntiNueCCFromKaMinus->SetDirectory(0);

    TH1D * iAntiNueCCFromKaLong;
    inF.GetObject("hAntiNueCCFromKaLong", iAntiNueCCFromKaLong);
    iAntiNueCCFromKaLong->SetDirectory(0);

    TH1D * iAntiNueCCFromOtherTot;
    inF.GetObject("hAntiNueCCFromOtherTot", iAntiNueCCFromOtherTot);
    iAntiNueCCFromOtherTot->SetDirectory(0);

    TH1D * iAntiNueCCFromRock;
    inF.GetObject("hAntiNueCCFromRock",iAntiNueCCFromRock);
    iAntiNueCCFromRock->SetDirectory(0);



    //Nue
    TH1D * iNueTot;
    inF.GetObject("hNueCCFromPiPlus", iNueTot);
    iNueTot->SetDirectory(0);

    TH1D * iNueCCFromPiPlus;
    inF.GetObject("hNueCCFromPiPlus", iNueCCFromPiPlus);
    iNueCCFromPiPlus->SetDirectory(0);

    TH1D * iNueCCFromPiMinus;
    inF.GetObject("hNueCCFromPiMinus", iNueCCFromPiMinus);
    iNueCCFromPiMinus->SetDirectory(0);

    TH1D * iNueCCFromKaPlus;
    inF.GetObject("hNueCCFromKaPlus", iNueCCFromKaPlus);
    iNueCCFromKaPlus->SetDirectory(0);

    TH1D * iNueCCFromKaMinus;
    inF.GetObject("hNueCCFromKaMinus", iNueCCFromKaMinus);
    iNueCCFromKaMinus->SetDirectory(0);

    TH1D * iNueCCFromKaLong;
    inF.GetObject("hNueCCFromKaLong", iNueCCFromKaLong);
    iNueCCFromKaLong->SetDirectory(0);

    TH1D * iNueCCFromOtherTot;
    inF.GetObject("hNueCCFromOtherTot", iNueCCFromOtherTot);
    iNueCCFromOtherTot->SetDirectory(0);

    TH1D * iNueCCFromRock;
    inF.GetObject("hNueCCFromRock",iNueCCFromRock);
    iNueCCFromRock->SetDirectory(0);


    //Antinumu
    TH1D * iAntiNumuTot;
    inF.GetObject("hAntiNumuSelCCFromPiPlus", iAntiNumuTot);
    iAntiNumuTot->SetDirectory(0);

    TH1D * iAntiNumuSelCCFromPiPlus;
    inF.GetObject("hAntiNumuSelCCFromPiPlus", iAntiNumuSelCCFromPiPlus);
    iAntiNumuSelCCFromPiPlus->SetDirectory(0);


    TH1D * iAntiNumuSelCCFromPiMinus;
    inF.GetObject("hAntiNumuSelCCFromPiMinus", iAntiNumuSelCCFromPiMinus);
    iAntiNumuSelCCFromPiMinus->SetDirectory(0);


    TH1D * iAntiNumuSelCCFromKaPlus;
    inF.GetObject("hAntiNumuSelCCFromKaPlus", iAntiNumuSelCCFromKaPlus);
    iAntiNumuSelCCFromKaPlus->SetDirectory(0);


    TH1D * iAntiNumuSelCCFromKaMinus;
    inF.GetObject("hAntiNumuSelCCFromKaMinus", iAntiNumuSelCCFromKaMinus);
    iAntiNumuSelCCFromKaMinus->SetDirectory(0);

    TH1D * iAntiNumuSelCCFromKaLong;
    inF.GetObject("hAntiNumuSelCCFromKaLong", iAntiNumuSelCCFromKaLong);
    iAntiNumuSelCCFromKaLong->SetDirectory(0);

    TH1D * iAntiNumuSelCCFromOtherTot;
    inF.GetObject("hAntiNumuSelCCFromOtherTot", iAntiNumuSelCCFromOtherTot);
    iAntiNumuSelCCFromOtherTot->SetDirectory(0);


    TH1D * iAntiNumuSelCCFromRock;
    inF.GetObject("hAntiNumuSelCCFromRock",iAntiNumuSelCCFromRock);
    iAntiNumuSelCCFromRock->SetDirectory(0);



    //Numu
    TH1D * iNumuTot;
    inF.GetObject("hNumuSelCCFromPiPlus", iNumuTot);
    iNumuTot->SetDirectory(0);



    TH1D * iNumuSelCCFromPiPlus;
    inF.GetObject("hNumuSelCCFromPiPlus", iNumuSelCCFromPiPlus);
    iNumuSelCCFromPiPlus->SetDirectory(0);


    TH1D * iNumuSelCCFromPiMinus;
    inF.GetObject("hNumuSelCCFromPiMinus", iNumuSelCCFromPiMinus);
    iNumuSelCCFromPiMinus->SetDirectory(0);


    TH1D * iNumuSelCCFromKaPlus;
    inF.GetObject("hNumuSelCCFromKaPlus", iNumuSelCCFromKaPlus);
    iNumuSelCCFromKaPlus->SetDirectory(0);


    TH1D * iNumuSelCCFromKaMinus;
    inF.GetObject("hNumuSelCCFromKaMinus", iNumuSelCCFromKaMinus);
    iNumuSelCCFromKaMinus->SetDirectory(0);

    TH1D * iNumuSelCCFromKaLong;
    inF.GetObject("hNumuSelCCFromKaLong", iNumuSelCCFromKaLong);
    iNumuSelCCFromKaLong->SetDirectory(0);

    TH1D * iNumuSelCCFromOtherTot;
    inF.GetObject("hNumuSelCCFromOtherTot", iNumuSelCCFromOtherTot);
    iNumuSelCCFromOtherTot->SetDirectory(0);


    TH1D * iNumuSelCCFromRock;
    inF.GetObject("hNumuSelCCFromRock", iNumuSelCCFromRock);
    iNumuSelCCFromRock->SetDirectory(0);


    
    //Michel
    /* TH1D * iMichelsNumu;
    inF.GetObject("hMichelsNumu", iMichelsNumu);
    iMichelsNumu->SetDirectory(0);

    TH1D * iMichelsData;
    inF.GetObject("hMichelsData", iMichelsData);
    iMichelsData->SetDirectory(0);


    TH1D * iMichelsNue;
    inF.GetObject("hMichelsNue", iMichelsNue);
    iMichelsNue->SetDirectory(0);


    TH1D * iMichelsNC;
    inF.GetObject("hMichelsNC", iMichelsNC);
    iMichelsNC->SetDirectory(0);*/



    //====================Making the plots ==========================
    
    //Choose your own out_dir
    std::string OUT_DIR = "true_energy_plots";
    std::string fullpath = OUT_DIR + "/" + "True_Neutrino_Energy" + "_" + horn_current;
    auto c = std::make_unique<TCanvas> ();
    c->Clear();

    //TGaxis::SetMaxDigits(3);

    //Scale and create a total TH1 by summing all parts
    iAntiNueCCFromOtherTot->Scale(.001);
    iAntiNueCCFromPiPlus->Scale(.001);
    iAntiNueCCFromPiMinus->Scale(.001);
    iAntiNueCCFromKaMinus->Scale(.001);
    iAntiNueCCFromKaPlus->Scale(.001);
    iAntiNueCCFromKaLong->Scale(.001);
    iAntiNueTot->Scale(.001);
    iAntiNueCCFromRock->Scale(.001);
    iAntiNueTot->Add(iAntiNueCCFromPiMinus);
    iAntiNueTot->Add(iAntiNueCCFromKaMinus);
    iAntiNueTot->Add(iAntiNueCCFromKaPlus);
    iAntiNueTot->Add(iAntiNueCCFromKaLong);
    iAntiNueTot->Add(iAntiNueCCFromOtherTot);
    //iAntiNueTot->Add(iAntiNueCCFromRock);

    //Set the title and axis information. Set colors
    //iAntiNueTot->GetYaxis()->SetTitleOffset(.35);
    iAntiNueTot->GetXaxis()->SetTitle("True neutrino energy (GeV)");
    iAntiNueTot->GetYaxis()->SetTitle("10^{3} Events / 11.0 #times 10^{20} POT");
    iAntiNueCCFromPiMinus->SetLineColor(kRed-3);     iAntiNueCCFromPiMinus->SetLineStyle(2);
    iAntiNueCCFromPiPlus->SetLineColor(kRed-3);    
    iAntiNueCCFromKaMinus->SetLineStyle(2);     iAntiNueCCFromKaMinus->SetLineColor(kBlue);
    iAntiNueCCFromKaPlus->SetLineColor(kBlue);
    iAntiNueCCFromKaLong->SetLineColor(kGreen + 3);
    iAntiNueCCFromOtherTot->SetLineColor(kGray);
    iAntiNueCCFromRock->SetLineColor(kMagenta);

    //Draw 
    iAntiNueTot->Draw("HIST");
    iAntiNueCCFromPiMinus->Draw("HIST SAME");
    iAntiNueCCFromPiPlus->Draw("HIST SAME");
    iAntiNueCCFromKaMinus->Draw("HIST SAME");
    iAntiNueCCFromKaPlus->Draw("HIST SAME");
    iAntiNueCCFromKaLong->Draw("HIST SAME");
    iAntiNueCCFromOtherTot->Draw("HIST SAME");
    //iAntiNueCCFromRock->Draw("HIST SAME");
    

    double xmin = 0.6; double xmax = 0.8; double ymin= 0.5; double ymax = 0.7;
    if(!isRHC){
      xmin = 0.65; ymin = 0.55; xmax = 0.85; ymax = 0.85;
    }
    auto legend = new TLegend(xmin, ymin, xmax, ymax); 
    legend->AddEntry(iAntiNueCCFromPiMinus, "#bar{#nu}_{e} from #pi^{-}", "l");
    legend->AddEntry(iAntiNueCCFromPiPlus, "#bar{#nu}_{e} from #pi^{+}", "l");
    legend->AddEntry(iAntiNueCCFromKaMinus, "#bar{#nu}_{e} from K^{-}", "l");
    legend->AddEntry(iAntiNueCCFromKaPlus, "#bar{#nu}_{e} from K^{+}", "l");
    legend->AddEntry(iAntiNueCCFromKaLong, "#bar{#nu}_{e} from K^{0}_{L}", "l");
    legend->AddEntry(iAntiNueCCFromOtherTot, "#bar{#nu}_{e} from Other", "l");
    //legend->AddEntry(iAntiNueCCFromRock, "#bar{#nu}_{e} from Rock", "l");
    legend->AddEntry(iAntiNueTot, "#bar{#nu}_{e} Total", "l");
    //legend->SetTextSize(0.03);
    legend->Draw();
    Simulation();
    if(!isRHC) CornerLabels("#nu-beam");
    else CornerLabels("#bar{#nu}-beam");
    CenterTitles(iAntiNueTot); //Center the titles
    if (gSystem->AccessPathName(fullpath.c_str()))
      gSystem->mkdir(fullpath.c_str());
    util::SaveObj(c.get(), fullpath + "/" + "anti_nue");
	TFile* nuebar_file = new TFile("fhc_nuebar_histos.root", "RECREATE");
	iAntiNueTot->Write("nuebar_tot");
	iAntiNueCCFromPiPlus->Write("nuebar_from_pip");
	iAntiNueCCFromPiMinus->Write("nuebar_from_pim");
	iAntiNueCCFromKaMinus->Write("nuebar_from_km");
	iAntiNueCCFromKaPlus->Write("nuebar_from_kp");
	iAntiNueCCFromKaLong->Write("nuebar_from_kl");
	iAntiNueCCFromOtherTot->Write("nuebar_from_other");
    c->Clear();

    
    
    
    
    //Nue breakdowns
    
    iNueTot->GetXaxis()->SetTitle("True neutrino energy (GeV)");
    iNueTot->GetYaxis()->SetTitle("10^{3} Events / 11.0 #times 10^{20} POT");
    iNueCCFromPiMinus->SetLineColor(kRed -3);     iNueCCFromPiMinus->SetLineStyle(2);
    iNueCCFromPiPlus->SetLineColor(kRed-3);
    iNueCCFromKaMinus->SetLineColor(kBlue);     iNueCCFromKaMinus->SetLineStyle(2);
    iNueCCFromKaPlus->SetLineColor(kBlue);
    iNueCCFromKaLong->SetLineColor(kGreen + 3);
    iNueCCFromOtherTot->SetLineColor(kGray);
    iNueCCFromRock->SetLineColor(kMagenta);

    iNueTot->Add(iNueCCFromPiMinus);
    iNueTot->Add(iNueCCFromKaMinus);
    iNueTot->Add(iNueCCFromKaPlus);
    iNueTot->Add(iNueCCFromKaLong);
    iNueTot->Add(iNueCCFromOtherTot);
    //iNueTot->Add(iNueCCFromRock);
    
    iNueCCFromKaMinus->Scale(.001);
    iNueCCFromKaPlus->Scale(.001);
    iNueCCFromKaLong->Scale(.001);
    iNueCCFromOtherTot->Scale(.001);
    iNueCCFromPiMinus->Scale(.001);
    iNueTot->Scale(.001);
    iNueCCFromPiPlus->Scale(.001);
    iNueCCFromRock->Scale(.001);
    
    iNueTot->Draw("HIST");
    iNueCCFromPiMinus->Draw("HIST SAME");
    iNueCCFromPiPlus->Draw("HIST SAME");
    iNueCCFromKaMinus->Draw("HIST SAME");
    iNueCCFromKaPlus->Draw("HIST SAME");
    iNueCCFromKaLong->Draw("HIST SAME");
    iNueCCFromOtherTot->Draw("HIST SAME");
    //iNueCCFromRock->Draw("HIST SAME");
    

    xmin = 0.3; xmax = 0.55; ymin= 0.5; ymax = 0.7;
    if(!isRHC){
      xmin = 0.65; ymin= 0.55;xmax = 0.85; ymax = 0.85;
    }   
    auto legend2 = new TLegend(xmin, ymin, xmax,ymax);    
    legend2->AddEntry(iNueCCFromPiMinus, "#nu_{e} from #pi^{-}", "l");
    legend2->AddEntry(iNueCCFromPiPlus, "#nu_{e} from #pi^{+}", "l");
    legend2->AddEntry(iNueCCFromKaMinus, "#nu_{e} from K^{-}", "l");
    legend2->AddEntry(iNueCCFromKaPlus, "#nu_{e} from K^{+}", "l");
    legend2->AddEntry(iNueCCFromKaLong, "#nu_{e} from K^{0}_{L}", "l");
    legend2->AddEntry(iNueCCFromOtherTot, "#nu_{e} from Other", "l");
    //legend2->AddEntry(iNueCCFromRock, "#nu_{e} from Rock", "l");
    legend2->AddEntry(iNueTot, "#nu_{e} Total", "l");
    legend2->SetTextSize(0.04);
    legend2->Draw();
    CenterTitles(iNueTot);
    Simulation();
    if(!isRHC) CornerLabels("#nu-beam");   
    else CornerLabels("#bar{#nu}-beam");

    if (gSystem->AccessPathName(fullpath.c_str()))
      gSystem->mkdir(fullpath.c_str());
    util::SaveObj(c.get(), fullpath + "/" + "nue");
	TFile* nue_file = new TFile("fhc_nue_histos.root", "RECREATE");
	iNueTot->Write("nue_tot");
	iNueCCFromPiPlus->Write("nue_from_pip");
	iNueCCFromPiMinus->Write("nue_from_pim");
	iNueCCFromKaMinus->Write("nue_from_km");
	iNueCCFromKaPlus->Write("nue_from_kp");
	iNueCCFromKaLong->Write("nue_from_kl");
	iNueCCFromOtherTot->Write("nue_from_other");
    c->Clear();
    


    //numu tot
    iNumuTot->GetXaxis()->SetTitle("True neutrino energy (GeV)");
    iNumuTot->GetYaxis()->SetTitle("10^{5} Events / 11.0 #times 10^{20} POT");

    iNumuSelCCFromPiMinus->SetLineColor(kRed -3);
    iNumuSelCCFromPiMinus->SetLineStyle(2);
    iNumuSelCCFromPiPlus->SetLineColor(kRed-3);
    iNumuSelCCFromKaMinus->SetLineColor(kBlue);
    iNumuSelCCFromKaMinus->SetLineStyle(2);
    iNumuSelCCFromKaPlus->SetLineColor(kBlue);
    iNumuSelCCFromKaLong->SetLineColor(kGreen + 3);
    iNumuSelCCFromOtherTot->SetLineColor(kGray);
    iNumuSelCCFromRock->SetLineColor(kMagenta);

    
    iNumuTot->Add(iNumuSelCCFromPiMinus);
    iNumuTot->Add(iNumuSelCCFromPiPlus);
    iNumuTot->Add(iNumuSelCCFromKaMinus);
    iNumuTot->Add(iNumuSelCCFromKaPlus);
    iNumuTot->Add(iNumuSelCCFromOtherTot);
    iNumuTot->Add(iNumuSelCCFromKaLong);
    //iNumuTot->Add(iNumuSelCCFromRock);

    iNumuTot->Scale(.00001);
    iNumuSelCCFromPiPlus->Scale(.00001);
    iNumuSelCCFromKaMinus->Scale(.00001);
    iNumuSelCCFromKaPlus->Scale(.00001);
    iNumuSelCCFromOtherTot->Scale(.00001);
    iNumuSelCCFromKaLong->Scale(.00001);
    iNumuSelCCFromPiMinus->Scale(.00001);
    iNumuSelCCFromRock->Scale(.00001);

    iNumuTot->Draw("HIST");
    iNumuSelCCFromPiPlus->Draw("HIST SAME");
    iNumuSelCCFromPiMinus->Draw("HIST SAME");
    iNumuSelCCFromKaMinus->Draw("HIST SAME");
    iNumuSelCCFromKaPlus->Draw("HIST SAME");
    iNumuSelCCFromKaLong->Draw("HIST SAME");
    iNumuSelCCFromOtherTot->Draw("HIST SAME");
    //iNumuSelCCFromRock->Draw("HIST SAME");


    xmin = 0.4;  xmax = 0.7;  ymin= 0.4; ymax = 0.7;
    if(!isRHC){
      xmin = 0.65; ymin= 0.55;xmax = 0.85; ymax = 0.85;
    }   
    auto legend3 = new TLegend(xmin, ymin, xmax,ymax);
    legend3->AddEntry(iNumuSelCCFromPiMinus, "#nu_{#mu} from #pi^{-}", "l");
    legend3->AddEntry(iNumuSelCCFromPiPlus, "#nu_{#mu} from #pi^{+}", "l");
    legend3->AddEntry(iNumuSelCCFromKaMinus, "#nu_{#mu} from K^{-}", "l");
    legend3->AddEntry(iNumuSelCCFromKaPlus, "#nu_{#mu} from K^{+}", "l");
    legend3->AddEntry(iNumuSelCCFromKaLong, "#nu_{#mu} from K^{0}_{L}", "l");
    legend3->AddEntry(iNumuSelCCFromOtherTot, "#nu_{#mu} from Other", "l");
    //legend3->AddEntry(iNumuSelCCFromRock, "#nu_{#mu} from Rock", "l");
    legend3->AddEntry(iNumuTot, "#nu_{#mu} Total", "l");
    legend3->SetTextSize(0.04);
    legend3->Draw();
    CenterTitles(iNumuTot);
    Simulation();
    if(!isRHC) CornerLabels("#nu-beam");
    else CornerLabels("#bar{#nu}-beam");
    
    if (gSystem->AccessPathName(fullpath.c_str()))
      gSystem->mkdir(fullpath.c_str());
    util::SaveObj(c.get(), fullpath + "/" + "numu");
	TFile* numu_file = new TFile("fhc_numu_histos.root", "RECREATE");
	iNumuTot->Write("numu_tot");
	iNumuSelCCFromPiPlus->Write("numu_from_pip");
	iNumuSelCCFromPiMinus->Write("numu_from_pim");
	iNumuSelCCFromKaMinus->Write("numu_from_km");
	iNumuSelCCFromKaPlus->Write("numu_from_kp");
	iNumuSelCCFromKaLong->Write("numu_from_kl");
	iNumuSelCCFromOtherTot->Write("numu_from_other");
	numu_file->Close();
    c->Clear();

    
    //anti numu
    iAntiNumuTot->GetXaxis()->SetTitle("True neutrino energy (GeV)");
    iAntiNumuTot->GetYaxis()->SetTitle("10^{3} Events / 11.0 #times 10^{20} POT");
    
    iAntiNumuSelCCFromPiMinus->SetLineColor(kRed -3);
    iAntiNumuSelCCFromPiMinus->SetLineStyle(2);
    iAntiNumuSelCCFromPiPlus->SetLineColor(kRed -3);
    iAntiNumuSelCCFromKaMinus->SetLineColor(kBlue);
    iAntiNumuSelCCFromKaMinus->SetLineStyle(2);
    iAntiNumuSelCCFromKaPlus->SetLineColor(kBlue);
    iAntiNumuSelCCFromKaLong->SetLineColor(kGreen + 3);
    iAntiNumuSelCCFromOtherTot->SetLineColor(kGray);
    iAntiNumuSelCCFromRock->SetLineColor(kMagenta);
    
    iAntiNumuTot->Add(iAntiNumuSelCCFromPiMinus);
    iAntiNumuTot->Add(iAntiNumuSelCCFromKaLong);
    iAntiNumuTot->Add(iAntiNumuSelCCFromKaMinus);
    iAntiNumuTot->Add(iAntiNumuSelCCFromKaPlus);
    iAntiNumuTot->Add(iAntiNumuSelCCFromOtherTot);
    //iAntiNumuTot->Add(iAntiNumuSelCCFromRock);
    
    iAntiNumuTot->Scale(.001);
    iAntiNumuSelCCFromPiMinus->Scale(.001);
    iAntiNumuSelCCFromPiPlus->Scale(.001);
    iAntiNumuSelCCFromKaLong->Scale(.001);
    iAntiNumuSelCCFromKaPlus->Scale(.001);
    iAntiNumuSelCCFromKaMinus->Scale(.001);
    iAntiNumuSelCCFromOtherTot->Scale(.001);
    iAntiNumuSelCCFromRock->Scale(.001);

    iAntiNumuTot->Draw("HIST");
    iAntiNumuSelCCFromPiMinus->Draw("HIST SAME");
    iAntiNumuSelCCFromPiPlus->Draw("HIST SAME");
    iAntiNumuSelCCFromKaMinus->Draw("HIST SAME");
    iAntiNumuSelCCFromKaPlus->Draw("HIST SAME");
    iAntiNumuSelCCFromKaLong->Draw("HIST SAME");
    iAntiNumuSelCCFromOtherTot->Draw("HIST SAME");
    // iAntiNumuSelCCFromRock->Draw("HIST SAME");

    xmin = 0.4; xmax = 0.8; ymin= 0.4; ymax = 0.7;
    if(!isRHC){
      xmin = 0.65; ymin= 0.55;xmax = 0.85; ymax = 0.85;
    }   
    auto legend4 = new TLegend(xmin, ymin, xmax,ymax);
    legend4->AddEntry(iAntiNumuSelCCFromPiMinus, "#bar{#nu}_{#mu} from #pi^{-}", "l");
    legend4->AddEntry(iAntiNumuSelCCFromPiPlus, "#bar{#nu}_{#mu} from #pi^{+}", "l");
    legend4->AddEntry(iAntiNumuSelCCFromKaMinus, "#bar{#nu}_{#mu} from K^{-}", "l");
    legend4->AddEntry(iAntiNumuSelCCFromKaPlus, "#bar{#nu}_{#mu} from K^{+}", "l");
    legend4->AddEntry(iAntiNumuSelCCFromKaLong, "#bar{#nu}_{#mu} from K^{0}_{L}", "l");
    legend4->AddEntry(iAntiNumuSelCCFromOtherTot, "#bar{#nu}_{#mu} from Other", "l");
    //legend4->AddEntry(iAntiNumuSelCCFromRock, "#bar{#nu}_{#mu} from Rock", "l");
    legend4->AddEntry(iAntiNumuTot, "#bar{#nu}_{#mu} Total", "l");
    legend4->SetTextSize(0.04);
    legend4->Draw();
    CenterTitles(iAntiNumuTot);
    Simulation();
    if(!isRHC) CornerLabels("#nu-beam");
    else CornerLabels("#bar{#nu}-beam");

    if (gSystem->AccessPathName(fullpath.c_str()))
      gSystem->mkdir(fullpath.c_str());
    util::SaveObj(c.get(), fullpath + "/" + "anti_numu");
    TFile* numubar_file = new TFile("fhc_numubar_histos.root", "RECREATE");
	iAntiNumuTot->Write("numubar_tot");
	iAntiNumuSelCCFromPiPlus->Write("numubar_from_pip");
	iAntiNumuSelCCFromPiMinus->Write("numubar_from_pim");
	iAntiNumuSelCCFromKaMinus->Write("numubar_from_km");
	iAntiNumuSelCCFromKaPlus->Write("numubar_from_kp");
	iAntiNumuSelCCFromKaLong->Write("numubar_from_kl");
	iAntiNumuSelCCFromOtherTot->Write("numubar_from_other");
    
    c->Clear();
    /* auto legend5 = new TLegend(0.5, 0.5, .8, .8);
    legend5->AddEntry(iMichelsData, "Data", "l");
    legend5->AddEntry(iMichelsNC, "NC", "l");
    legend5->AddEntry(iMichelsNumu, "#nu_{#mu} CC", "l");
    legend5->AddEntry(iMichelsNue, "#nu_{e} CC", "l");
    legend5->SetTextSize(0.04);

    iMichelsData->GetYaxis()->SetTitle("10^{3} Events / 11.0 #times 10^{20} POT");
    iMichelsData->GetXaxis()->SetTitle("Number of michel electrons");
    
    iMichelsData->Scale(.001);
    iMichelsNC->Scale(.001);
    iMichelsNumu->Scale(.001);
    iMichelsNue->Scale(.001);
    
    iMichelsNue->SetLineColor(kViolet-5);
    iMichelsNC->SetLineColor(kAzure);
    iMichelsNumu->SetLineColor(kRed-3);
    iMichelsData->SetMarkerStyle(9);
    iMichelsData->Draw("HIST E0");
    iMichelsNC->Draw("HIST SAME");
    iMichelsNumu->Draw("HIST SAME");
    iMichelsNue->Draw("HIST SAME");

    legend5->Draw();
    CenterTitles(iMichelsData);
    Preliminary();
    CornerLabels("#nu-beam");
    
    fullpath = OUT_DIR + "/" + "NMichels" + "_" + horn_current;*/
   

    /* if (gSystem->AccessPathName(fullpath.c_str()))
      gSystem->mkdir(fullpath.c_str());
    util::SaveObj(c.get(), fullpath + "/" + "anti_numu");*/

  }//end if make plots



} //end macro
