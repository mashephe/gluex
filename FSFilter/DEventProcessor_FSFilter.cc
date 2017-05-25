// $Id$
//
//    File: DEventProcessor_FSFilter.cc
// Created: Mon Jul 21 14:42:17 EDT 2014
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-358.6.2.el6.x86_64 x86_64)
//

#include "DEventProcessor_FSFilter.h"
#include "PID/DChargedTrack.h"

#ifdef VTRACE
#include "vt_user.h"
#endif

// Routine used to create our DEventProcessor

extern "C"
{
  void InitPlugin(JApplication *locApplication)
  {
    InitJANAPlugin(locApplication);
    locApplication->AddProcessor(new DEventProcessor_FSFilter());
    locApplication->AddFactoryGenerator(new DFactoryGenerator_FSFilter());
  }
} // "C"

//------------------
// init
//------------------
jerror_t DEventProcessor_FSFilter::init(void)
{
  // This is called once at program startup. If you are creating
  // and filling historgrams in this plugin, you should lock the
  // ROOT mutex like this:
  //
  // japp->RootWriteLock();
  //  ... create historgrams or trees ...
  // japp->RootUnLock();
  //

  dEventWriterROOT = NULL;
  dEventWriterREST = NULL;

  return NOERROR;
}

//------------------
// brun
//------------------
jerror_t DEventProcessor_FSFilter::brun(jana::JEventLoop* locEventLoop, int locRunNumber)
{
  // This is called whenever the run number changes

  /*
  //Optional: Retrieve REST writer for writing out skims
  locEventLoop->GetSingle(dEventWriterREST);
  */

  locEventLoop->GetSingle(dEventWriterROOT);
  dEventWriterROOT->Create_DataTrees(locEventLoop);
 
  return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t DEventProcessor_FSFilter::evnt(jana::JEventLoop* locEventLoop, int locEventNumber)
{

#ifdef VTRACE
  VT_TRACER( "DEventProcesor_FSFilter::evnt" );
#endif

  // This is called for every event. Use of common resources like writing
  // to a file or filling a histogram should be mutex protected. Using
  // locEventLoop->Get(...) to get reconstructed objects (and thereby activating the
  // reconstruction algorithm) should be done outside of any mutex lock
  // since multiple threads may call this method at the same time.
  //
  // Here's an example:
  //
  // vector<const MyDataClass*> mydataclasses;
  // locEventLoop->Get(mydataclasses);
  //
  // japp->RootWriteLock();
  //  ... fill historgrams or trees ...
  // japp->RootUnLock();

  /*  Remove cut on maximum number of showers

  vector< const DNeutralParticle* > neutralPartVec;
  locEventLoop->Get( neutralPartVec );
  int numShowers = 0;

  double photonFOMCut = 5.7E-7;
  double photonECut = 0.1;

  for( vector< const DNeutralParticle*>::const_iterator neutPart = neutralPartVec.begin();
       neutPart != neutralPartVec.end();
       ++neutPart ){
    
    if( ( (**neutPart).Get_BestFOM() == NULL ) ||
	( (**neutPart).dNeutralShower == NULL ) ) continue;
    
    if( ( (**neutPart).Get_BestFOM()->dFOM > photonFOMCut ) &&
	( (**neutPart).dNeutralShower->dEnergy > photonECut ) ) ++numShowers;
  }

  if( numShowers > 8 ) return NOERROR;
  */

  // DOCUMENTATION:
  // ANALYSIS library: https://halldweb1.jlab.org/wiki/index.php/GlueX_Analysis_Software

  //Recommended: Write surviving particle combinations (if any) to output ROOT TTree
  //If no cuts are performed by the analysis actions added to a DReaction, then this saves all of its particle combinations. 
  //The event writer gets the DAnalysisResults objects from JANA, performing the analysis. 
  // string is DReaction factory tag: will fill trees for all DReactions that are defined in the specified factory
  dEventWriterROOT->Fill_DataTrees( locEventLoop, "FSFilter" );
 
  /*
  //Optional: Get all particle combinations for all DReactions. 
  //If kinematic fits were requested, these contain both the measured and kinematic-fit track parameters
  //No cuts from DAnalysisActions are placed on these combos
  vector<const DParticleCombo*> locParticleCombos;
  locEventLoop->Get(locParticleCombos);
  for(size_t loc_i = 0; loc_i < locParticleCombos.size(); ++loc_i)
  {
  const DParticleCombo* locParticleCombo = locParticleCombos[loc_i];
  if(locParticleCombo->Get_Reaction()->Get_ReactionName() != "FSFilter")
  continue; // particle combination was for a different reaction
  //perform further analysis steps here...
  }
  */

  /*
  //Optional: Get the analysis results for all DReactions. 
  //Getting these objects triggers the analysis, if it wasn't performed already. 
  //These objects contain the DParticleCombo objects that survived the DAnalysisAction cuts that were added to the DReactions
  vector<const DAnalysisResults*> locAnalysisResultsVector;
  locEventLoop->Get(locAnalysisResultsVector);
  for(size_t loc_i = 0; loc_i < locAnalysisResultsVector.size(); ++loc_i)
  {
  const DAnalysisResults* locAnalysisResults = locAnalysisResultsVector[loc_i];
  if(locAnalysisResults->Get_Reaction()->Get_ReactionName() != "FSFilter")
  continue; // analysis results were for a different reaction
  //get the DParticleCombo objects for this DReaction that survived all of the DAnalysisAction cuts
  deque<const DParticleCombo*> locPassedParticleCombos;
  locAnalysisResults->Get_PassedParticleCombos(locPassedParticleCombos);
  for(size_t loc_j = 0; loc_j < locPassedParticleCombos.size(); ++loc_j)
  {
  const DParticleCombo* locPassedParticleCombo = locPassedParticleCombos[loc_j];
  //perform further analysis steps here...
  }
  }
  */

  /*
  //Optional: Save event to output REST file. Use this to create skims.
  dEventWriterREST->Write_RESTEvent(locEventLoop, "FSFilter"); //string is part of output file name
  */

  return NOERROR;
}

//------------------
// erun
//------------------
jerror_t DEventProcessor_FSFilter::erun(void)
{
  // This is called whenever the run number changes, before it is
  // changed to give you a chance to clean up before processing
  // events from the next run number.
  return NOERROR;
}

//------------------
// fini
//------------------
jerror_t DEventProcessor_FSFilter::fini(void)
{
  // Called before program exit after event processing is finished.
  return NOERROR;
}

