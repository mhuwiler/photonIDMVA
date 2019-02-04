#include <string>
#include <iostream>
#include <map>
#include <vector>

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
	

void plotMultipleGraphs(vector<std::pair<TGraph*, TString> > graphs, TCanvas* canvas) {

	//TCanvas *canvas = new TCanvas("canvas", "ROC curves", 800, 600); 
	vector<Int_t> colors = {1, 4, 2, 3, 6, 7, 5, 9, 8, 15}; 

	if (graphs.size() > 10) {
		std::cout << "You are trying to draw more than 10 graphs on the same plot. You may need a more elaborate color handling. " << std::endl; 
		return; 
	}

	canvas->cd(); 

	TLegend *legend = new TLegend(0.11, 0.11, 0.7, 0.3);

	for (unsigned int i=0; i < graphs.size(); i++) 
	{
		TGraph *curve = graphs.at(i).first; 

		curve->SetTitle("");
		curve->SetLineColor(colors.at(i));
		curve->SetMarkerColor(colors.at(i)); 

		if (i==0) {
			curve->Draw("AP."); 
			//curve->GetYaxis()->SetRangeUser(0.055, 0.2)	// Setting axis range must be done on the first graph to be plotted (with option A);

		}
		else 
		{
			curve->Draw("P.");
		}

		setStyle(curve);
		
		legend->AddEntry(curve, graphs.at(i).second, "p");		
	
	
	

	}

	legend->SetTextSize(0.04);

	

	legend->Draw();
	canvas->Modified(); 
	canvas->Update(); 
	//canvas->Draw();

	
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
		bestFromOptimisation = "/afs/cern.ch/work/m/mhuwiler/lsfjobs/ScanSecondRunBarrel/nTrees100nodeSize0_5treeDepth2nCuts2000separationGiniIndexboostTypeGradlearningRate0_01boostSecond0nVars0pruneMethodCostComplexitypruneStrength5Barrel/plots.root"; 
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

	std::vector<std::pair<TGraph*, TString> > graphs = {	std::pair<TGraph*, TString>(ROCcurrent, "Current (retrained)"), 
															std::pair<TGraph*, TString>(ROCbest, "Best from grid scan"), 
															std::pair<TGraph*, TString>(ROCxgboost, "XGBoost w. Bayesian optimization"), 
															std::pair<TGraph*, TString>(ROCexisting, "Current in production (benchmark)") 
														}; 

	/*vector<TString*> captions = {	"Current (retrained)", 
									"Best from grid scan", 
									"XGBoost w. Bayesian optimization", 
									"Current in production (benchmark)", 
								}; */

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