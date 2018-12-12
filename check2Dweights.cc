#include "TROOT.h"
#include "TKey.h"
#include "TFile.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TObjArray.h"
#include "THStack.h"
#include "TLegend.h"
#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"
#include "TF1.h"
#include "TMath.h"
#include "TCut.h"
#include "TPaletteAxis.h"
#include "TLatex.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <algorithm>

void plotAnyVariable(TCanvas * can, TCut baseCut, TCut twoDwCut, string varName, TLegend * legend, string label, int nbins, float min, float max, double maxHisto){
  TFile *File = TFile::Open("$EOSPATH/data/added/PhotonID/output_GJet_Combined_DoubleEMEnriched_TuneCP5_13TeV_Pythia8_reweighted.root");
// Local computer Mac : /Users/mhuwiler/cernbox/data/added/PhotonID/output_GJet_Combined_DoubleEMEnriched_TuneCP5_13TeV_Pythia8_reweighted.root

  TTree *t_FakePhotons = (TTree*)File->Get("fakePhotons");
  TTree *t_PromtPhotons = (TTree*)File->Get("promptPhotons");

  TH1F * histo_fake = new TH1F ("histo_fake","histo_fake",nbins,min,max);
  TH1F * histo_promt = new TH1F ("histo_promt","histo_promt",nbins,min,max);
  TH1F * histo_promt_with2Dw = new TH1F ("histo_promt_with2Dw","histo_promt_with2Dw",nbins,min,max);

  TString tmp_fake = "";
  tmp_fake += varName;
  tmp_fake+=">>histo_fake";

  TString tmp_promt = "";
  tmp_promt += varName;
  tmp_promt +=">>histo_promt";

  TString tmp_promt_2Dw = "";
  tmp_promt_2Dw += varName;
  tmp_promt_2Dw +=">>histo_promt_with2Dw";

  t_FakePhotons->Draw(tmp_fake,baseCut,"goff");
  t_PromtPhotons->Draw(tmp_promt,baseCut,"goff");
  t_PromtPhotons->Draw(tmp_promt_2Dw,twoDwCut,"goff");

  histo_fake->Sumw2();
  histo_promt->Sumw2();
  histo_promt_with2Dw->Sumw2();

  can->cd();
  //  can->SetLogy();

  histo_fake->SetTitle("");
  histo_fake->SetStats(0);
  histo_fake->GetXaxis()->SetTitle(label.c_str());
  histo_fake->SetLineColor(kRed);
  histo_fake->SetLineWidth(2);  

  histo_fake->Scale(1./histo_fake->Integral());
  histo_promt->Scale(1./histo_promt->Integral());
  histo_promt_with2Dw->Scale(1./histo_promt_with2Dw->Integral());

  histo_fake->SetMinimum(0.);
  histo_promt->SetMinimum(0.);

  double maxVal_prompt = histo_promt->GetBinContent(histo_promt->GetMaximumBin());
  double maxVal_fake = histo_fake->GetBinContent(histo_fake->GetMaximumBin());

  double maxValue = 0;

  if(maxVal_prompt > maxVal_fake){
    maxValue = maxVal_prompt*1.2;
  }
  else{
    maxValue = maxVal_fake*1.2;
  }

  histo_fake->SetMarkerColor(kRed);
  histo_fake->SetMarkerStyle(20);
  histo_fake->SetMarkerSize(0.7);

  histo_fake->SetMaximum(maxValue);
  histo_fake->Draw();
  
  histo_promt->SetLineColor(kBlue);
  histo_promt->SetLineWidth(2);

  histo_promt->SetMarkerColor(kBlue);
  histo_promt->SetMarkerStyle(20);
  histo_promt->SetMarkerSize(0.7);

  histo_promt_with2Dw->SetMarkerColor(kGreen+1);
  histo_promt_with2Dw->SetMarkerStyle(20);
  histo_promt_with2Dw->SetMarkerSize(0.7);

  histo_promt->Draw("EPsame");

  histo_promt_with2Dw->Draw("EPsame");

  histo_promt_with2Dw->SetLineColor(kGreen+1);
  histo_promt_with2Dw->SetLineWidth(2);


  legend->AddEntry(histo_fake,"bkg","lem");
  legend->AddEntry(histo_promt,"sig","lem");
  legend->AddEntry(histo_promt_with2Dw,"sig * 2D weight","lem");

  legend->Draw("same");

  TLatex *txt = new TLatex(0.2, 0.9, "");
  // txt->SetTextSize(0.05);
  txt->DrawLatexNDC(0.1, 0.91, "CMS #bf{#it{#scale[0.8]{Simulation Preliminary}}}");
  txt->DrawLatexNDC(0.76, 0.91, "#bf{13 TeV}");
  txt->Draw("same");

  can->Update();
  can->Modified();

}

void check2Dweights(){


  bool plot_pT = true;
  bool plot_SCEta = true;
  bool plot_SCERaw = false;

  bool doEB = true;
  bool doEE = false;

  string tmp = "_94X_allPho_valid";
  if(doEE) tmp = "_94X_EEPho_Hgg";
  if(doEB) tmp = "_94X_EBPho_Hgg";


  if(plot_pT){

    TCanvas * can_pT = new TCanvas("can_pT","can_pT",600,600);

    TCut baseCut_rho = "(covIphiIphi<100 && pt > 20)*weight";
    TCut twoDwCut_rho = "(covIphiIphi<100 && pt > 20)*weight*PtvsEtaWeight";

    if(doEB) {
      baseCut_rho = "(abs(scEta)<1.4442 && pt > 20)*weight";
      twoDwCut_rho = "(abs(scEta)<1.4442 && pt > 20)*weight*PtvsEtaWeight";
    }

    if(doEE) {
      baseCut_rho = "(abs(scEta)>1.566 && pt > 20)*weight";
      twoDwCut_rho = "(abs(scEta)>1.566 && pt > 20)*weight*PtvsEtaWeight";
    }

    string varName_rho = "pt";
    string label_rho = "p_{T}";

    int nbins_rho = 62; //65
    float min_rho = 0;
    float max_rho = 250;

    double maxHisto_rho = .05;

    TLegend *legend_rho = new TLegend(0.6,0.65,.9,0.90,"","brNDC");
    legend_rho->SetHeader("");
    if(doEE) legend_rho->SetHeader("EE");
    if(doEB) legend_rho->SetHeader("EB");

    legend_rho->SetBorderSize(0);
    legend_rho->SetFillStyle(0);
    legend_rho->SetTextFont(42);

    plotAnyVariable(can_pT, baseCut_rho, twoDwCut_rho, varName_rho, legend_rho, label_rho, nbins_rho, min_rho, max_rho, maxHisto_rho);

    string rho_Name = "check2DW_PhoPtRew";
    rho_Name += tmp;
    can_pT->SaveAs((rho_Name+".pdf").c_str());
    can_pT->SaveAs((rho_Name+".png").c_str());
    can_pT->SaveAs((rho_Name+".root").c_str());


  }

  if(plot_SCEta){

    TCanvas * can_SCEta = new TCanvas("can_SCEta","can_SCEta",600,600);

    TCut baseCut_SCEta = "(covIphiIphi<100 && pt > 20)*weight";
    TCut twoDwCut_SCEta = "(covIphiIphi<100 && pt > 20)*weight*PtvsEtaWeight";

    if(doEB) {
      baseCut_SCEta = "(abs(scEta)<1.4442 && pt > 20)*weight";
      twoDwCut_SCEta = "(abs(scEta)<1.4442 && pt > 20)*weight*PtvsEtaWeight";
    }

    if(doEE) {
      baseCut_SCEta = "(abs(scEta)>1.5 && pt > 20)*weight";
      twoDwCut_SCEta = "(abs(scEta)>1.5 && pt > 20)*weight*PtvsEtaWeight";
    }

    string varName_SCEta = "abs(scEta)";
    string label_SCEta = "SC #eta";

    int nbins_SCEta = 10;
    float min_SCEta = 1.566;
    float max_SCEta = 2.5;

    if(doEB){
      min_SCEta = 0.;
      max_SCEta = 1.4442;
    }

    double maxHisto_SCEta = .025;
    if(doEB) maxHisto_SCEta = .03;
    if(doEE) maxHisto_SCEta = .1;
    
    TLegend *legend_SCEta = new TLegend(0.15,0.15,.45,0.45,"","brNDC");
    // TLegend *legend_SCEta = new TLegend(0.15,0.65,.45,0.90,"","brNDC");
    legend_SCEta->SetHeader("");
    if(doEE) legend_SCEta->SetHeader("EE");
    if(doEB) legend_SCEta->SetHeader("EB");
    
    legend_SCEta->SetBorderSize(0);
    legend_SCEta->SetFillStyle(0);
    legend_SCEta->SetTextFont(42);

    plotAnyVariable(can_SCEta, baseCut_SCEta, twoDwCut_SCEta, varName_SCEta, legend_SCEta, label_SCEta, nbins_SCEta, min_SCEta, max_SCEta, maxHisto_SCEta);

    string SCEta_Name = "check2DW_PhoEtaRew";
    SCEta_Name += tmp;
    can_SCEta->SaveAs((SCEta_Name+".pdf").c_str());
    can_SCEta->SaveAs((SCEta_Name+".png").c_str());
    can_SCEta->SaveAs((SCEta_Name+".root").c_str());


  }


}
