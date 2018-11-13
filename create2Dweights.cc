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

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <algorithm>

TString filename = "/eos/user/m/mhuwiler/data/added/PhotonID/output_GJet_Combined_DoubleEMEnriched_TuneCP5_13TeV_Pythia8.root";
// /eos/user/m/mhuwiler/data/added/PhotonID/filename.root // Example file

void create2Dweights(){

  const Int_t XBINS = 31;
  Double_t xEdges[32] = {10.0,12.0,14.0,16.0,18.0,20.0,22.0,24.0,26.0,28.0,30.0,32.0,34.0,36.0,38.0,40.0,42.0,44.0,46.0,48.0,50.0,55.0,60.0,65.0,70.0,75.0,80.0,90.0,100.0,150.0,200.0,250.0};

  TH2F * hPt_vs_Eta_EB_Sig = new TH2F ("hPt_vs_EtaEB_Sig","Pt vs eta",XBINS,xEdges,10,0,1.4442);     
  hPt_vs_Eta_EB_Sig->GetXaxis()->SetTitle("Pt_{#gamma} (GeV)");   
  hPt_vs_Eta_EB_Sig->GetYaxis()->SetTitle("#eta_{SC}");

  TH2F * hPt_vs_Eta_EE_Sig = new TH2F ("hPt_vs_EtaEE_Sig","Pt vs eta",XBINS,xEdges,10,1.566,2.5);   
  hPt_vs_Eta_EE_Sig->GetXaxis()->SetTitle("Pt_{#gamma} (GeV)");   
  hPt_vs_Eta_EE_Sig->GetYaxis()->SetTitle("#eta_{SC}");

  TH2F * hPt_vs_Eta_EB_Bkg = new TH2F ("hPt_vs_EtaEB_Bkg","Pt vs eta",XBINS,xEdges,10,0,1.4442);
  hPt_vs_Eta_EB_Bkg->GetXaxis()->SetTitle("Pt_{#gamma} (GeV)");
  hPt_vs_Eta_EB_Bkg->GetYaxis()->SetTitle("#eta_{SC}");

  TH2F * hPt_vs_Eta_EE_Bkg = new TH2F ("hPt_vs_EtaEE_Bkg","Pt vs eta",XBINS,xEdges,10,1.566,2.5);
  hPt_vs_Eta_EE_Bkg->GetXaxis()->SetTitle("Pt_{#gamma} (GeV)");
  hPt_vs_Eta_EE_Bkg->GetYaxis()->SetTitle("#eta_{SC}");

  TFile *File = TFile::Open(filename);
  TDirectory *dir_Photon = (TDirectory*)File->Get("photonViewDumper/trees");

  TTree *t_PromtPhotons = (TTree*)dir_Photon->Get("promptPhotons");
  TTree *t_FakePhotons = (TTree*)dir_Photon->Get("fakePhotons");

  int sigNEvs = t_PromtPhotons->GetEntries();
  int bkgNEvs = t_FakePhotons->GetEntries();

  cout << "sigNEvs = " << sigNEvs << endl;
  cout << "bkgNEvs = " << bkgNEvs << endl;

  float pt_sig;
  float eta_sig;
  float weight_sig;

  float pt_bkg;
  float eta_bkg;
  float weight_bkg;

  t_PromtPhotons->SetBranchAddress("pt",&pt_sig);
  t_PromtPhotons->SetBranchAddress("scEta",&eta_sig);
  t_PromtPhotons->SetBranchAddress("weight",&weight_sig);

  t_FakePhotons->SetBranchAddress("pt",&pt_bkg);
  t_FakePhotons->SetBranchAddress("scEta",&eta_bkg);
  t_FakePhotons->SetBranchAddress("weight",&weight_bkg);

  for(int i = 0; i < sigNEvs; i++){
    t_PromtPhotons->GetEntry(i);

    if(fabs(eta_sig) < 1.4442) {
      hPt_vs_Eta_EB_Sig->Fill(pt_sig,fabs(eta_sig),weight_sig);
    }
   if (fabs(eta_sig)>1.566){
     hPt_vs_Eta_EE_Sig->Fill(pt_sig,fabs(eta_sig),weight_sig);
    }

  }

  cout << "signal 2D histos filled" << endl;

  for(int i = 0; i < bkgNEvs; i++){
    t_FakePhotons->GetEntry(i);

    if(fabs(eta_bkg) < 1.4442) {
      hPt_vs_Eta_EB_Bkg->Fill(pt_bkg,fabs(eta_bkg),weight_bkg);
    }
   if (fabs(eta_bkg)>1.566){
     hPt_vs_Eta_EE_Bkg->Fill(pt_bkg,fabs(eta_bkg), weight_bkg);
    }

  }

  cout << "bkg 2D histos filled" << endl;

  //plot 2D as they are

  TCanvas *c = new TCanvas("c","c");
  c->Divide(2,2);
  c->cd(1);
  hPt_vs_Eta_EB_Sig->Draw("lego");
  c->cd(2);
  hPt_vs_Eta_EB_Bkg->Draw("lego");
  c->cd(3);
  hPt_vs_Eta_EE_Sig->Draw("lego");
  c->cd(4);
  hPt_vs_Eta_EE_Bkg->Draw("lego");

  c->SaveAs("PtvsEta_histos_0thNotTrueVtx.pdf");
  c->SaveAs("PtvsEta_histos_0thNotTrueVtx.png");
  c->SaveAs("PtvsEta_histos_0thNotTrueVtx.root");

  //normalization

  float Sig_bar_Integral = hPt_vs_Eta_EB_Sig->Integral();
  float Bkg_bar_Integral = hPt_vs_Eta_EB_Bkg->Integral();

  float Sig_end_Integral = hPt_vs_Eta_EE_Sig->Integral();
  float Bkg_end_Integral = hPt_vs_Eta_EE_Bkg->Integral();

  cout << "Integral Sig_bar = " << Sig_bar_Integral << endl;
  cout << "Integral Bkg_bar = " << Bkg_bar_Integral << endl;
  cout << "Integral Sig_end = " << Sig_end_Integral << endl;
  cout << "Integral Bkg_end = " << Bkg_end_Integral << endl;

  float SigOverBkg_bar_norm = (Sig_bar_Integral/Bkg_bar_Integral);
  float BkgOverSig_bar_norm = (Bkg_bar_Integral/Sig_bar_Integral);

  float SigOverBkg_end_norm = (Sig_end_Integral/Bkg_end_Integral);
  float BkgOverSig_end_norm = (Bkg_end_Integral/Sig_end_Integral);

  cout << "SigOverBkg_bar_norm = " << SigOverBkg_bar_norm << endl;
  cout << "BkgOverSig_bar_norm = " << BkgOverSig_bar_norm << endl;
  cout << "SigOverBkg_end_norm = " << SigOverBkg_end_norm << endl;
  cout << "BkgOverSig_end_norm = " << BkgOverSig_end_norm << endl;

  TH2F *hSig_bar_norm = (TH2F *)hPt_vs_Eta_EB_Sig->Clone("hSig_bar_norm");
  hSig_bar_norm->Scale(BkgOverSig_bar_norm);

  TH2F *hSig_end_norm = (TH2F *)hPt_vs_Eta_EE_Sig->Clone("hSig_end_norm");
  hSig_end_norm->Scale(BkgOverSig_end_norm);

  TCanvas *cNorm = new TCanvas("cNorm","cNorm");
  cNorm->Divide(2,2);
  cNorm->cd(1);
  hSig_bar_norm->Draw("lego");
  cNorm->cd(2);
  hPt_vs_Eta_EB_Bkg->Draw("lego");
  cNorm->cd(3);
  hSig_end_norm->Draw("lego");
  cNorm->cd(4);
  hPt_vs_Eta_EE_Bkg->Draw("lego");

  cNorm->SaveAs("PtvsEta_normHistos_0thNotTrueVtx.pdf");
  cNorm->SaveAs("PtvsEta_normHistos_0thNotTrueVtx.png");
  cNorm->SaveAs("PtvsEta_normHistos_0thNotTrueVtx.root");

  cout << "Integral of signal norm bar = " << hSig_bar_norm->Integral() << endl;
  cout << "Integral of bkg bar = " << hPt_vs_Eta_EB_Bkg->Integral() << endl;
  cout << "Integral of signal norm end = " << hSig_end_norm->Integral() << endl;
  cout << "Integral of bkg end = " << hPt_vs_Eta_EE_Bkg->Integral() << endl;


  //weights creation

  TH2F *hWeight_bar = (TH2F *)hPt_vs_Eta_EB_Bkg->Clone("hWeight_bar");
  hWeight_bar->Divide(hSig_bar_norm);

  TH2F *hWeight_end = (TH2F *)hPt_vs_Eta_EE_Bkg->Clone("hWeight_end");
  hWeight_end->Divide(hSig_end_norm);

  TCanvas *cWeight = new TCanvas("cWeight","cWeight");
  cWeight->Divide(2,2);
  cWeight->cd(1);
  hWeight_bar->Draw("COLZ");
  cWeight->cd(2);
  hWeight_end->Draw("COLZ");
  cWeight->cd(3);
  hWeight_bar->Draw("lego");
  cWeight->cd(4);
  hWeight_end->Draw("lego");

  cWeight->SaveAs("PtvsEta_2DWeights.pdf");
  cWeight->SaveAs("PtvsEta_2DWeights.png");
  cWeight->SaveAs("PtvsEta_2DWeights.root");

  TFile fOut("/eos/user/m/mhuwiler/data/isodata/PhotonID/Weights_PtVSeta_Hgg_Gjets_all.root","RECREATE");
  fOut.cd();

  hWeight_bar->Write("hWeight_bar");
  hWeight_end->Write("hWeight_end");

  fOut.Close();


}
