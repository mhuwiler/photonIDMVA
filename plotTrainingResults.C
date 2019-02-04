#include <string>
#include <iostream>
#include <map>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TH2D.h"
#include "TGraph2D.h"


void setStyle(TGraph* graph) {
	Double_t titleSize = 0.05; 
	Double_t labelSize = 0.04; 
	graph->GetXaxis()->SetTitleSize(titleSize);
	graph->GetXaxis()->SetTitleOffset(1.05);
	graph->GetXaxis()->SetLabelSize(labelSize); 
	graph->GetYaxis()->SetTitleSize(titleSize);
	graph->GetYaxis()->SetTitleOffset(1.05);
	graph->GetYaxis()->SetLabelSize(labelSize); 

	graph->SetLineWidth(2); 
	graph->SetMarkerSize(1.2); 
	graph->SetMarkerStyle(8); 


	TObject* isCurrentCanvas = gPad->GetPrimitive((graph->GetName())); 	// Making sure the graph is plot in gPad 
	if (isCurrentCanvas) 
	{
		gPad->SetLeftMargin(0.11); 
		gPad->SetBottomMargin(0.11); 
		//gPad->SetRightMargin(0.15); 
		gPad->Modified();  
    	gPad->Update();

	} 
}
	

void plotMultipleGraphs(vector<TGraph*> graphs, TCanvas* canvas) {

	//TCanvas *canvas = new TCanvas("canvas", "ROC curves", 800, 600); 
	vector<Int_t> colors = {1, 4, 2, 3, 6, 7, 5, 9, 8, 15}; 

	canvas->cd(); 

	TGraph* curve1 = graphs.at(0); 
	curve1->SetTitle("");
	curve1->SetLineColor(kWhite);	//blue
	curve1->SetMarkerColor(kWhite);
	//curve1->GetYaxis()->SetRangeUser(0.055, 0.2)	// TSetting axis range must be done on the first graph to be plotted (with potion A);
	
	TGraph *curve2 = graphs.at(1);
	curve2->SetMarkerColor(2);	//red
	curve2->SetLineColor(2);

	TGraph *curve3 = graphs.at(2);
	curve3->SetMarkerColor(3);	//red
	curve3->SetLineColor(3);

	TGraph *curve4 = graphs.at(3);
	curve4->SetMarkerColor(1);	//red
	curve4->SetLineColor(1);

	//TGraph *curve3 = graphs.at(2); 
	//curve3->SetMarkerColor(3); 
	//curve3->SetLineColor(3); 

	TLegend *legend = new TLegend(0.11, 0.11, 0.7, 0.3);
	legend->AddEntry(curve1, "Current (retrained)", "p");
	legend->AddEntry(curve2, "Best from grid scan", "p");
	//legend->AddEntry(curve3, "XGBoost with bayesian optimisation", "p"); 
	legend->AddEntry(curve3, "XGBoost w. Bayesian optimization", "p");
	legend->AddEntry(curve4, "Current in production (benchmark)", "p");
	legend->SetTextSize(0.04);

	
	//setStyle(curve3); 
	
	curve1->Draw("AP.");
	curve2->Draw("P.");
	curve3->Draw("P."); 
	curve4->Draw("P."); 
	setStyle(curve1);
	setStyle(curve2);
	setStyle(curve3); 
	setStyle(curve4); 
	//curve3->Draw("P.");
	legend->Draw();
	canvas->Modified(); 
	canvas->Update(); 
	//canvas->Draw();

	//return canvas

} 
	
void plotTrainingResults (bool isEndcap = false) {
	
	// Setting the files where the OC curves are 
	TString currentTraining; 
	TString currentFlashgg; 
	TString bestFromOptimisation; 
	TString xgboostTraining; 
	if (isEndcap) {
		currentTraining = "$EOSPATH/software/PhotonID/photonIDMVA/development/getroc/ROCexisting.root"; 
		currentFlashgg = "$EOSPATH/data/isodata/PhotonID/ROC/ROCexistingFggEndcap.root"; 
		bestFromOptimisation = "/afs/cern.ch/work/m/mhuwiler/lsfjobs/ScanFirstRunEndcap/nTrees500nodeSize8_0treeDepth8nCuts2000separationGiniIndexboostTypeGradlearningRate0_1boostSecond0nVars0pruneMethodCostComplexitypruneStrength5Endcap/plots.root"; 
		xgboostTraining = "/afs/cern.ch/work/m/mhuwiler/lsfjobs/XgboWeightSecondRunSmallEndcap/plots.root"; 
	}
	else {
		currentTraining = "$EOSPATH/software/PhotonID/photonIDMVA/development/getroc/ROCexisting.root"; 
		currentFlashgg = "$EOSPATH/data/isodata/PhotonID/ROC/ROCexistingFggBarrel.root"; 
		bestFromOptimisation = "/afs/cern.ch/work/m/mhuwiler/lsfjobs/ScanFirstRunBarrel/nTrees100nodeSize0_5treeDepth2nCuts2000separationGiniIndexboostTypeGradlearningRate0_01boostSecond0nVars0pruneMethodCostComplexitypruneStrength5Barrel/plots.root"; 
		xgboostTraining = "/afs/cern.ch/work/m/mhuwiler/lsfjobs/XgboWeightSecondRunBarrel/plots.root"; 
	}


	TFile *currentFile = TFile::Open(currentTraining, "READ"); 
	TFile *bestFile = TFile::Open(bestFromOptimisation, "READ"); 
	TFile *xgboFile = TFile::Open(xgboostTraining, "READ"); 
	TFile *existingFile = TFile::Open(currentFlashgg, "READ"); 
	TGraph *ROCcurrent = static_cast<TGraph*>(currentFile->Get("TMVA-like_ROC")); 
	TGraph *ROCbest = static_cast<TGraph*>(bestFile->Get("TMVA-like_ROC")); 
	TGraph *ROCxgboost= static_cast<TGraph*>(xgboFile->Get("xgboOptimized/TMVA-like_ROC")); 
	TGraph *ROCexisting = static_cast<TGraph*>(existingFile->Get("TMVA-like_ROC")); 


	TCanvas *canvas = new TCanvas("canvas", "ROC for PhotonID MVA (Barrel)", 800., 600.);

	vector<TGraph*> graphs = {ROCcurrent, ROCbest, ROCxgboost, ROCexisting }; 
	//vector<TGraph*> graphs = {ROCcurrent, ROCbest }; 

	plotMultipleGraphs(graphs, canvas); 
	//ROCcurrent->Draw(); 
	//ROCcurrent->SetMarkerColor(3); 
	canvas->Update(); 
	canvas->Print("figures/ROCresults.root");
	canvas->Print("figures/ROCresults.pdf"); 	 
	canvas->Print("figures/ROCresults.png"); 

	//delete canvas; 


	//currentFile->Close(); 
	//bestFile->Close(); 
	//xgboFile->Close(); 
	//existingFile->Close(); 


}