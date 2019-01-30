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


void myCheck2Dweights(){


  Bool_t isEndcap = false; 

  TString outputfilename = "plotCheck2DWeights"; 

  
  TFile *File = TFile::Open("$EOSPATH/data/added/PhotonID/output_GJet_Combined_DoubleEMEnriched_TuneCP5_13TeV_Pythia8_reweighted.root");
  // Local computer Mac : /Users/mhuwiler/cernbox/data/added/PhotonID/output_GJet_Combined_DoubleEMEnriched_TuneCP5_13TeV_Pythia8_reweighted.root

  TTree *signal = static_cast<TTree*>(File->Get("promptPhotons"));
  TTree *background = static_cast<TTree*>(File->Get("fakePhotons"));

  // The cuts 

  TString cut, cutW; 
  // What is that ?  "(covIphiIphi<100 && pt > 20)*weight";
  if (isEndcap) {
    cut = "(abs(scEta)>1.566 && pt > 20)*weight"; 
    cutW = "(abs(scEta)>1.566 && pt > 20)*weight*PtvsEtaWeight"; 
  }
  else {
    cut = "(abs(scEta)<1.4442 && pt > 20)*weight"; 
    cutW = "(abs(scEta)<1.4442 && pt > 20)*weight*PtvsEtaWeight"; 
  }


  // Plotting the pT 


{

  TCanvas * canvasPt = new TCanvas("can_pT","can_pT",800,600);

  Int_t nPtBins = 31; 
  Double_t pTBins[32] = {10.0,12.0,14.0,16.0,18.0,20.0,22.0,24.0,26.0,28.0,30.0,32.0,34.0,36.0,38.0,40.0,42.0,44.0,46.0,48.0,50.0,55.0,60.0,65.0,70.0,75.0,80.0,90.0,100.0,150.0,200.0,250.0}; 
  // One edge more than bins 


  THStack *histoStack = new THStack("histoStack", ""); 

  TH1D *histSig = new TH1D("histSig", "Signal distribution", nPtBins, pTBins); 
  TH1D *histBkg = new TH1D("histBkg", "Background distribution", nPtBins, pTBins); 
  TH1D *histSigRw = new TH1D("histSigRw", "Reweighted Signal distribution", nPtBins, pTBins); 

  background->Draw("pt>>histBkg", cut, "goff");
  signal->Draw("pt>>histSig", cut, "goff");
  signal->Draw("pt>>histSigRw", cutW, "goff");

  // histBkg->Sumw2();   // Stores the sum squared of the weights 
  // histSig->Sumw2(); // Normalises the errors with the weights 
  // histSigRw->Sumw2();

  // Normalising histograms 
  histBkg->Scale(1./histBkg->Integral());
  histSig->Scale(1./histSig->Integral());
  histSigRw->Scale(1./histSigRw->Integral());

  Double_t maxValue = max(histSig->GetBinContent(histSig->GetMaximumBin()), histBkg->GetBinContent(histBkg->GetMaximumBin()))*1.2;


  // Setting style fot background 
  histBkg->SetMinimum(0.);
  histBkg->SetMarkerColor(kRed);
  histBkg->SetLineColor(kRed);
  histBkg->SetLineWidth(2);  
  histBkg->SetMarkerStyle(20);
  histBkg->SetMarkerSize(0.7);


  // Setting style for signal 
  histSig->SetMinimum(0.);
  histSig->SetLineColor(kBlue);
  histSig->SetLineWidth(2);
  histSig->SetMarkerColor(kBlue);
  histSig->SetMarkerStyle(20);
  histSig->SetMarkerSize(0.7);


  // Setting style for reweighted signal 
  histSigRw->SetMarkerColor(kGreen+1);
  histSigRw->SetLineColor(kGreen+1);
  histSigRw->SetLineWidth(2);
  histSigRw->SetMarkerStyle(20);
  histSigRw->SetMarkerSize(0.7);


  histoStack->Add(histSig); 
  histoStack->Add(histBkg); 
  histoStack->Add(histSigRw); 



  TLegend *legendPt = new TLegend(0.6,0.65,.9,0.90,"","brNDC");
  if(isEndcap) legendPt->SetHeader("ECAL Endcap");
  else legendPt->SetHeader("ECAL Barrel");

  legendPt->SetBorderSize(0);
  legendPt->SetFillStyle(0);
  legendPt->SetTextFont(42);

  legendPt->AddEntry(histBkg,"bkg","lem");
  legendPt->AddEntry(histSig,"sig","lem");
  legendPt->AddEntry(histSigRw,"sig * 2D weight","lem");

       
  canvasPt->cd(); 

  histoStack->Draw("nostack"); 


  legendPt->Draw("same");



  // canvasPt->cd();
  // histBkg->SetTitle("");
  // histBkg->SetStats(0);
  // histBkg->GetXaxis()->SetTitle("p_{T}");
  // Double_t maxSig = histSig->GetBinContent(histSig->GetMaximumBin());
  // Double_t maxBkg = histBkg->GetBinContent(histBkg->GetMaximumBin()); 
  // Double_t maxValue = max(maxSig, maxBkg)*1.2;
  // histBkg->SetMaximum(maxValue);
  // histBkg->Draw();
  // histSig->Draw("EPsame");    // EPsame : 
  // histSigRw->Draw("EPsame");


  

  TLatex *txt = new TLatex(0.2, 0.9, "");
  // txt->SetTextSize(0.05);
  txt->DrawLatexNDC(0.1, 0.91, "CMS #bf{#it{#scale[0.8]{Simulation Preliminary}}}");
  txt->DrawLatexNDC(0.76, 0.91, "#bf{13 TeV}");
  txt->Draw("same");

  histoStack->GetXaxis()->SetTitle("p_T"); 
  canvasPt->Update();
  canvasPt->Modified();
 
    

  TString outname = outputfilename+"pT"; 
  if (isEndcap) outname+="Endcap"; 
  else outname+="Barrel"; 

  canvasPt->SaveAs(outname+".pdf");
  canvasPt->SaveAs(outname+".root");
  canvasPt->SaveAs(outname+".png");


}


{


  // Plotting the eta


  TCanvas * canvasEta = new TCanvas("can_eta","can_eta",800,600);

  Int_t nEtaBins = 10; 
  Double_t etaMin = 999., etaMax = 999.; 
  if (isEndcap) {
    etaMin = 1.566; 
    etaMax = 2.5; 
  }
  else {
    etaMin = 0.;
    etaMax = 1.4442; 
  }
  // One edge more than bins 


  THStack *histoStack = new THStack("histoStackEta", ""); 

  TH1D *histSig = new TH1D("histSig", "Signal distribution", nEtaBins, etaMin, etaMax); 
  TH1D *histBkg = new TH1D("histBkg", "Background distribution", nEtaBins, etaMin, etaMax); 
  TH1D *histSigRw = new TH1D("histSigRw", "Reweighted Signal distribution", nEtaBins, etaMin, etaMax); 

  background->Draw("abs(scEta)>>histBkg", cut, "goff");
  signal->Draw("abs(scEta)>>histSig", cut, "goff");
  signal->Draw("abs(scEta)>>histSigRw", cutW, "goff");

  // histBkg->Sumw2();   // Stores the sum squared of the weights 
  // histSig->Sumw2(); // Normalises the errors with the weights 
  // histSigRw->Sumw2();

  // Normalising histograms 
  histBkg->Scale(1./histBkg->Integral());
  histSig->Scale(1./histSig->Integral());
  histSigRw->Scale(1./histSigRw->Integral());

  Double_t maxValue = max(histSig->GetBinContent(histSig->GetMaximumBin()), histBkg->GetBinContent(histBkg->GetMaximumBin()))*1.2;


  // Setting style fot background 
  histBkg->SetMinimum(0.);
  histBkg->SetMarkerColor(kRed);
  histBkg->SetLineColor(kRed);
  histBkg->SetLineWidth(2);  
  histBkg->SetMarkerStyle(20);
  histBkg->SetMarkerSize(0.7);


  // Setting style for signal 
  histSig->SetMinimum(0.);
  histSig->SetLineColor(kBlue);
  histSig->SetLineWidth(2);
  histSig->SetMarkerColor(kBlue);
  histSig->SetMarkerStyle(20);
  histSig->SetMarkerSize(0.7);


  // Setting style for reweighted signal 
  histSigRw->SetMarkerColor(kGreen+1);
  histSigRw->SetLineColor(kGreen+1);
  histSigRw->SetLineWidth(2);
  histSigRw->SetMarkerStyle(20);
  histSigRw->SetMarkerSize(0.7);


  histoStack->Add(histSig); 
  histoStack->Add(histBkg); 
  histoStack->Add(histSigRw); 



  TLegend *legendEta = new TLegend(0.15,0.15,.45,0.45,"","brNDC");
  if(isEndcap) legendEta->SetHeader("ECAL Endcap");
  else legendEta->SetHeader("ECAL Barrel");

  legendEta->SetBorderSize(0);
  legendEta->SetFillStyle(0);
  legendEta->SetTextFont(42);

  legendEta->AddEntry(histBkg,"bkg","lem");
  legendEta->AddEntry(histSig,"sig","lem");
  legendEta->AddEntry(histSigRw,"sig * 2D weight","lem");

  

       
  canvasEta->cd(); 

  histoStack->Draw("nostack"); 

  legendEta->Draw("same");



  // canvasEta->cd();
  // histBkg->SetTitle("");
  // histBkg->SetStats(0);
  // histBkg->GetXaxis()->SetTitle("p_{T}");
  // Double_t maxSig = histSig->GetBinContent(histSig->GetMaximumBin());
  // Double_t maxBkg = histBkg->GetBinContent(histBkg->GetMaximumBin()); 
  // Double_t maxValue = max(maxSig, maxBkg)*1.2;
  // histBkg->SetMaximum(maxValue);
  // histBkg->Draw();
  // histSig->Draw("EPsame");    // EPsame : 
  // histSigRw->Draw("EPsame");


  

  TLatex *txt = new TLatex(0.2, 0.9, "");
  // txt->SetTextSize(0.05);
  txt->DrawLatexNDC(0.1, 0.91, "CMS #bf{#it{#scale[0.8]{Simulation Preliminary}}}");
  txt->DrawLatexNDC(0.76, 0.91, "#bf{13 TeV}");
  txt->Draw("same");

  histoStack->GetXaxis()->SetTitle("sc #eta"); 
  canvasEta->Update();
  canvasEta->Modified();
    

  TString outname = outputfilename+"Eta"; 
  if (isEndcap) outname+="Endcap"; 
  else outname+="Barrel"; 

  canvasEta->SaveAs(outname+".pdf");
  canvasEta->SaveAs(outname+".root");
  canvasEta->SaveAs(outname+".png");
  

}


  


  


}
