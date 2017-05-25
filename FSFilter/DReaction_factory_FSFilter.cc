// $Id$
//
// File: DReaction_factory_FSFilter.cc
// Created: Mon Jul 21 14:42:17 EDT 2014
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-358.6.2.el6.x86_64 x86_64)
//

#include "DReaction_factory_FSFilter.h"
#include "DCustomAction_HistMesonSys.h"
#include "DCustomAction_Preselect.h"
#include "DFSConfig.h"

//------------------
// init
//------------------
jerror_t DReaction_factory_FSFilter::init(void)
{
  vector< DFSConfig > finalStates;
  
  // manual setup for now -- this would be better placed
  // in a configuration file that could be parsed
  /*
  for (int charge = 0; charge <= 1; charge++){
    for (int nkp  = 0; nkp  <= 2; nkp++){
      for (int nkm  = 0; nkm  <= 2; nkm++){
	for (int nks  = 0; nks  <= 1; nks++){
	  for (int npip = 0; npip <= 3; npip++){
	    for (int npim = 0; npim <= 3; npim++){
	      for (int npi0 = 0; npi0 <= 2; npi0++){

		int nt = npip + npim + nkp + nkm + 2*nks;
		int q = npip - npim + nkp - nkm;
		int s = abs(nkp - nkm);

		for (int i = 0; i < nks; i++){
		  if (s == 0){ s++; }
		  else       { s--; }
		}

		int npi = npip + npim + npi0;
		int nk = nkp + nkm + nks;

		// trim the list for now:
		if ( nk > 4 ) continue;
		if ( charge > 0 ) continue;

		if (nt < 2) continue;
		if (q != charge) continue;
		if (s != 0) continue;
		if (npi + nk > 6) continue;

		int code1 = nkp *100000 +
		  nkm *10000 +
		  nks *1000 +
		  npip*100 +
		  npim*10 +
		  npi0;

		ostringstream name;		
		name << "FS_0_" << code1 << "_Q=" << charge;

		map< Particle_t, int > fs;
		fs[KPlus]   = nkp;
		fs[KMinus]  = nkm;
		fs[KShort]  = nks;
		fs[PiPlus]  = npip;
		fs[PiMinus] = npim;
		fs[Pi0]     = npi0;

		finalStates.push_back( DFSConfig( name.str(), fs, 
						  ( charge == 1 ? Neutron : Proton ),
						  ( charge == 1 ? DFSConfig::kMissingN : 
						    DFSConfig::kFoundN ) ) );
	      }
	    }
	  }
	}
      }
    }
  }
  */

  
  map< Particle_t, int > mesSys;

  mesSys[PiPlus]  = 1;
  mesSys[PiMinus] = 1;
  finalStates.push_back( DFSConfig( "Pi+Pi-P", mesSys, Proton, DFSConfig::kFoundN ) );
  mesSys.clear();
  
  mesSys[PiPlus]  = 1;
  mesSys[PiMinus] = 1;
  mesSys[Pi0] = 1;
  finalStates.push_back( DFSConfig( "Pi+Pi-Pi0P", mesSys, Proton, DFSConfig::kFoundN ) );
  mesSys.clear();
  
  mesSys[KPlus]  = 1;
  mesSys[KMinus] = 1;
  finalStates.push_back( DFSConfig( "K+K-P", mesSys, Proton, DFSConfig::kFoundN ) );
  mesSys.clear();
  
  mesSys[KPlus]  = 1;
  mesSys[KMinus] = 1;
  mesSys[Pi0] = 1;
  finalStates.push_back( DFSConfig( "K+K-Pi0P", mesSys, Proton, DFSConfig::kFoundN ) );
  mesSys.clear();
  
  mesSys[PiPlus]  = 2;
  mesSys[PiMinus] = 2;
  finalStates.push_back( DFSConfig( "2(Pi+Pi-)P", mesSys, Proton, DFSConfig::kFoundN ) );
  mesSys.clear();
  
  mesSys[PiPlus]  = 2;
  mesSys[PiMinus] = 2;
  mesSys[Pi0] = 1;
  finalStates.push_back( DFSConfig( "2(Pi+Pi-)Pi0P", mesSys, Proton, DFSConfig::kFoundN ) );
  mesSys.clear();
  
  mesSys[PiPlus]  = 3;
  mesSys[PiMinus] = 3;
  finalStates.push_back( DFSConfig( "3(Pi+Pi-)P", mesSys, Proton, DFSConfig::kFoundN ) );
  mesSys.clear();  

  //  the above should probably come from a function like this:
  //  vector< DFSConfig > finalStates = parseConfig( fileName );


  for( vector< DFSConfig >::iterator fs = finalStates.begin(); 
       fs != finalStates.end(); ++fs ){

    DReactionStep* reactionStep = NULL;
    DReaction* reaction = new DReaction( fs->name() );

    reaction->Set_MaxExtraGoodTracks( 4 );

    reactionStep = new DReactionStep();
    reactionStep->Set_InitialParticleID(Gamma);
    reactionStep->Set_TargetParticleID(Proton);
    reactionStep->Add_FinalParticleID( fs->recoil(), fs->recMiss() );

    map< Particle_t, int > mesonSystem = fs->mesonSys();

    for( map< Particle_t, int >::iterator part = mesonSystem.begin();
	 part != mesonSystem.end(); ++part ){

      for( int i = 0; i < part->second; ++i )
	reactionStep->Add_FinalParticleID( part->first );
    }
    reaction->Add_ReactionStep( reactionStep );
    dReactionStepPool.push_back( reactionStep );

    if( mesonSystem[Pi0] > 0 ){

      DReactionStep* pi0Step = new DReactionStep();
      pi0Step->Set_InitialParticleID( Pi0 );
      pi0Step->Add_FinalParticleID( Gamma );
      pi0Step->Add_FinalParticleID( Gamma );
      //      pi0Step->Set_ApplyKinFitMassConstraintOnInitialParticleFlag( false );
      for( int i = 0; i < mesonSystem[Pi0]; ++i ) {

	reaction->Add_ReactionStep( pi0Step );
      }
      dReactionStepPool.push_back( pi0Step );
    }

    if( mesonSystem[Eta] > 0 ){

      DReactionStep* etaStep = new DReactionStep();
      etaStep->Set_InitialParticleID( Eta );
      etaStep->Add_FinalParticleID( Gamma );
      etaStep->Add_FinalParticleID( Gamma );
      for( int i = 0; i < mesonSystem[Eta]; ++i ){

	reaction->Add_ReactionStep( etaStep );
      }
      dReactionStepPool.push_back( etaStep );
    }

    if( mesonSystem[KShort] > 0 ){

      DReactionStep* ksStep = new DReactionStep();
      ksStep->Set_InitialParticleID( KShort );
      ksStep->Add_FinalParticleID( PiPlus );
      ksStep->Add_FinalParticleID( PiMinus );
      for( int i = 0; i < mesonSystem[KShort]; ++i ){

	reaction->Add_ReactionStep( ksStep );
      }
      dReactionStepPool.push_back( ksStep );
    }

    /*** FSFilter Control Variables ***/

    reaction->Set_KinFitType(d_P4AndVertexFit);
    
    // Highly Recommended: When generating particle combinations, reject 
    // all photon candidates with a PID confidence level < 5.73303E-7 
    // (+/- 5-sigma)
    reaction->Set_MinPhotonPIDFOM(5.73303E-7);

    // other cuts recommended by Paul:
    reaction->Set_MaxPhotonRFDeltaT(0.5*2.004);
    //    reaction->Set_MinChargedPIDFOM(5.73303E-7);

    // Highly Recommended: When recording thrown/reconstructed matches 
    // in ROOT TTree, only register matches with match FOM > 5.73303E-7
    // (+/- 5-sigma)
    // reaction->Set_MinThrownMatchFOMForROOT(5.73303E-7);

    if( fs->writeROOT() ) reaction->Enable_TTreeOutput( fs->name() + ".root");

    /*** FSFilter Analysis Actions ***/

    // rough pi0, eta, Ks selection cuts
    
    reaction->Set_InvariantMassCut( Pi0,    0.065, 0.205 );
    reaction->Set_InvariantMassCut( Eta,    0.5,   0.6 );
    reaction->Set_InvariantMassCut( KShort, 0.45,  0.55 );
 
    // preselection cuts to reduce combinations
    reaction->Add_ComboPreSelectionAction( new DCustomAction_Preselect( reaction, false ) );  
    
    reaction->Add_AnalysisAction( new DCustomAction_HistMesonSys( reaction, false, "Preselect" ) );

    // now do kinematic fit and require CL > 1E-5
    reaction->Add_AnalysisAction( new DHistogramAction_KinFitResults( reaction, 0.05 ) );
    reaction->Add_AnalysisAction( new DCutAction_KinFitFOM( reaction, 1E-5 ) );

    if( mesonSystem[Pi0] > 0 ){

      reaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( reaction, Pi0, true, 100, 
									0.085, 0.185, "CL1E-5" ) );
      reaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( reaction, Pi0, false, 100, 
									0.085, 0.185, "CL1E-5_Meas" ) );
    }
    if( mesonSystem[Eta] > 0 ){

      reaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( reaction, Eta, true, 100, 
									0.5, 0.6, "CL1E-5" ) );
      reaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( reaction, Eta, false, 100, 
									0.5, 0.6, "CL1E-5_Meas" ) );
    }
    if( mesonSystem[KShort] > 0 ){

      reaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( reaction, KShort, true, 100, 
									0.45, 0.55, "CL1E-5" ) );
      reaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( reaction, KShort, false, 100, 
									0.45, 0.55, "CL1E-5_Meas" ) );
    }

    // make a loose cut on pi0 invariant mass - this will pass for all combos where a 
    // constraint is applied
    reaction->
      Add_AnalysisAction( new DCutAction_InvariantMass( reaction, Pi0, true, 0.105, 0.165 ) );

    reaction->Add_AnalysisAction( new DCustomAction_HistMesonSys( reaction, true, "CL1E-5" ) );
    reaction->Add_AnalysisAction( new DCustomAction_HistMesonSys( reaction, false, "CL1E-5_Meas" ) );
    
    _data.push_back(reaction); //Register the DReaction with the factory
  }

  return NOERROR;
}

//------------------
// fini
//------------------
jerror_t DReaction_factory_FSFilter::fini(void)
{
  for(size_t loc_i = 0; loc_i < dReactionStepPool.size(); ++loc_i)
    delete dReactionStepPool[loc_i]; //cleanup memory
  return NOERROR;
}

