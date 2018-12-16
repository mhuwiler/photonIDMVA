// @(#)root/tmva $Id$
/**********************************************************************************
 * Project   : TMVA - a ROOT-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVAClassification                                                 *
 *                                                                                *
 * This macro provides examples for the training and testing of the               *
 * TMVA classifiers.                                                              *
 *                                                                                *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 *                                                                                *
 * The methods to be used can be switched on and off by means of booleans, or     *
 * via the prompt command, for example:                                           *
 *                                                                                *
 *    root -l ./TMVAClassification.C\(\"Fisher,Likelihood\"\)                     *
 *                                                                                *
 * (note that the backslashes are mandatory)                                      *
 * If no method given, a default set of classifiers is used.                      *
 *                                                                                *
 * The output file "TMVA.root" can be analysed with the use of dedicated          *
 * macros (simply say: root -l <macro.C>), which can be conveniently              *
 * invoked through a GUI that will appear at the end of the run of this macro.    *
 * Launch the GUI via the command:                                                *
 *                                                                                *
 *    root -l ./TMVAGui.C                                                         *
 *                                                                                *
 **********************************************************************************/

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

//#include "TMVAGui.C"

//#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
//#endif


void OptimizeBDTtrainingEB( Int_t nTrees, Double_t nodeSize, Int_t maxDepth, TString boostType, Int_t nCuts, TString transformations = "" )
{
    
   TString myMethodList = "";
    Double_t learningRateBoost = 0.5;
    
    TString  runname = TString::Format("OptimizeBDTnTrees%dnodeSize%.2fmaxDepth%dnCuts%dboostType"+boostType+"transformations"+transformations, nTrees, nodeSize, maxDepth, nCuts);
    cout << runname;
    
   // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
   // if you use your private .rootrc, or run from a different directory, please copy the
   // corresponding lines from .rootrc

   // methods to be processed can be given as an argument; use format:
   //
   // mylinux~> root -l TMVAClassification.C\(\"myMethod1,myMethod2,myMethod3\"\)
   //
   // if you like to use a method via the plugin mechanism, we recommend using
   //
   // mylinux~> root -l TMVAClassification.C\(\"P_myMethod\"\)
   // (an example is given for using the BDT as plugin (see below),
   // but of course the real application is when you write your own
   // method based)

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();
   TMVA::PyMethodBase::PyInitialize(); 

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassification" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // --- Here the preparation phase begins

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   TString outfileName( "output.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );
    
    

   TMVA::Factory *factory = new TMVA::Factory( runname, outputFile, "!V:!Silent:Color:DrawProgressBar:Transformations=:AnalysisType=Classification" );
  
   TMVA::DataLoader *dataloader = new TMVA::DataLoader("model");

   dataloader->AddVariable( "SCRawE", 'F' );  
   dataloader->AddVariable( "r9", 'F' );
   dataloader->AddVariable( "sigmaIetaIeta", 'F' );
   dataloader->AddVariable( "etaWidth", 'F' );
   dataloader->AddVariable( "phiWidth", 'F' );
   dataloader->AddVariable( "covIEtaIPhi", 'F' ); 
   dataloader->AddVariable( "s4", 'F' );
   dataloader->AddVariable( "phoIso03", 'F' );
   //dataloader->AddVariable( "phoIsoCorr", 'F' );
   dataloader->AddVariable( "chgIsoWrtChosenVtx", 'F' );
   dataloader->AddVariable( "chgIsoWrtWorstVtx", 'F' );
   dataloader->AddVariable( "scEta", 'F' );
   dataloader->AddVariable( "rho", 'F' );  

   TString fname = "$EOSPATH/data/added/PhotonID/output_GJet_Combined_DoubleEMEnriched_TuneCP5_13TeV_Pythia8_reweighted_split.root";
   TFile *input = TFile::Open( fname );                 
   TTree *signalTrain     = (TTree*)input->Get("SignalTrain");
   TTree *backgroundTrain = (TTree*)input->Get("BackgroundTrain");
    TTree *signalTest     = (TTree*)input->Get("SignalTest");
    TTree *backgroundTest = (TTree*)input->Get("BackgroundTest");

   dataloader->SetSignalWeightExpression( "weight*PtvsEtaWeight" );
   dataloader->SetBackgroundWeightExpression( "weight*PtvsEtaWeight" );

    
    Double_t globalSigWeight = 1.0;
    Double_t globalBkgWeight = 1.0;
    
    dataloader->AddSignalTree(signalTrain, globalSigWeight, TMVA::Types::kTraining);
    dataloader->AddBackgroundTree(backgroundTrain, globalBkgWeight, TMVA::Types::kTraining);
    dataloader->AddSignalTree(signalTest, globalSigWeight, TMVA::Types::kTesting);
    dataloader->AddBackgroundTree(backgroundTest, globalBkgWeight, TMVA::Types::kTesting);
    
    
   
   TCut mycuts = "abs(scEta)>0 && abs(scEta)<1.5 && s4>-2 && s4<2";
   TCut mycutb = "abs(scEta)>0 && abs(scEta)<1.5 && s4>-2 && s4<2";

    dataloader->PrepareTrainingAndTestTree( mycuts, mycutb, "nTrain_Signal=1000:nTrain_Background=1000:nTest_Signal=1000:nTest_Background=1000:SplitMode=Random:NormMode=EqualNumEvents" ); //NumEvents
    // old : nTrain_Signal=1000:nTrain_Background=1000:SplitMode=Random:NormMode=EqualNumEvents:!V  // tested : nTrain_Signal=1000:nTrain_Background=1000:nTest_Signal=1000:nTest_Background=1000:SplitMode=Alternate:NormMode=EqualNumEvents:!V SplitMode=Random:SplitSeed=25
    
    TString trainingstring = TString::Format("!H:!V:NTrees=%d:MinNodeSize=%f%:MaxDepth=%d:nCuts=%d:BoostType="+boostType+":SeparationType=GiniIndex:VarTransform="+transformations, nTrees, nodeSize, maxDepth, nCuts);
    //"!H:!V:NTrees=2000:BoostType=Grad:Shrinkage=0.10:!UseBaggedGrad:nCuts=2000:UseNvars=4:PruneStrength=5:PruneMethod=CostComplexity:MaxDepth=6"
    // old :  H:V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate
    // For DNN : trainingstring = 'H:!V:FilenameModel='+modelfilename+':VarTransform='+transformations + ':NumEpochs={}'.format(numEpochs) + ':BatchSize={}'.format(batchSize) + ':TriesEarlyStopping={}'.format(triesEarlyStopping)
    
    factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT", trainingstring );
    

   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // --------------------------------------------------------------

   // Save the output
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;

   delete factory;
    
    exit(0); 

}
