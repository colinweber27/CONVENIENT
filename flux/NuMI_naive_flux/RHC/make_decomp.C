// Macro to generate 20202 nue FHC analysis root file for Michel/BEN and Prop decomps/plots

#include "CAFAna/Vars/Vars.h"
#include "3FlavorAna/Vars/NueVars.h"
#include "CAFAna/Weights/GenieWeights.h"
#include "CAFAna/Weights/PPFXWeights.h"
#include "CAFAna/Weights/XsecTunes.h"

#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "3FlavorAna/Cuts/NumuCuts.h"
#include "3FlavorAna/Cuts/NueCuts2020.h"
#include "CAFAna/Cuts/TruthCuts.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"


#include "3FlavorAna/Decomp/MichelDecomp.h"
#include "3FlavorAna/Decomp/BENDecomp.h"
#include "CAFAna/Decomp/ProportionalDecomp.h"
#include "CAFAna/Decomp/CheatDecomp.h"
#include "3FlavorAna/Decomp/NumuDecomp.h"
#include "CAFAna/Extrap/ModularExtrap.h"
#include "CAFAna/Prediction/PredictionExtrap.h"
#include "CAFAna/Prediction/PredictionNoExtrap.h"

#include "CAFAna/Systs/Systs.h"
#include "CAFAna/Core/SystShifts.h"
#include "3FlavorAna/Cuts/NumuCuts2020.h"




#include "Utilities/func/MathUtil.h"

#include "TCanvas.h"
#include "TH2.h"
#include "TFile.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TRandom3.h"
#include "3FlavorAna/Vars/NumuVars.h"

#include <iostream>
#include <cmath>

using namespace ana;

void make_decomp()
{  
  HistAxis NumuAxis("CCE", Binning::Simple(50,0,5), kCCE);
  


  //===========Prod5 Defs and weights===============================================

  const Weight Wei = kXSecCVWgt2020* kPPFXFluxCVWgt;


  //FHC 
  //SpectrumLoader lNDData("defname: prod_caf_R19-11-18-prod5reco.d.f.h.l_nd_numi_fhc_full_v1_goodruns");
  //SpectrumLoader lNDMC("defname: prod_caf_R19-11-18-prod5reco.d.h.l_nd_genie_N1810j0211a_nonswap_fhc_nova_v08_full_v1");

  //FHC Concats 
  SpectrumLoader lNDData("defname: prod_sumdecaf_R19-11-18-prod5reco.d.f.h.l_nd_numi_fhc_full_v1_goodruns_nue2020");
  SpectrumLoader lNDMC("defname: prod_sumdecaf_R19-11-18-prod5reco.d.h.l_nd_genie_N1810j0211a_nonswap_fhc_nova_v08_full_v1_nue2020");
 
  lNDData.SetSpillCut(kStandardSpillCuts);
  lNDMC.SetSpillCut(kStandardSpillCuts);
  
  
  struct HistDef
  {
      std::string name;
      HistAxis axis;
  };

  const HistDef def = {"cvn2d", {"CVN 2D", kNue2020Binning, kNue2020AnaBin}};

  Cut NueND = kNue2020ND;





  IDecomp*            cheatDCMP; 
  IDecomp*            michelDCMP;
  IDecomp*            michelbenDCMP; 
  BENDecomp*          benDCMP;
  BENDecomp*          benDCMPFix; 
  ProportionalDecomp* propDCMP;
  IDecomp*            michelpropDCMP;

  const HistAxis& axis = def.axis;
  
  // Michel decomp with decomp as input
  cheatDCMP = new CheatDecomp(lNDMC, axis, NueND, kNoShift, Wei);
  michelDCMP = new MichelDecomp(lNDMC, lNDData, axis, NueND, cheatDCMP, kNoShift, kNoShift, Wei);
      
  // COMBO Decomp = Michel+BEN decomp. This is used with FHC for the analysis. Others are used for studies/verifications.
  benDCMPFix = new BENDecomp(lNDMC, lNDData, axis, NueND, kNoShift, kNoShift, Wei);
  benDCMP = new BENDecomp (BENDecomp::kCalcKaonScale, lNDMC, lNDData,kNue2020Axis, NueND, kNoShift, kNoShift, Wei);
  michelbenDCMP = new MichelDecomp(lNDMC, lNDData, axis, NueND, benDCMP, kNoShift, kNoShift, Wei);
  
  // Proportional decomp - to be used with RHC for the analysis
  propDCMP = new ProportionalDecomp(lNDMC, lNDData, axis, NueND, kNoShift, kNoShift, Wei);
  michelpropDCMP = new MichelDecomp(lNDMC, lNDData, axis, NueND, propDCMP, kNoShift, kNoShift, Wei);
  

  lNDMC.Go();
  lNDData.Go();
  
  TFile* file = new TFile("nue_dcmp.root","recreate");
  TDirectory* d = file->mkdir("nue_decomps");
  
  michelDCMP->SaveTo(d, "michelcheatDCMP");
  michelbenDCMP->SaveTo(d, "michelbenDCMP");
  propDCMP->SaveTo(d, "propDCMP");
  benDCMPFix->SaveTo(d, "benDCMPFix");
  benDCMP->SaveTo(d, "benDCMP");
  michelpropDCMP->SaveTo(d, "michelpropDCMP");
  file->Close();  
}
