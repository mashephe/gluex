// $Id$
//
//    File: DReaction_factory_Pi0Inclusive.cc
// Created: Fri Jul 11 14:18:49 EDT 2014
// Creator: mashephe (on Darwin 149-160-160-51.dhcp-bl.indiana.edu 13.3.0 i386)
//


#include "DReaction_factory_Pi0Inclusive.h"

//------------------
// init
//------------------
jerror_t DReaction_factory_Pi0Inclusive::init(void)
{
  // Make as many DReaction objects as desired
  DReactionStep* locReactionStep = NULL;
  //needs to be a unique name for each DReaction object, CANNOT (!) be "Thrown"
  DReaction* locReaction = new DReaction("Pi0Inclusive");

  // DOCUMENTATION:
  // ANALYSIS library: https://halldweb1.jlab.org/wiki/index.php/GlueX_Analysis_Software
  // DReaction factory: https://halldweb1.jlab.org/wiki/index.php/Analysis_DReaction

  //Required: DReactionSteps to specify the channel and decay chain you want to study
  //Particles are of type Particle_t, an enum defined in sim-recon/src/libraries/include/particleType.h

  //Example: pi0 -> g, g
  locReactionStep = new DReactionStep();
  locReactionStep->Set_InitialParticleID(Pi0);
  locReactionStep->Add_FinalParticleID(Gamma);
  locReactionStep->Add_FinalParticleID(Gamma);

  //optional: in a p4 kinematic fit, this will disable the constraint on the mass of the pi0
  //default is enabled, but you don't need to worry about setting false for the beam particle step
  //note that the omega and phi will always be false, regardless of what you set below
	
  locReactionStep->Set_ApplyKinFitMassConstraintOnInitialParticleFlag(false);

  locReaction->Add_ReactionStep(locReactionStep);
  dReactionStepPool.push_back(locReactionStep); //register so will be deleted later: prevent memory leak

  // Highly Recommended: When generating particle combinations, reject all
  // photon candidates with a PID confidence level < 5.73303E-7 (+/- 5-sigma)
  //  locReaction->Set_MinPhotonPIDFOM(5.73303E-7);

  // Highly Recommended: When recording thrown/reconstructed matches in ROOT
  // TTree, only register matches with match FOM > 5.73303E-7 (+/- 5-sigma)
  locReaction->Set_MinThrownMatchFOMForROOT(5.73303E-7);

  // Recommended: Enable ROOT TTree output for this DReaction
  //  locReaction->Enable_TTreeOutput("tree_Pi0Inclusive.root");
  
  /**************************************************** Pi0Inclusive Analysis Actions ****************************************************/

	
  // Recommended: Analysis actions automatically performed by the DAnalysisResults factories to histogram useful quantities.
  //These actions are executed sequentially, and are executed on each surviving (non-cut) particle combination
  //Pre-defined actions can be found in ANALYSIS/DHistogramActions.h and ANALYSIS/DCutActions.h

  locReaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( locReaction, Pi0, false, 100, 0.05, 0.22 ) );

  locReaction->Add_AnalysisAction( new DCutAction_InvariantMass( locReaction, Pi0, false,
								 0.080, 0.190 ) );
  locReaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( locReaction, Pi0, false, 100, 0.05, 0.22, "AfterCut" ) );

  // Kinematics
  locReaction->Add_AnalysisAction(new DHistogramAction_ParticleComboKinematics(locReaction, false));

  _data.push_back(locReaction); //Register the DReaction with the factory

  return NOERROR;
}

//------------------
// fini
//------------------
jerror_t DReaction_factory_Pi0Inclusive::fini(void)
{
  for(size_t loc_i = 0; loc_i < dReactionStepPool.size(); ++loc_i)
    delete dReactionStepPool[loc_i]; //cleanup memory
  return NOERROR;
}

