#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>



const Int_t factor = 10;

void TrainTestSplit() 
{
	TString filename = "$EOSPATH/data/added/PhotonID/output_GJet_Combined_DoubleEMEnriched_TuneCP5_13TeV_Pythia8_reweighted.root"; 
	TFile *initialfile = TFile::Open(filename, "READ"); 
	TFile *outfile = TFile::Open(TString(filename).ReplaceAll(".root", "_smallsplit.root"), "RECREATE"); 

	TTree *signal = static_cast<TTree*>(initialfile->Get("promptPhotons")); 
	TTree *background = static_cast<TTree*>(initialfile->Get("fakePhotons")); 

	// Creating the train and test trees for signal and background. 
	TTree *signalTrain = signal->CloneTree(0);
    signalTrain->SetName("SignalTrain");
	TTree *signalTest = signal->CloneTree(0);
    TTree *signalLeft = signal->CloneTree(0);

	TTree *backgroundTrain = background->CloneTree(0); 
	TTree *backgroundTest = background->CloneTree(0);
    TTree *backgroundLeft = background->CloneTree(0);
    
    signalTest->SetName("SignalTest");
    backgroundTrain->SetName("BackgroundTrain");
    backgroundTest->SetName("BackgroundTest");


	signalTrain->SetAutoSave(-100000); 
	signalTest->SetAutoSave(-100000); 
	backgroundTrain->SetAutoSave(-100000); 
	backgroundTest->SetAutoSave(-100000);
    
    
    // Signal loop
	for (unsigned int i=0; i<signal->GetEntries(); i++ ) 
	{
        if ( i%1000000 == 0 ) cout << "Processing signal entry " << i << " ... " << std::endl;
        
        signal->GetEntry(i);
        if ( i%factor == 0 )
		{
			signalTrain->Fill();
        }
        
        if ( (i%factor)-1 == 0 )
        {
            signalTest->Fill();
        }
        
        else
        {
            //signalLeft->Fill();
        }
        
	}
    
    // Background loop
    for (unsigned int i=0; i<background->GetEntries(); i++ )
    {
        if ( i%1000000 == 0 ) cout << "Processing background entry " << i << " ... " << std::endl;
        
        background->GetEntry(i);
        if ( i%factor == 0 )
        {
            backgroundTrain->Fill();
        }
        
        if ( (i%factor)-1 == 0 )
        {
            backgroundTest->Fill();
        }
        
        else
        {
            //backgroundLeft->Fill();
        }
        
    }

        
    outfile->Write();
        
    outfile->Close();



}
