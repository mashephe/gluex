// $Id$
//
//    File: DCustomAction_HistMesonSys.cc
// Created: Mon Jul 21 14:42:17 EDT 2014
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-358.6.2.el6.x86_64 x86_64)
//

#include "DCustomAction_HistMesonSys.h"

void DCustomAction_HistMesonSys::Initialize(JEventLoop* locEventLoop)
{
  //Optional: Create histograms and/or modify member variables.
  //Create any histograms/trees/etc. within a ROOT lock. 
  //This is so that when running multithreaded, only one thread is writing to the ROOT file at a time. 

  japp->RootWriteLock(); //ACQUIRE ROOT LOCK!!
  {
    // Optional: Useful utility functions.
    locEventLoop->GetSingle(dAnalysisUtilities);

    // Required: Create a folder in the ROOT output file that will contain
    //  all of the output ROOT objects (if any) for this action.
    //  If another thread has already created the folder, it just changes to it. 
    CreateAndChangeTo_ActionDirectory();

    string name = Get_Reaction()->Get_DetectedParticlesROOTName();

    // create histograms:

    string invMass( name + " (All); Invariant Mass [GeV/c^{2}]; Combos / 20 MeV/c^{2}");
    if(gDirectory->Get("InvariantMass") == NULL) 
      dMassHist = new TH1D("InvariantMass", invMass.c_str(), 175, 0.0, 3.5);
    else 
      dMassHist = static_cast<TH1D*>(gDirectory->Get("InvariantMass"));
    string invMassBkg( name + " (Bkg); Invariant Mass [GeV/c^{2}]; Combos / 20 MeV/c^{2}");
    if(gDirectory->Get("InvariantMassBkg") == NULL) 
      dMassHistBkg = new TH1D("InvariantMassBkg", invMassBkg.c_str(), 175, 0.0, 3.5);
    else 
      dMassHistBkg = static_cast<TH1D*>(gDirectory->Get("InvariantMassBkg"));

    string t( name + " (All); -t [GeV^{2}]; Combos / 10 MeV^{2}" );
    if(gDirectory->Get("t") == NULL) 
      dtHist = new TH1D("t", t.c_str(), 150, 0.0, 1.5);
    else 
      dtHist = static_cast<TH1D*>(gDirectory->Get("t"));
    string tBkg( name + " (Bkg); -t [GeV^{2}]; Combos / 10 MeV^{2}" );
    if(gDirectory->Get("tBkg") == NULL) 
      dtHistBkg = new TH1D("tBkg", tBkg.c_str(), 150, 0.0, 1.5);
    else 
      dtHistBkg = static_cast<TH1D*>(gDirectory->Get("tBkg"));

    string pt( name + " (All); p_{t} [GeV/c]; Combos / 5 MeV/c" );
    if(gDirectory->Get("pt") == NULL) 
      dptHist = new TH1D("pt", pt.c_str(), 100, 0.0, 1.0);
    else 
      dptHist = static_cast<TH1D*>(gDirectory->Get("pt"));
    string ptBkg( name + " (Bkg); p_{t} [GeV/c]; Combos / 5 MeV/c" );
    if(gDirectory->Get("ptBkg") == NULL) 
      dptHistBkg = new TH1D("ptBkg", ptBkg.c_str(), 100, 0.0, 1.0);
    else 
      dptHistBkg = static_cast<TH1D*>(gDirectory->Get("ptBkg"));
 
    string e( name + " (All); E^{Det} [GeV]; Combos / 15 MeV" );
    if(gDirectory->Get("e") == NULL) 
      deHist = new TH1D("e", e.c_str(), 200, 7.0, 10.0);
    else 
      deHist = static_cast<TH1D*>(gDirectory->Get("e"));
    string eBkg( name + " (Bkg); E^{Det} [GeV]; Combos / 15 MeV" );
    if(gDirectory->Get("eBkg") == NULL) 
      deHistBkg = new TH1D("eBkg", eBkg.c_str(), 200, 7.0, 10.0);
    else 
      deHistBkg = static_cast<TH1D*>(gDirectory->Get("eBkg"));

    string pBal( name + " (All); p^{Det}_{z} - p^{Beam} [GeV/c]; Combos / 20 MeV/c" );
    if(gDirectory->Get("pBal") == NULL) 
      dpBalHist = new TH1D("pBal", pBal.c_str(), 200, -2, 2);
    else 
      dpBalHist = static_cast<TH1D*>(gDirectory->Get("pBal"));
    string pBalBkg( name + " (Bkg); p^{Det}_{z} - p^{Beam} [GeV/c]; Combos / 20 MeV/c" );
    if(gDirectory->Get("pBalBkg") == NULL) 
      dpBalHistBkg = new TH1D("pBalBkg", pBalBkg.c_str(), 200, -2, 2);
    else 
      dpBalHistBkg = static_cast<TH1D*>(gDirectory->Get("pBalBkg"));

    string mm2( name + " (All); (Missing Mass)^{2} [(GeV/c^{2})^{2}]; Combos / 15 (GeV/c^{2})^{2}" );
    if(gDirectory->Get("mm2") == NULL) 
      dmm2Hist = new TH1D("mm2", mm2.c_str(), 200, -1.0, 2.0);
    else 
      dmm2Hist = static_cast<TH1D*>(gDirectory->Get("mm2"));
    string mm2Bkg( name + " (Bkg); (Missing Mass)^{2} [(GeV/c^{2})^{2}]; Combos / 15 (GeV/c^{2})^{2}" );
    if(gDirectory->Get("mm2Bkg") == NULL) 
      dmm2HistBkg = new TH1D("mm2Bkg", mm2Bkg.c_str(), 200, -1.0, 2.0);
    else 
      dmm2HistBkg = static_cast<TH1D*>(gDirectory->Get("mm2Bkg"));

    dMassHistBkg->SetFillColor( kYellow );
    dtHistBkg->SetFillColor( kYellow );
    dptHistBkg->SetFillColor( kYellow );
    deHistBkg->SetFillColor( kYellow );
    dpBalHistBkg->SetFillColor( kYellow );
    dmm2HistBkg->SetFillColor( kYellow );  
  }
  japp->RootUnLock(); //RELEASE ROOT LOCK!!
}

bool DCustomAction_HistMesonSys::Perform_Action(JEventLoop* evtLoop, const DParticleCombo* partCombo )
{
  //Optional: check whether the user wanted to use the kinematic fit results when performing this action
  bool useKinFit = Get_UseKinFitResultsFlag();
  
  const DParticleComboStep* initialStep = partCombo->Get_ParticleComboStep( 0 );

  const DKinematicData* beamKD = ( useKinFit ? initialStep->Get_InitialParticle() :
				   initialStep->Get_InitialParticle_Measured() );

  if( beamKD == NULL ){
    cout << "ERROR:  invalid beam object " << endl 
	 << "        (trying to access kinematic fit data before fit?)" << endl;
    abort();
  }

  DLorentzVector beam = beamKD->lorentzMomentum();
  DLorentzVector target = initialStep->Get_TargetParticle()->lorentzMomentum();

  // true means to skip the first particle in the step, which is the recoil nucleon
  DLorentzVector mesonSysP4 = totalStepP4( partCombo, initialStep, useKinFit, true );
  DLorentzVector totalP4 = totalStepP4( partCombo, initialStep, useKinFit, false );

  bool signal = dAnalysisUtilities->Check_ThrownsMatchReaction( evtLoop, Get_Reaction(), true );

  japp->RootWriteLock(); //ACQUIRE ROOT LOCK!!
  {
    // Fill any histograms here
    dMassHist->Fill( mesonSysP4.M() );
    dtHist->Fill( ( beam - mesonSysP4 ).M2() );
    dptHist->Fill( totalP4.Pt() );
    deHist->Fill( totalP4.E() );
    dpBalHist->Fill( totalP4.Vect().Z() - beam.E() );
    dmm2Hist->Fill( ( beam + target - mesonSysP4 ).M2() );

    if( !signal ){
      
      dMassHistBkg->Fill( mesonSysP4.M() );
      dtHistBkg->Fill( ( beam - mesonSysP4 ).M2() );
      dptHistBkg->Fill( totalP4.Pt() );
      deHistBkg->Fill( totalP4.E() );
      dpBalHistBkg->Fill( totalP4.Vect().Z() - beam.E() );
      dmm2HistBkg->Fill( ( beam + target - mesonSysP4 ).M2() );
    }
  }
  japp->RootUnLock(); //RELEASE ROOT LOCK!!

  return true;
}

DLorentzVector
DCustomAction_HistMesonSys::totalStepP4( const DParticleCombo* combo, 
					  const DParticleComboStep* step,
					  bool useKinFit, bool skipFirst ) const {

  DLorentzVector totalP4;

  // for the initial step, we want to skip the first particle in the list
  // because, by convention, it is always set to be the recoil nucleon

  for( unsigned int i = ( skipFirst ? 1 : 0 ); 
       i < step->Get_NumFinalParticles(); ++i ){

    int stepIndex = step->Get_DecayStepIndex( i );

    if( stepIndex >= 0 ){ 

      totalP4 += totalStepP4( combo, combo->Get_ParticleComboStep( stepIndex ), useKinFit );
    }
    else{

      if( useKinFit ){

	const DKinematicData* kdPart = step->Get_FinalParticle( i );
	if( kdPart == NULL ){

	  cout << "ERROR:  trying use kinematic fit results when no kinematic fit has been done." << endl;
	  abort();
	}
	
	totalP4 += kdPart->lorentzMomentum();
      }
      else{

	totalP4 += step->Get_FinalParticle_Measured( i )->lorentzMomentum();
      }
    }			      
  }

  return totalP4;
}


