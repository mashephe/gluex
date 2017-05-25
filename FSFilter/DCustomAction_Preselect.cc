// $Id$
//
//    File: DCustomAction_Preselect.cc
// Created: Mon Jul 21 14:42:17 EDT 2014
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-358.6.2.el6.x86_64 x86_64)
//

#include "DCustomAction_Preselect.h"

void DCustomAction_Preselect::Initialize(JEventLoop* locEventLoop)
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

    string pt( name + "; p_{t} [GeV/c]; Combos / 5 MeV/c" );
    if(gDirectory->Get("pt") == NULL) 
      ptHist = new TH1D("pt", pt.c_str(), 200, 0.0, 1.0);
    else 
      ptHist = static_cast<TH1D*>(gDirectory->Get("pt"));
 
    string pBal( name + "; p_{Det} - p_{Beam} [GeV/c]; Combos / 20 MeV/c" );
    if(gDirectory->Get("pBal") == NULL) 
      pBalHist = new TH1D("pBal", pBal.c_str(), 400, -4, 4);
    else 
      pBalHist = static_cast<TH1D*>(gDirectory->Get("pBal"));
  }
  japp->RootUnLock(); //RELEASE ROOT LOCK!!
}

bool DCustomAction_Preselect::Perform_Action(JEventLoop* evtLoop, const DParticleCombo* partCombo )
{
  
  const DParticleComboStep* initialStep = partCombo->Get_ParticleComboStep( 0 );

  DLorentzVector beam = initialStep->Get_InitialParticle_Measured()->lorentzMomentum();
  DLorentzVector target = initialStep->Get_TargetParticle()->lorentzMomentum();

  // true means to skip the first particle in the step, which is the recoil nucleon
  DLorentzVector mesonSysP4 = totalStepP4( partCombo, initialStep, false, true );
  DLorentzVector totalP4 = totalStepP4( partCombo, initialStep, false, false );

  double delpz = totalP4.Vect().Z() - beam.E();

  bool select;

  if( initialStep->Is_FinalParticleMissing( 0 ) == false ){
    
    // selection criteria for detected recoil proton
    
    select = ( ( fabs( delpz ) < 2.0 ) && 
	       ( totalP4.Pt() < 0.3 ) );
  }
  else{

    // seleciton criteria for missing recoil nucleon

    select = true;
  }

  japp->RootWriteLock(); //ACQUIRE ROOT LOCK!!
  {
    // Fill any histograms here
    ptHist->Fill( totalP4.Pt() );
    pBalHist->Fill( delpz );
  }
  japp->RootUnLock(); //RELEASE ROOT LOCK!!

  return select;
}

DLorentzVector
DCustomAction_Preselect::totalStepP4( const DParticleCombo* combo, 
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

