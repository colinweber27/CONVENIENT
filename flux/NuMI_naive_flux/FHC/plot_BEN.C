#include "3FlavorAna/Plotting/NuePlotStyle.h"
#include "CAFAna/Analysis/Style.h"
#include "CAFAna/Analysis/Exposures.h"
#include "CAFAna/Core/LoadFromFile.h"
#include "3FlavorAna/Decomp/BENDecomp.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Weights/GenieWeights.h"
#include "CAFAna/Weights/PPFXWeights.h"
#include "CAFAna/Vars/HistAxes.h"
#include "Utilities/rootlogon.C"
#include "TFile.h"
#include "TLine.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TObject.h"
#include "THStack.h"
#include "TH3.h"
#include "TH2.h"
#include "TH1.h"
#include "TROOT.h"
#include "TGaxis.h"


#include <iomanip>
using namespace ana;
void SetPaletteS()
{
const Int_t NRGBs = 3;
const Int_t NCont = 255;
Double_t stops[NRGBs] = { 0.0 , 0.5, 1};
Double_t red[NRGBs]   = { 1   , 0.6, .2};
Double_t green[NRGBs] = { 0.8   , 0.2, .0};
Double_t blue[NRGBs]  = { 0.8   , 0.2, .0};
TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
gStyle->SetNumberContours(NCont);
}

void CornerLabels(std::string Str) {
  TLatex* CornLab = new TLatex(.1, .93, Str.c_str());
  CornLab->SetTextColor(kGray+1);
  CornLab->SetNDC();
  CornLab->SetTextSize (1.5/30.);
  CornLab->SetTextAlign(11);
  CornLab->Draw();
}


//POT For 2020 analysis
const double pot = 1.10158e+21;
std::string fname = "nue_dcmp.root";
const std::string path = "plots";

void PlotPionPlots(BENDecomp* dcmp, std::string ananame);

void CompareDecompDataMC(BENDecomp *ben, std::string ana);

///for 2018 official uncomment the two lines you see below and comment out the general lines  
void plot_BEN(const std::string file){ 
  if (std::strstr(file.c_str(),"official")) {fname = "/nova/ana/nu_e_ana/Ana2018/Predictions/provisional_singles/NueProd4SystematicsOnRealNDData_2018-05-03/pred_allxp_Nue2018Axis_full_FHCAllSyst_nueconcat_realNDData_2018-05-11.root";}
  //BENDecomp* benD = LoadFromFile<BENDecomp>(fname, TString::Format("pred_xp_combo_Nominal/noShift/nue_pred_Nue2018Axis/extrap/EEextrap/Decomp/NueEstimate").Data()).release();
  
  BENDecomp* benD = LoadFromFile<BENDecomp>(fname, TString::Format("nue_decomps/benDCMP" ).Data()).release();
  std::cout << "2020 kaon scale" << benD->GetKaonScale() << std::endl;
  //Plot Decomposed data vs MC
  
  BENDecomp* ben[1] = {benD};
  //const std::string bennames[1] =  {"pred_xp_combo_Nominal/noShift/nue_pred_Nue2018Axis/extrap/EEextrap/Decomp/NueEstimate"};
  const std::string bennames[1] = {"nue_decomps/benDCMP"};

  TFile * outfile = new TFile((TString)file + "BEN_plots.root","recreate");

  for (int i = 0; i< 1; ++i){
    TDirectory* d = outfile->mkdir((TString)bennames[i]);

    const double pot = ben[i] -> Data_Component().POT();

    CompareDecompDataMC(ben[i], bennames[i]);
    PlotPionPlots(ben[i], bennames[i]);

    ben[i]->SavePlotsPi(d);
    ben[i]->SavePlotsKa(d);
    ben[i]->SavePlots(d);

  }
}
void PlotPionPlots(BENDecomp* dcmp, std::string ananame){
  new TCanvas;
  rootlogon();
  gStyle->SetTitleOffset(.82, "xyz");
  gStyle->SetLabelSize(0.035, "xy");
  gStyle->SetLabelSize(.035, "z");
  gStyle->SetTitleSize(0.05, "xyz");
  const std::string path = "plots";

  auto data = dcmp->Data_Component().ToTH1(pot);

  data->SetMarkerStyle(kFullCircle);

  auto nemc = dcmp->MC_NueComponent().ToTH1(pot);
  nemc->SetLineStyle(7);
  nemc->Scale(.001);
  nemc->Draw("hist");
  nemc->GetYaxis()->SetTitle("10^{3} Events / 11.0 #times 10^{20} POT");
  nemc->GetXaxis()->SetTitle("Reconstructed neutrino energy (GeV)");
  nemc->GetYaxis()->SetRangeUser(0,1.2*nemc->GetMaximum());
  TGaxis::SetMaxDigits(3);
  TLine* line = new TLine();
  line->SetLineStyle(3);
  const float ymin = nemc->GetMinimum();
  
  float size = nemc->GetXaxis()->GetLabelSize();
  float offs = nemc->GetXaxis()->GetLabelOffset();
  int   font = nemc->GetXaxis()->GetLabelFont();
  nemc->GetXaxis()->SetRangeUser(0,19);
  nemc->GetXaxis()->SetNdivisions(1900, kFALSE);
  TGaxis* gax1 = new TGaxis(0, ymin, 9, ymin, 0.0001, 4.5, 205, "S L");
  TGaxis* gax2 = new TGaxis(9, ymin, 18, ymin, 0.0001, 4.5, 205, "S L");
  int numAxes = 2;
  std::vector <TGaxis*> gaxes = {gax1, gax2};
  for(int a = 0; a < numAxes; a++){
      gaxes[a]->SetTickLength(0.);
      gaxes[a]->SetLabelSize(size);
      gaxes[a]->SetLabelOffset(offs);
      gaxes[a]->SetLabelFont(font);
      gaxes[a]->Draw();
  }  
  nemc->Draw("hist");  
  //Nue2018ThreeBinDivisions();
  Nue2018ThreeBinLabels(0.98, 0.04, kGray+3, true);
  // Nue2018ThreeBinAxis(nemc,true, true, false);

  auto ne  = dcmp->NueComponent(). ToTH1(pot);
  ne->Scale(.001);
  ne->Draw("hist same");



  auto nuepiunwei = LoadFromFile<Spectrum>(fname, TString::Format("%s/nueCC_from_pi",ananame.c_str()).Data()).release();
  TH1* fnuepiunwei = nuepiunwei->ToTH1(pot,kAzure +4, 7);
  auto nuekaunwei = LoadFromFile<Spectrum>(fname, TString::Format("%s/nueCC_from_ka",ananame.c_str()).Data()).release();
  TH1* fnuekaunwei = nuekaunwei->ToTH1(pot,kMagenta -9, 7);
  auto nueotherunwei = LoadFromFile<Spectrum>(fname, TString::Format("%s/nueCC_from_other",ananame.c_str()).Data()).release();
  TH1* fnueotherunwei = nueotherunwei->ToTH1(pot,kGreen + 2, 7);
  
  fnuepiunwei->Scale(.001);
  fnuekaunwei->Scale(.001);
  fnueotherunwei->Scale(.001);
  fnuepiunwei ->Draw("hist same");
  fnuekaunwei ->Draw("hist same");
  fnueotherunwei ->Draw("hist same");

  TH1* fnuepiweight = dcmp -> NuePiEstimate().ToTH1(pot,kAzure +4);
  fnuepiweight->Scale(.001);
  fnuepiweight->Draw("hist same");

  TH1* fnuekaweight = dcmp -> NueKaEstimate().ToTH1(pot, kMagenta -9);
  fnuekaweight->Scale(.001);
  fnuekaweight->Draw("hist same");
  TH1* fnueotherweight = dcmp ->NueComponent().ToTH1(pot, kGreen+2);
  fnueotherweight->Scale(.001);
  fnueotherweight->Add(fnuekaweight, -1);
  fnueotherweight->Add(fnuepiweight, -1);
  fnueotherweight->Draw("hist same");
 
  Simulation();
  CenterTitles(nemc);
  CornerLabels("#nu-beam");
  TLegend * leg = new TLegend(0.165,0.4,0.39,0.75);
  gStyle->SetTextSize(0.047);
  leg->AddEntry(ne, "Total #nu_{e} Corr.", "l");
  leg->AddEntry(fnuepiweight,"#nu_{e} from #pi^{#pm} Corr.","l");
  leg->AddEntry(fnuekaweight,"#nu_{e} from K^{#pm} Corr.","l");
  leg->AddEntry(fnueotherweight,"#nu_{e} from Other Corr.", "l");
  leg->AddEntry(nemc, "#nu_{e} Uncorr.", "l");
  leg->Draw("same");


// nue table tota/pi/ka/other
//
std::cout<<"\\multirow{2}*{Total $\\nu_e$}   & Uncorrected &"<< nemc->Integral(0,9) <<" & "<< nemc->Integral(10, 18)<<" & "<< nemc->Integral(19, 27)<<" & "<< nemc->Integral()<<" \\\\"<<std::endl;

std::cout<<setprecision(4)<<"   &   Corrected & "<<  ne->Integral(0,9) <<"("<<100*(ne->Integral(0,9)-nemc->Integral(0,9))/nemc->Integral(0,9)<<"\\%)" <<" & "<< ne->Integral(10, 18)<<"("<<100*(ne->Integral(10,18)-nemc->Integral(10,18))/nemc->Integral(10,18)<<"\\%)" << " & "<< ne->Integral(19,27) <<"("<<100*(ne->Integral(19,27)-nemc->Integral(19,27))/nemc->Integral(19,27)<<"\\%)" << " & "<< ne->Integral() <<"("<<100*(ne->Integral()-nemc->Integral())/nemc->Integral()<<"\\%)" << " \\\\"<<std::endl;
std::cout<<"\\hline"<<std::endl;


std::cout<<"\\multirow{2}*{$\\nu_e$ From Pion}   & Uncorrected &"<< fnuepiunwei->Integral(0,9) <<" & "<< fnuepiunwei->Integral(10, 18)<<" & "<< fnuepiunwei->Integral(19,27)<<" & "<< fnuepiunwei->Integral()<<" \\\\"<<std::endl;

std::cout<<"   &   Corrected & "<<  fnuepiweight->Integral(0,9) <<"("<<100*(fnuepiweight->Integral(0,9)-fnuepiunwei->Integral(0,9))/fnuepiunwei->Integral(0,9)<<"\\%)" <<" & "<< fnuepiweight->Integral(10, 18)<<"("<<100*(fnuepiweight->Integral(10,18)-fnuepiunwei->Integral(10,18))/fnuepiunwei->Integral(10,18)<<"\\%)" << " & "<<  fnuepiweight->Integral(19,27)<<"("<<100*(fnuepiweight->Integral(19,27)-fnuepiunwei->Integral(19,27))/fnuepiunwei->Integral(19,27)<<"\\%)" <<" & "<<  fnuepiweight->Integral()<<"("<<100*(fnuepiweight->Integral()-fnuepiunwei->Integral())/fnuepiunwei->Integral()<<"\\%)" <<" \\\\"<<std::endl;
std::cout<<"\\hline"<<std::endl;

std::cout<<"\\multirow{2}*{$\\nu_e$ From Kaon}   & Uncorrected &"<< fnuekaunwei->Integral(0,9) <<" & "<< fnuekaunwei->Integral(10, 18)<<" & "<< fnuekaunwei->Integral(19,27)<<" & "<< fnuekaunwei->Integral()<<" \\\\"<<std::endl;
std::cout<<"   &   Corrected & "<<  fnuekaweight->Integral(0,9) <<"("<<100*(fnuekaweight->Integral(0,9)-fnuekaunwei->Integral(0,9))/fnuekaunwei->Integral(0,9)<<"\\%)" <<" & "<< fnuekaweight->Integral(10, 18)<<"("<<100*(fnuekaweight->Integral(10,18)-fnuekaunwei->Integral(10,18))/fnuekaunwei->Integral(10,18)<<"\\%)" <<" & "<< fnuekaweight->Integral(19,27)<<"("<<100*(fnuekaweight->Integral(19,27)-fnuekaunwei->Integral(19,27))/fnuekaunwei->Integral(19,27)<<"\\%)" <<" & "<< fnuekaweight->Integral()<<"("<<100*(fnuekaweight->Integral()-fnuekaunwei->Integral())/fnuekaunwei->Integral()<<"\\%)" <<" \\\\"<<std::endl;

std::cout<<"\\hline"<<std::endl;

std::cout<<"\\multirow{2}*{$\\nu_e$ From Other}   & Uncorrected &"<< fnueotherunwei->Integral(0,9) <<" & "<< fnueotherunwei->Integral(10, 18)<<" & "<< fnueotherunwei->Integral(19,27)<<" & "<< fnueotherunwei->Integral()<<" \\\\"<<std::endl;

std::cout<<"   &   Corrected & "<<  fnueotherweight->Integral(0,9) <<"("<<100*(fnueotherweight->Integral(0,9)-fnueotherunwei->Integral(0,9))/fnueotherunwei->Integral(0,9)<<"\\%)" <<" & "<< fnueotherweight->Integral(10, 18)<<"("<<100*(fnueotherweight->Integral(10,18)-fnueotherunwei->Integral(10,18))/fnueotherunwei->Integral(10,18)<<"\\%)" <<" & "<< fnueotherweight->Integral(19,27)<<"("<<100*(fnueotherweight->Integral(19,27)-fnueotherunwei->Integral(19,27))/fnueotherunwei->Integral(19,27)<<"\\%)" <<" & "<< fnueotherweight->Integral()<<"("<<100*(fnueotherweight->Integral()-fnueotherunwei->Integral())/fnueotherunwei->Integral()<<"\\%)" <<" \\\\"<<std::endl;

std::cout<<"\\hline"<<std::endl;

  gPad->SaveAs((TString)"plots/"+ ananame + "_nueCC_from_All.pdf");


}


void CompareDecompDataMC(BENDecomp* dcmp, std::string ananame){
  const std::string path = "plots";
rootlogon();
std::cout<<ananame<<": "<< pot<<std::endl;
auto data = dcmp->Data_Component().ToTH1(pot);
data->SetMarkerStyle(kFullCircle);

//uncorrected first
auto ccmc = dcmp->MC_NumuComponent().ToTH1(pot);
ccmc->SetLineColor(kNumuBackgroundColor);
ccmc->SetLineStyle(7);
auto ccbarmc=dcmp->MC_AntiNumuComponent().ToTH1(pot);
ccbarmc->SetLineColor(kNumuBarBackgroundColor);
ccbarmc->SetLineStyle(7);
auto nemc = dcmp->MC_NueComponent().ToTH1(pot);
nemc->SetLineColor(kBeamNueBackgroundColor);
nemc->SetLineStyle(7);
auto nebarmc = dcmp->MC_AntiNueComponent().ToTH1(pot);
nebarmc->SetLineColor(kBeamNueBarBackgroundColor);
nebarmc->SetLineStyle(7);
auto ncmc = (dcmp->MC_NCTotalComponent()).ToTH1(pot);
ncmc->SetLineColor(kNCBackgroundColor);
ncmc->SetLineStyle(7);

auto totalmc = ( dcmp->MC_NumuComponent()+dcmp->MC_AntiNumuComponent()+dcmp->MC_NueComponent() +dcmp->MC_AntiNueComponent()+dcmp->MC_NCTotalComponent()).ToTH1(pot,kRed,2);


THStack * hsmc = new THStack ("hsmc","");
hsmc->Add(nemc, "hist");
hsmc->Add(nebarmc,"hist");
hsmc->Add(ncmc,"hist");
hsmc->Add(ccmc, "hist");
hsmc->Add(ccbarmc, "hist");

char leg_data[50], leg_tot[50], leg_nemc[50],leg_nebarmc[50],leg_ccmc[50],leg_ccbarmc[50],leg_nc[50];

sprintf(leg_data, "Data: Bin1=  %.1f, Bin2= %.1f", data->Integral(0,9), data->Integral(10, 18));
//  sprintf(leg_tot, "Tot MC: Bin1=  %.1f, Bin2= %.1f", totalmc->Integral(0,9), totalmc->Integral(10, 18));
sprintf(leg_nemc, "#{nu}_{e}: Bin1=  %.1f, Bin2= %.1f", nemc->Integral(0,9), nemc->Integral(10, 18));
sprintf(leg_nebarmc, "#bar{#nu}_{e}: Bin1=  %.1f, Bin2= %.1f", nebarmc->Integral(0,9), nebarmc->Integral(10, 18));
sprintf(leg_ccmc, "#{nu}_{#mu}: Bin1=  %.1f, Bin2= %.1f", ccmc->Integral(0,9), ccmc->Integral(10, 18));
sprintf(leg_ccbarmc, "#bar{#nu}_{#mu}: Bin1=  %.1f, Bin2= %.1f", ccbarmc->Integral(0,9), ccbarmc->Integral(10, 18));
sprintf(leg_nc, "NC: Bin1=  %.1f, Bin2= %.1f", ncmc->Integral(0,9), ncmc->Integral(10, 18));

// corrected

auto cc  = dcmp->NumuComponent().ToTH1(pot);
cc->SetLineColor(kNumuBackgroundColor);
auto ccbar = dcmp->AntiNumuComponent().ToTH1(pot);
ccbar->SetLineColor(kNumuBarBackgroundColor);

auto ne  = dcmp->NueComponent(). ToTH1(pot);
ne->SetLineColor(kBeamNueBackgroundColor );
auto nebar  = dcmp->AntiNueComponent(). ToTH1(pot);
nebar->SetLineColor(kBeamNueBarBackgroundColor );

auto nc  = (dcmp->NCTotalComponent()).ToTH1(pot);
nc->SetLineColor(kNCBackgroundColor);
auto totalmcc = ( dcmp->NumuComponent()+dcmp->AntiNumuComponent()+dcmp->NueComponent() +dcmp->AntiNueComponent()+dcmp->NCTotalComponent()).ToTH1(pot,kRed);

THStack * hs = new THStack ("hs","");

hs->Add(ne, "hist");
hs->Add(nebar, "hist");
hs->Add(nc,"hist");
hs->Add(cc, "hist");
hs->Add(ccbar, "hist");

char leg_totc[50], leg_ne[50],leg_nebar[50],leg_cc[50],leg_ccbar[50],leg_ncc[50];

sprintf(leg_totc, "MC: Bin1=  %.1f, Bin2= %.1f", totalmcc->Integral(0,9), totalmcc->Integral(10, 18));
sprintf(leg_ne, "#{nu}_{e}: Bin1= %.1f, Bin2= %.1f", ne->Integral(0,9), ne->Integral(10, 18));
sprintf(leg_nebar, "#bar{#nu}_{e}: Bin1= %.1f, Bin2= %.1f", nebar->Integral(0,9), nebar->Integral(10, 18));
sprintf(leg_cc, "#{nu}_{#mu}: Bin1= %.1f, Bin2= %.1f", cc->Integral(0,9), cc->Integral(10, 18));
sprintf(leg_ccbar, "#bar{#nu}_{#mu}: Bin1=  %.1f, Bin2= %.1f", ccbar->Integral(0,9), ccbar->Integral(10, 18));
sprintf(leg_ncc, "NC: Bin1=  %.1f, Bin2= %.1f", nc->Integral(0,9), nc->Integral(10, 18));

std::cout<<"\\multirow{2}*{$\\nu_e$}   & Uncorrected & "<<  nemc->Integral(0,9) <<" & "<< nemc->Integral(10, 18)<<" \\\\"<<std::endl;
std::cout<<"   &   Corrected & "<<  ne->Integral(0,9) <<" & "<< ne->Integral(10, 18)<<" \\\\"<<std::endl;
std::cout<<"\\multirow{2}*{$\\bar{\\nu}_e$}   & Uncorrected & "<<  nebarmc->Integral(0,9) <<" & "<< nebarmc->Integral(10, 18)<<" \\\\"<<std::endl;
std::cout<<"   &   Corrected & "<<  nebar->Integral(0,9) <<" & "<< nebar->Integral(10, 18)<<" \\\\"<<std::endl;

std::cout<<"\\multirow{2}*{$\\nu_\\mu$}   & Uncorrected & "<<  ccmc->Integral(0,9) <<" & "<< ccmc->Integral(10, 18)<<" \\\\"<<std::endl;
std::cout<<"   &   Corrected & "<<  cc->Integral(0,9) <<" & "<< cc->Integral(10, 18)<<" \\\\"<<std::endl;
std::cout<<"\\multirow{2}*{$\\bar{\\nu}_\\mu$}   & Uncorrected & "<<  ccbarmc->Integral(0,9) <<" & "<< ccbarmc->Integral(10, 18)<<" \\\\"<<std::endl;
std::cout<<"   &   Corrected & "<<  ccbar->Integral(0,9) <<" & "<< ccbar->Integral(10, 18)<<" \\\\"<<std::endl;

std::cout<<"\\multirow{2}*{nc}   & Uncorrected & "<<  ncmc->Integral(0,9) <<" & "<< ncmc->Integral(10, 18)<<" \\\\"<<std::endl;
std::cout<<"   &   Corrected & "<<  nc->Integral(0,9) <<" & "<< nc->Integral(10, 18)<<" \\\\"<<std::endl;


  new TCanvas;
  data->Draw();
  hsmc->Draw("hist same");
  hs->Draw("hist same");
  data->Draw("same");

  CenterTitles(data);
  Preliminary();

  TLegend * leg = new TLegend(0.7,0.5,0.89,0.89);
  leg->AddEntry(data,"ND Data","lep");
  leg->AddEntry(ne, "Beam #nu_{e}","l");
  leg->AddEntry(nebar, "Beam #bar{#nu}_{e}","l");
  leg->AddEntry(cc, "#nu_{#mu} CC","l");
  leg->AddEntry(ccbar, "#bar{#nu}_{#mu} CC","l");
  leg->AddEntry(nc, "NC","l");

  auto dummy = (TH1*) nemc->Clone();
  dummy->SetLineColor(kBlack);
  leg->AddEntry(dummy, "Uncorr. MC","l");


  leg->SetLineColor(4000);
  leg->SetTextSize(0.03);
  leg->SetTextFont(42);
  leg->Draw("same");
  gPad->Update();
  gPad->SaveAs((TString)"plots/"+ ananame + "_Data_MC_corrected_uncorrected_stack.pdf");

  ne->GetYaxis()->SetRangeUser(0,3000);
  ne->Draw("hist");
  cc->Draw("hist same");
  nc->Draw("hist same");
  ccmc->Draw("hist same");
  nemc->Draw("hist same");
  ncmc->Draw("hist same");
  nebar->Draw("hist same");
  ccbar->Draw("hist same");
  ccbarmc->Draw("hist same");
  nebarmc->Draw("hist same");
  leg->Draw();
  rootlogon();
  ne->SetTitleSize(0);
  Simulation();
  gPad->Update();
  gPad->SaveAs((TString)"plots/"+ ananame + "_Data_MC_corrected_uncorrected.pdf");

}

