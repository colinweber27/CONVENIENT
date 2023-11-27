// Macro to draw 2020 nue FHC analysis MichelDecomp plot

#include "OscLib/OscCalcDumb.h"

#include "CAFAna/Vars/Vars.h"
#include "3FlavorAna/Vars/NueVars.h"
#include "CAFAna/Weights/GenieWeights.h"

#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "3FlavorAna/Cuts/NumuCuts.h"
#include "CAFAna/Cuts/TruthCuts.h"
#include "CAFAna/Cuts/NueCutsSecondAna.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Core/LoadFromFile.h"

#include "CAFAna/Analysis/Style.h"
#include "CAFAna/Analysis/SALoaders.h"
#include "3FlavorAna/Plotting/NuePlotStyle.h"
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

#include "Utilities/func/MathUtil.h"

#include "TROOT.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TFile.h"
#include "TPad.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TRandom3.h"
#include "THStack.h"
#include "TGaxis.h"
#include "TLine.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TGraph.h"

#include "TColor.h"


#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Utilities/rootlogon.C"

using namespace ana;

void FormatRatio(TH1 *hrat, std::string xtitle, std::string ytitle)
{
  gPad->SetTopMargin(0);
  gPad->SetBottomMargin(0.2);
  hrat->GetXaxis()->SetTitleSize(0.10);
  hrat->GetXaxis()->SetLabelSize(0.08);
  hrat->GetXaxis()->SetTitleOffset(0.8);
  hrat->GetYaxis()->SetTitleSize(0.10);
  hrat->GetYaxis()->SetLabelSize(0.08);
  hrat->GetYaxis()->SetTitleOffset(0.5);
  hrat->SetTitle("");
  hrat->GetYaxis()->SetRangeUser(0.6501,1.3499);
  hrat->GetXaxis()->SetTitle(xtitle.c_str());
  hrat->GetYaxis()->SetTitle(ytitle.c_str());
  hrat->GetYaxis()->CenterTitle();
  hrat->GetXaxis()->CenterTitle();
}

void CornerLabels(std::string Str) {
  TLatex* CornLab = new TLatex(.1, .93, Str.c_str());
  CornLab->SetTextColor(kGray+1);
  CornLab->SetNDC();
  CornLab->SetTextSize (1.5/30.);
  CornLab->SetTextAlign(11);
  CornLab->Draw();
}


void MakeStackNME(std::vector<const TH2F*> hs, double potrat, int idx1, int idx2,
                  std::vector<const TH1F*> scales,
                  std::string xtitle, std::string ytitle,
                  std::string yrattitle, double &tot, bool applyScales = false)
{
  TCanvas *can = new TCanvas(UniqueName().c_str(),"",1500,700);
  can->Divide(3,1);


  TH1D *hnm = (TH1D*)(hs[0]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());
  TH1D *hnc = (TH1D*)(hs[1]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());
  TH1D *hne = (TH1D*)(hs[2]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());
  TH1D *hda = (TH1D*)(hs[3]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());

  TH1D *g0 = (TH1D*)(hs[0]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());
  TH1D *g1 = (TH1D*)(hs[1]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());
  TH1D *g2 = (TH1D*)(hs[2]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());

  TH1D *f0 = (TH1D*)(hs[0]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());
  TH1D *f1 = (TH1D*)(hs[1]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());
  TH1D *f2 = (TH1D*)(hs[2]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());


  hnm->Scale(potrat*1e-3);
  hnc->Scale(potrat*1e-3);
  hne->Scale(potrat*1e-3);

  g0->Scale(potrat*1e-3);
  g1->Scale(potrat*1e-3);
  g2->Scale(potrat*1e-3);

  f0->Scale(potrat*1e-3);
  f1->Scale(potrat*1e-3);
  f2->Scale(potrat*1e-3);
  

  for (int i = 1; i <= hda->GetNbinsX(); i++){
    hda->SetBinContent(i,0.001*hda->GetBinContent(i));
    hda->SetBinError  (i,0.001*hda->GetBinError(i));
  }


  TH1D *hmc = (TH1D*)(hs[0]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());
  hmc->Scale(potrat*1e-3);
  hmc->Add(hnc);
  hmc->Add(hne);

  TH1D *fmc = (TH1D*)(hs[0]->ProjectionY(UniqueName().c_str(),idx1,idx2))->Clone(UniqueName().c_str());
  fmc->Scale(potrat*1e-3);

  hnm->SetFillColor(kRed -3);
  hnc->SetFillColor(kAzure);
  hne->SetFillColor(kPink+9);
  hda->SetMinimum(1e-3);

  int EIdx = idx1%10;
  std::string ERange = "1.0 < E_{#nu} < 1.5 GeV";
  if (EIdx == 1) ERange = "0.0 < E_{#nu} < 0.5 GeV";
  if (EIdx == 2) ERange = "0.5 < E_{#nu} < 1.0 GeV";
  if (EIdx == 3) ERange = "1.0 < E_{#nu} < 1.5 GeV";
  if (EIdx == 4) ERange = "1.5 < E_{#nu} < 2.0 GeV";
  if (EIdx == 5) ERange = "2.0 < E_{#nu} < 2.5 GeV";
  if (EIdx == 6) ERange = "2.5 < E_{#nu} < 3.0 GeV";
  if (EIdx == 7) ERange = "3.0 < E_{#nu} < 3.5 GeV";
  if (EIdx == 8) ERange = "3.5 < E_{#nu} < 4.0 GeV";
  if (EIdx == 9) ERange = "4.0 < E_{#nu} < 4.5 GeV";
  
  ERange += " and ";

  int CIdx = idx1/10;
  std::string CRange = "0.84 <= CVN < 0.96";
  if (CIdx == 1) CRange = "CVN >= 0.96";

  hda->SetTitle((ERange + CRange).c_str());
  hmc->GetYaxis()->SetTitleSize(0.04);
  hmc->GetYaxis()->SetTitleOffset(1.4);
  hda->GetYaxis()->SetTitleSize(0.04);
  hda->GetYaxis()->SetTitleOffset(1.4);


  THStack *st = new THStack();
  st->Add(hne);
  st->Add(hnc);
  st->Add(hnm);

  can->cd(1);


  double max = std::max(hmc->GetMaximum(),hda->GetMaximum());
  hmc->GetYaxis()->SetRangeUser(1e-3,1.2*max);
  hda->GetYaxis()->SetRangeUser(1e-3,1.2*max);

  hmc->GetYaxis()->SetTitle("10^{3}Events / 11.0 #times 10^{20} POT");
  hmc->GetYaxis()->CenterTitle();
  hda->GetYaxis()->SetTitle("10^{3}Events / 11.0 #times 10^{20} POT");
  hda->GetYaxis()->CenterTitle();

  g0->Divide(hmc);
  g1->Divide(hmc);
  g2->Divide(hmc);

  g0->SetFillColor(kRed -3);
  g1->SetFillColor(kAzure);
  g2->SetFillColor(kPink+9);

  THStack *stg = new THStack();
  stg->Add(g2);
  stg->Add(g1);
  stg->Add(g0);

  g0->GetXaxis()->SetTitle("N_{ME}");
  g0->GetXaxis()->CenterTitle();
  g0->GetYaxis()->SetTitle("Fraction of Events");
  g0->GetYaxis()->CenterTitle();
  g0->GetYaxis()->SetTitleOffset(1.2);
  gPad->SetLeftMargin(0.13);

  g0->GetYaxis()->SetRangeUser(0,1);
  g0->Draw("hist");
  stg->Draw("hist,same");

  can->cd(2);

  TPad *pRat2 = new TPad(UniqueName().c_str(),"",0,0,1,0.32);
  TPad *pSpec2= new TPad(UniqueName().c_str(),"",0,0.32,1,1);

  pRat2->Draw();
  pSpec2->Draw();

  pSpec2->cd();
  gPad->SetBottomMargin(0);

  hda->GetXaxis()->SetTitle("");

  hda->Draw("E");
  st->Draw("hist,same");
  hda->Draw("E,same");

  TLegend *leg = new TLegend(0.6,0.55,0.8,0.8);
  leg->AddEntry(hda,"Data","el");
  leg->AddEntry(hnm,"#nu_{#mu} CC","f");
  leg->AddEntry(hnc,"NC","f");
  leg->AddEntry(hne,"#nu_{e} CC","f");
  leg->Draw();

  pRat2->cd();

  TH1F *hrat = (TH1F*)hda->Clone(UniqueName().c_str());
  hrat->Divide(hmc);

  FormatRatio(hrat,"N_{ME}","Ratio");

  for (int i = 1; i <=3; i++)
    hrat->SetBinError(i,1./sqrt(1000*hda->GetBinContent(i)));
    //hrat->SetBinError(i,1./sqrt(hda->GetBinContent(i)));
  hrat->Draw("E");

  TLine *l = new TLine(0,1,3,1);
  l->SetLineStyle(7);
  l->Draw("same");

  can->cd(3);

  TPad *pRat3 = new TPad(UniqueName().c_str(),"",0,0,1,0.32);
  TPad *pSpec3= new TPad(UniqueName().c_str(),"",0,0.32,1,1);

  pRat3->Draw();
  pSpec3->Draw();

  pSpec3->cd();
  gPad->SetBottomMargin(0);
  
  f0->SetFillColor(kRed -3);
  f1->SetFillColor(kAzure);
  f2->SetFillColor(kPink+9);

  f0->Scale(scales[0]->GetBinContent(idx1));
  f1->Scale(scales[1]->GetBinContent(idx1));
  f2->Scale(scales[2]->GetBinContent(idx1));

  fmc->Scale(scales[0]->GetBinContent(idx1));
  fmc->Add(f1);
  fmc->Add(f2);

  THStack *st3 = new THStack();
  st3->Add(f2);
  st3->Add(f1);
  st3->Add(f0);

  TH1F *hda2 = (TH1F*)hda->Clone(UniqueName().c_str());
  hda2->SetTitle("");

  hda2->Draw("E");
  st3->Draw("hist,same");
  hda2->Draw("E,same");

  TH1F *frat = (TH1F*)hda2->Clone(UniqueName().c_str());
  frat->Divide(fmc);

  double chisq = 0;
  for (int i = 1; i <=3; i++){
    frat->SetBinError(i,1./sqrt(1000*hda->GetBinContent(i)));
    chisq += util::sqr(fmc->GetBinContent(i)-hda->GetBinContent(i))/
      hda->GetBinContent(i);
  }
  chisq *= 1000; 
  tot += chisq;
  std::cout << chisq << std::endl;

  TLatex latex;
  latex.SetTextSize(0.09);

  std::ostringstream out;
  out << std::setprecision(3) << chisq;

  latex.DrawLatexNDC(0.46,0.65,("#chi^{2} = "+out.str()).c_str());

  pRat3->cd();

  FormatRatio(frat,"N_{ME}","Ratio");

  frat->Draw("E");
  l->Draw("same");

  can->SaveAs(("Fig_AnaBin_"+std::to_string(idx1)+".png").c_str());
}

void MakeFracPlot(std::vector<const TH1F*> hs)
{
  TH1F *f0 = (TH1F*)hs[0]->Clone(UniqueName().c_str());
  TH1F *f1 = (TH1F*)hs[1]->Clone(UniqueName().c_str());
  TH1F *f2 = (TH1F*)hs[2]->Clone(UniqueName().c_str());
  TH1F *sum= (TH1F*)hs[2]->Clone(UniqueName().c_str());
  sum->Add(f1);
  sum->Add(f0);

  f0->Divide(sum);
  f1->Divide(sum);
  f2->Divide(sum);

  f0->SetFillColor(kRed -3);
  f1->SetFillColor(kAzure);
  f2->SetFillColor(kPink+9);


  TCanvas *can = new TCanvas();

  TH2F *h = new TH2F(UniqueName().c_str(),";Reconstructed neutrino energy (GeV);Fraction of Events",1,0,9.5,1,0,1);

  h->Draw();

  THStack *st = new THStack();
  st->Add(f2);
  st->Add(f1);
  st->Add(f0);
  st->Draw("hist same");

  Nue2018ThreeBinAxis(h,true,true,true);

  TLine *l = new TLine(0,0.8,19,0.8);
  l->Draw("same");

  can->SaveAs("Fracs.png");
}

void MakeRawPlot(std::vector<const TH1F*> hs, double potrat)
{
  TH1F *hrat = (TH1F*)hs[3]->Clone(UniqueName().c_str());
  TH1F *hmc  = (TH1F*)hs[0]->Clone(UniqueName().c_str());
  TH1F *hda  = (TH1F*)hs[3]->Clone(UniqueName().c_str()); 
  
  TH1F *hs0 = (TH1F*)hs[0]->Clone(UniqueName().c_str());
  TH1F *hs1 = (TH1F*)hs[1]->Clone(UniqueName().c_str());
  TH1F *hs2 = (TH1F*)hs[2]->Clone(UniqueName().c_str());
  
  hmc->Add(hs1);
  hmc->Add(hs2);
  hmc->Scale(potrat*1e-3);
  hrat->Scale(1e-3);
  hrat->Divide(hmc);

  hs0->Scale(potrat*1e-3);
  hs1->Scale(potrat*1e-3);
  hs2->Scale(potrat*1e-3);

  hs0->SetFillColor(kRed -3);
  hs1->SetFillColor(kAzure);
  hs2->SetFillColor(kPink+9);
  
  TCanvas *can = new TCanvas();

  TPad *pRat1 = new TPad("rat1","",0,0.05,1,0.35);
  TPad *pSpec1= new TPad("spec1","",0,0.35,1,1);
  
  pRat1->Draw();
  pSpec1->Draw();

  pSpec1->cd();
  gPad->SetBottomMargin(0);

  TH2F *h = new TH2F(UniqueName().c_str(),";Reconstructed neutrino energy (GeV);10^{3} Events",1,0,9.5,20000,1e-3,4.8);
  h->GetYaxis()->CenterTitle();
  h->GetXaxis()->CenterTitle();
  h->SetMinimum(1e-3);
  h->GetYaxis()->SetLabelSize(0.06);
  h->GetYaxis()->SetTitleOffset(0.6);
  h->GetYaxis()->SetTitleSize(0.07);
  h->SetMaximum(6);
  h->Draw();

  Nue2018ThreeBinAxis(h,true,true,true);
  Nue2018ThreeBinLabels(0.95,0.08,kGray+3,true);
  //  h->GetXaxis()->SetTitle("Reconstructed neutrino energy (GeV)");
  h->Draw("same");

  
  Nue2018ThreeBinAxis(h,true,true,true);
  Nue2018ThreeBinLabels(0.95,0.08,kGray+3,true);

  THStack *st = new THStack();
  st->Add(hs2);
  st->Add(hs1);
  st->Add(hs0);
  
  st->Draw("hist,same");
  hda->SetMarkerStyle(8);
  hda->Scale(1e-3);
  hda->Draw("E,same");


  CenterTitles(hda);
  Preliminary();
  CornerLabel("#nu-beam");
  TLegend *leg2 = new TLegend(0.3,0.47,0.6,0.77);
  leg2->AddEntry(hs2,"Beam #nu_{e} CC","f");
  leg2->AddEntry(hs1,"NC","f");
  leg2->AddEntry(hs0,"#nu_{#mu} CC","f");
  leg2->AddEntry(hda, "Data","le");
  leg2->Draw();

  pRat1->cd();
  gPad->SetTopMargin(0);
  gPad->SetBottomMargin(0.2);

  hrat->GetXaxis()->SetTitleSize(0.18);
  hrat->GetXaxis()->SetLabelSize(0.14);
  hrat->GetXaxis()->SetTitleOffset(.75);
  hrat->GetXaxis()->SetRangeUser(0,20);

  hrat->GetYaxis()->SetTitleSize(0.15);
  hrat->GetYaxis()->SetLabelSize(0.12);
  hrat->GetYaxis()->SetTitleOffset(0.25);
  hrat->SetLineColor(kBlack);
  hrat->SetMaximum(1.25001);
  hrat->SetMinimum(0.74999);
  hrat->GetYaxis()->SetTitle("Data/MC");
  hrat->GetXaxis()->SetTitle("Reconstructed neutrino energy (GeV)");
  hrat->GetYaxis()->CenterTitle();
  hrat->GetXaxis()->CenterTitle();
  hrat->Draw("E");
  
  Nue2018ThreeBinAxis(hrat,true,true,true);
  std::cout << "Number of events Uncorr." << endl;
  std::cout << "Data: " << hda->Integral() << endl;
  std::cout << "Numu: " << hs0->Integral() << endl;
  std::cout << "NC: " << hs1->Integral() << endl;
  std::cout << "Beam: " << hs2->Integral() << endl;

  TLine *l = new TLine(0,1,19,1);
  l->SetLineStyle(7);
  l->Draw("same");

  can->SaveAs("RawDaMC.png");
  can->SaveAs("RawDaMC.pdf");
}

void MakeScalesPlot(std::vector<const TH1F*> hs)
{
  TH1F *hs0 = (TH1F*)hs[0]->Clone(UniqueName().c_str());
  TH1F *hs1 = (TH1F*)hs[1]->Clone(UniqueName().c_str());
  TH1F *hs2 = (TH1F*)hs[2]->Clone(UniqueName().c_str());
  
  hs0->SetFillColor(kRed -3);
  hs1->SetLineColor(kAzure);
  hs2->SetLineColor(kPink+9);

  hs0->GetYaxis()->SetRangeUser(0,2);
  hs0->GetXaxis()->SetRangeUser(0,20);
  hs0->GetXaxis()->CenterTitle();
  hs0->GetXaxis()->SetTitle("Reconstructed neutrino energy (GeV)");
  hs0->GetYaxis()->SetTitle("Decomp Scale Factor");
  hs0->GetYaxis()->CenterTitle();

  TCanvas *can = new TCanvas();
  hs0->Draw();
  hs1->Draw("same");
  hs2->Draw("same");

  TLegend *leg = new TLegend(0.55,0.64,0.66,0.87);
  leg->AddEntry(hs0,"#nu_{#mu} CC","le");
  leg->AddEntry(hs1,"NC","le");
  leg->AddEntry(hs2,"#nu_{e} CC","le");
  leg->Draw();

  TLine *l1 = new TLine(1,0,1,2);
  l1->Draw("same");
  TLine *l2 = new TLine(6,0,6,2);
  l2->Draw("same");
  
  can->SaveAs("DecompScales.png");
}

void MakeResultPlot(MichelDecomp *m, TH1 *hda, std::vector<const TH1F*> ht, std::vector<const TH1F*> hs, double pot, double potrat)
{
  TCanvas *can = new TCanvas();

  TH1F *ht0 = (TH1F*)ht[0]->Clone(UniqueName().c_str());
  TH1F *ht1 = (TH1F*)ht[1]->Clone(UniqueName().c_str());
  TH1F *ht2 = (TH1F*)ht[2]->Clone(UniqueName().c_str());
  
  m->NumuComponent();
  TH1* estNM=(m->NumuComponent() + m->AntiNumuComponent()).ToTH1(pot);
  TH1* estNE=(m->NueComponent()  + m->AntiNueComponent()).ToTH1(pot);
  TH1* estNC= m->NCTotalComponent().ToTH1(pot);

  TH1F *ratNM = (TH1F*)hs[0]->Clone(UniqueName().c_str());
  TH1F *ratNC = (TH1F*)hs[1]->Clone(UniqueName().c_str());
  TH1F *ratNE = (TH1F*)hs[2]->Clone(UniqueName().c_str());

  TH1* mcNM=(m->MC_NumuComponent()+m->MC_AntiNumuComponent()).ToTH1(pot);
  TH1* mcNE=(m->MC_NueComponent() +m->MC_AntiNueComponent()).ToTH1(pot);
  TH1* mcNC= m->MC_NCTotalComponent().ToTH1(pot);

  for (int i = 1; i <= hda->GetNbinsX(); i++){
    hda->SetBinContent(i,1e-3*hda->GetBinContent(i));
    hda->SetBinError(i,1e-3*hda->GetBinError(i));
  }

  mcNM->Scale(1e-3);
  mcNE->Scale(1e-3);
  mcNC->Scale(1e-3);
  estNM->Scale(1e-3);
  estNE->Scale(1e-3);
  estNC->Scale(1e-3);
  
  TH1* nn2 = (TH1*)estNE->Clone();
  
  TH1* nn1 = (TH1*)estNE->Clone();
  nn1->Add(estNC);
  
  TH1* nn0 = (TH1*)estNE->Clone();
  nn0->Add(estNC);
  nn0->Add(estNM);
  
  ht0->Scale(potrat*1e-3);
  ht1->Scale(potrat*1e-3);
  ht2->Scale(potrat*1e-3);

  
  estNM->SetFillColor(kRed-3);
  estNC->SetFillColor(kAzure);
  estNE->SetFillColor(kPink+9);  
  ratNM->SetLineColor(kRed-3);
  ratNC->SetLineColor(kAzure);
  ratNE->SetLineColor(kPink+9);
  ratNM->SetMarkerColor(kRed-3);
  ratNC->SetMarkerColor(kAzure);
  ratNE->SetMarkerColor(kPink+9);

  ht0->SetLineStyle(2);
  ht1->SetLineStyle(2);
  ht2->SetLineStyle(2);

  ht1->Add(ht2);
  
  ht0->Add(ht1);
  
  TH1* htdum=(TH1*)ht0->Clone();
  
  ht0->SetLineColor(kRed-9);
  ht1->SetLineColor(kAzure+6);
  ht2->SetLineColor(kViolet -9); 

  THStack *st = new THStack();
  st->Add(estNE);
  st->Add(estNC);
  st->Add(estNM);
  
  
  hda->SetMarkerStyle(8);
  hda->GetXaxis()->SetTitle(" Reconstructed neutrino energy (GeV)");
  hda->GetXaxis()->CenterTitle();
  hda->GetYaxis()->SetTitle("10^{3} Events / 11.0 #times 10^{20} POT");
  hda->GetYaxis()->CenterTitle();
  hda->SetTitle("");

  hda->SetMaximum(4);
  hda->SetMinimum(1e-3);
  hda->GetYaxis()->SetRangeUser(0,8.5);
  hda->GetXaxis()->SetRangeUser(0,20);
  hda->Draw("E");
  st ->Draw("hist,same");
  ht0->Draw("hist,same");
  ht1->Draw("hist,same");
  ht2->Draw("hist,same");
  
  nn0->Draw("hist,same");
  nn1->Draw("hist,same");
  nn2->Draw("hist,same");
  hda->Draw("E,same");

  CenterTitles(hda);
  Preliminary();
  CornerLabels("#nu-beam");  
  Nue2018ThreeBinAxis(hda,true,true,true);
  Nue2018ThreeBinLabels(0.95,0.08,kGray+3,true);
  hda->GetXaxis()->SetTitle(" Reconstructed neutrino energy (GeV)");
  hda->Draw("E,same");

  TLegend *leg = new TLegend(0.3,0.47,0.6,0.77);
  leg->AddEntry(estNE,"Beam #nu_{e} CC","f");
  leg->AddEntry(estNC,"NC","f");
  leg->AddEntry(estNM,"#nu_{#mu} CC","f");
  leg->AddEntry(hda, "Data","le");
  leg->AddEntry(htdum, "Uncorr.","l");
  leg->Draw();
  
  std::cout << "Number of events after BEN+MICHEL" << endl;
  std::cout << "Data: " << hda->Integral() << endl;
  std::cout << "Numu: " << estNM->Integral() << endl;
  std::cout << "NC: " << estNC->Integral() << endl;
  std::cout << "Beam: " << estNE->Integral() << endl;

  can->SaveAs("Fig_DCMP.png");
  can->SaveAs("Fig_DCMP.pdf");
}


void draw_michel_plots()
{
  TGaxis::SetMaxDigits(3);

  
  std::string fname = "nue_dcmp.root";

  std::string dname="nue_decomps/michelbenDCMP";

  MichelDecomp *mdcmp = LoadFromFile<MichelDecomp>(fname,dname).release();
  // Uncomment this for the first run to generate 'MDCMP.root'
  //TFile *out = new TFile("MDCMP.root","recreate");
  // mdcmp->SavePlots(out->mkdir("mdcmp"));
  //out->Close();
  //return;

  TFile *in = new TFile(fname.c_str(),"read");

  TH2F *tnm = (TH2F*)(in->Get((dname+"/TempNumu/hist").c_str()))->Clone(UniqueName().c_str());
  TH2F *tnc = (TH2F*)(in->Get((dname+"/TempNC/hist").c_str()))->Clone(UniqueName().c_str());
  TH2F *tne = (TH2F*)(in->Get((dname+"/TempNue/hist").c_str()))->Clone(UniqueName().c_str());
  TH2F *tda = (TH2F*)(in->Get((dname+"/TempData/hist").c_str()))->Clone(UniqueName().c_str());
  const std::vector<const TH2F*> hs = {tnm,tnc,tne,tda};
  
  TH1F *nm = (TH1F*)(in->Get((dname+"/Numu/hist").c_str()))->Clone(UniqueName().c_str());
  TH1F *anm= (TH1F*)(in->Get((dname+"/AntiNumu/hist").c_str()))->Clone(UniqueName().c_str());
  nm->Add(anm);
  TH1F *ne = (TH1F*)(in->Get((dname+"/Nue/hist").c_str()))->Clone(UniqueName().c_str());
  TH1F *ane= (TH1F*)(in->Get((dname+"/AntiNue/hist").c_str()))->Clone(UniqueName().c_str());
  ne->Add(ane);
  TH1F *nc = (TH1F*)(in->Get((dname+"/NC/hist").c_str()))->Clone(UniqueName().c_str());
  TH1F *anc = (TH1F*)(in->Get((dname+"/AntiNC/hist").c_str()))->Clone(UniqueName().c_str());
  nc->Add(anc);
  TH1F *da = (TH1F*)(in->Get((dname+"/Data/hist").c_str()))->Clone(UniqueName().c_str());

  TH1D *potda = (TH1D*)(in->Get((dname+"/TempData/pot").c_str()))->Clone(UniqueName().c_str());
  TH1D *potmc = (TH1D*)(in->Get((dname+"/TempNumu/pot").c_str()))->Clone(UniqueName().c_str());
  std::cout << "POT " << potda->Integral() << "  " << potmc->Integral() << std::endl;
  const std::vector<const TH1F*> bs = {nm,nc,ne,da};
  
  TFile *in2 = new TFile("MDCMP.root", "read");
  TH1F *hscc = (TH1F*)(in2->Get("mdcmp/numu/numu_ratio"))->Clone(UniqueName().c_str());
  TH1F *hsnc = (TH1F*)(in2->Get("mdcmp/nc/nc_ratio"))->Clone(UniqueName().c_str());
  TH1F *hsne = (TH1F*)(in2->Get("mdcmp/nue/nue_ratio"))->Clone(UniqueName().c_str());
  const std::vector<const TH1F*> scales = {hscc,hsnc,hsne};
  
  std::vector<int> idxs = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
  double tot = 0;
  
  for (int idx : idxs)
    
    MakeStackNME(hs,potda->Integral()/potmc->Integral(), idx, idx, scales, "Reconstructed neutrino energy (GeV)","Events / 11.0 #times 10^{20} POT","Ratio",tot);

  std::cout << tot << std::endl;
  
  MakeScalesPlot(scales);

  MakeRawPlot(bs,potda->Integral()/potmc->Integral());

  MakeFracPlot(bs);
  
  MakeResultPlot(mdcmp, da, bs, scales, potda->Integral(), potda->Integral()/potmc->Integral());
}
