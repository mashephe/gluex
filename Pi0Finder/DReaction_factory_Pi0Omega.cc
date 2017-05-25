// $Id$
//
//    File: DReaction_factory_Pi0Omega.cc
// Created: Fri Jul 11 14:18:49 EDT 2014
// Creator: mashephe (on Darwin 149-160-160-51.dhcp-bl.indiana.edu 13.3.0 i386)
//


#include "DReaction_factory_Pi0Omega.h"

//------------------
// init
//------------------
jerror_t DReaction_factory_Pi0Omega::init(void)
{
  // Make as many DReaction objects as desired
  DReactionStep* locReactionStep = NULL;
  DReaction* locReaction = new DReaction("Pi0Omega"); //needs to be a unique name for each DReaction object, CANNOT (!) be "Thrown"

  // DOCUMENTATION:
  // ANALYSIS library: https://halldweb1.jlab.org/wiki/index.php/GlueX_Analysis_Software
  // DReaction factory: https://halldweb1.jlab.org/wiki/index.php/Analysis_DReaction

  /**************************************************** Pi0Omega Reaction Steps ****************************************************/

   
  //Required: DReactionSteps to specify the channel and decay chain you want to study
  //Particles are of type Particle_t, an enum defined in sim-recon/src/libraries/include/particleType.h

  //Example: g, p -> pi+, pi-, pi0, (p)
  locReactionStep = new DReactionStep();
  locReactionStep->Set_InitialParticleID(Gamma);
  locReactionStep->Set_TargetParticleID(Proton);
  locReactionStep->Add_FinalParticleID(omega);
  locReactionStep->Add_FinalParticleID(Proton); 
  locReaction->Add_ReactionStep(locReactionStep);
  dReactionStepPool.push_back(locReactionStep); //register so will be deleted later: prevent memory leak

  locReactionStep = new DReactionStep();
  locReactionStep->Set_InitialParticleID(omega);
  locReactionStep->Add_FinalParticleID(PiPlus);
  locReactionStep->Add_FinalParticleID(PiMinus);
  locReactionStep->Add_FinalParticleID(Pi0);
  locReaction->Add_ReactionStep(locReactionStep);
  dReactionStepPool.push_back(locReactionStep); //register so will be deleted later: prevent memory leak
  
  //Example: pi0 -> g, g
  locReactionStep = new DReactionStep();
  locReactionStep->Set_InitialParticleID(Pi0);
  locReactionStep->Add_FinalParticleID(Gamma);
  locReactionStep->Add_FinalParticleID(Gamma);
  
  //optional: in a p4 kinematic fit, this will disable the constraint on the mass of the pi0
  //default is enabled, but you don't need to worry about setting false for the beam particle step
  //note that the omega and phi will always be false, regardless of what you set below
  //  locReactionStep->Set_ApplyKinFitMassConstraintOnInitialParticleFlag(true);

  locReaction->Add_ReactionStep(locReactionStep);
  dReactionStepPool.push_back(locReactionStep); //register so will be deleted later: prevent memory leak  
  
  /**************************************************** Pi0Omega Control Variables ****************************************************/

  // Recommended: Type of kinematic fit to perform (default is d_NoFit)
  //fit types are of type DKinFitType, an enum defined in sim-recon/src/libraries/ANALYSIS/DKinFitResults.h
  locReaction->Set_KinFitType(d_P4AndVertexFit); 
  //locReaction->Set_KinFitType(d_P4Fit); 

  //simultaneously constrain apply four-momentum conservation, invariant masses, and common-vertex constraints

  // Highly Recommended: When generating particle combinations, reject all photon candidates with a PID confidence level < 5.73303E-7 (+/- 5-sigma)
  locReaction->Set_MinPhotonPIDFOM(5.73303E-7);

  // Highly Recommended: When recording thrown/reconstructed matches in ROOT TTree, only register matches with match FOM > 5.73303E-7 (+/- 5-sigma)
  locReaction->Set_MinThrownMatchFOMForROOT(5.73303E-7);
  
  // Recommended: Enable ROOT TTree output for this DReaction
  //  locReaction->Enable_TTreeOutput("tree_Pi0Omega.root"); //string is file name (must end in ".root"!!): doen't need to be unique, feel free to change

  /**************************************************** Pi0Omega Analysis Actions ****************************************************/
    
  // Recommended: Analysis actions automatically performed by the DAnalysisResults factories to histogram useful quantities.
  // These actions are executed sequentially, and are executed on each surviving (non-cut) particle combination 
  // Pre-defined actions can be found in ANALYSIS/DHistogramActions.h and ANALYSIS/DCutActions.h

  // PID & Kinematics
  //  locReaction->Add_AnalysisAction(new DHistogramAction_PID(locReaction));
  //  locReaction->Add_AnalysisAction(new DHistogramAction_TruePID(locReaction)); 
  

  // Transverse Momentum //Recommended for no-missing-particle reactions only!
  // locReaction->Add_AnalysisAction(new DCutAction_TransverseMomentum(locReaction, 0.4)); //Max Missing Pt of 0.4 GeV

  locReaction->Add_AnalysisAction( new DCutAction_InvariantMass( locReaction, Pi0, false, 0.09, 0.19 ) );
  locReaction->Add_AnalysisAction( new DCutAction_InvariantMass( locReaction, omega, false, 0.6, 1.2 ) );
  locReaction->Add_AnalysisAction( new DCutAction_MissingMass( locReaction, false, -0.5, 0.5 ) );

  locReaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( locReaction, Pi0, false, 100, 0.09, 0.19, "pi0" ) );
  locReaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( locReaction, omega, false, 100, 0.6, 1.2, "omega" ) );
  locReaction->Add_AnalysisAction( new DHistogramAction_MissingMass( locReaction, false, 100, -1.0, 1.0 ) );

  locReaction->Add_AnalysisAction( new DHistogramAction_KinFitResults( locReaction, 0.05 ) );
  locReaction->Add_AnalysisAction( new DCutAction_KinFitFOM( locReaction, 0.5 ) );

  locReaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( locReaction, Pi0, true, 100, 0.09, 0.19, "pi0_PostFit" ) );
  locReaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( locReaction, omega, true, 100, 0.6, 1.2, "omega_PostFit" ) );
  locReaction->Add_AnalysisAction( new DHistogramAction_MissingMass( locReaction, true, 100, -1.0, 1.0, "PostFit" ) );

  locReaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( locReaction, Pi0, false, 100, 0.09, 0.19, "pi0_PostFitMeas" ) );
  locReaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( locReaction, omega, false, 100, 0.6, 1.2, "omega_PostFitMeas" ) );
  locReaction->Add_AnalysisAction( new DHistogramAction_MissingMass( locReaction, false, 100, -1.0, 1.0, "PostFitMeas" ) );  

  locReaction->Add_AnalysisAction( new DCutAction_ThrownTopology( locReaction, true ) );

  //  locReaction->Add_AnalysisAction( new DHistogramAction_KinFitResults( locReaction, 0.05, "Signal" ) );

  locReaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( locReaction, Pi0, false, 100, 0.09, 0.19, "pi0_PostFitMeas_Signal" ) );
  locReaction->Add_AnalysisAction( new DHistogramAction_InvariantMass( locReaction, omega, false, 100, 0.6, 1.2, "omega_PostFitMeas_Signal" ) );
  locReaction->Add_AnalysisAction( new DHistogramAction_MissingMass( locReaction, false, 100, -1.0, 1.0, "PostFitMeas_Signal" ) );

  // Kinematics
  //  locReaction->Add_AnalysisAction(new DHistogramAction_ParticleComboKinematics(locReaction, false)); 
  //true: fill histograms with kinematic-fit particle data //"KinFit": unique name since action type is repeated
  //  locReaction->Add_AnalysisAction(new DHistogramAction_ParticleComboKinematics(locReaction, true, "KinFit")); 
  
  _data.push_back(locReaction); //Register the DReaction with the factory
  
  return NOERROR;
}

//------------------
// fini
//------------------
jerror_t DReaction_factory_Pi0Omega::fini(void)
{
  for(size_t loc_i = 0; loc_i < dReactionStepPool.size(); ++loc_i)
    delete dReactionStepPool[loc_i]; //cleanup memory
  return NOERROR;
}

