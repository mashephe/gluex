// $Id$
//
//    File: JEventProcessor_FCALHitOmega.cc
// Created: Wed Jul 13 09:42:29 EDT 2022
// Creator: mashephe (on Linux stanley-1.physics.indiana.edu 3.10.0-1160.71.1.el7.x86_64 x86_64)
//

#include "JEventProcessor_FCALHitOmega.h"
using namespace jana;

#include "FCAL/DFCALShower.h"
#include "FCAL/DFCALHit.h"
#include "FCAL/DFCALGeometry.h"

#include "ANALYSIS/DAnalysisResults.h"
#include "ANALYSIS/DParticleCombo.h"


#include "DFactoryGenerator_OmegaSkim.h"

#include "ANALYSIS/DTreeInterface.h"

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
extern "C"{
  void InitPlugin(JApplication *app){
    InitJANAPlugin(app);
    app->AddProcessor(new JEventProcessor_FCALHitOmega());
    app->AddFactoryGenerator( new DFactoryGenerator_OmegaSkim() );

  }
} // "C"


//------------------
// JEventProcessor_FCALHitOmega (Constructor)
//------------------
JEventProcessor_FCALHitOmega::JEventProcessor_FCALHitOmega()
{

}

//------------------
// ~JEventProcessor_FCALHitOmega (Destructor)
//------------------
JEventProcessor_FCALHitOmega::~JEventProcessor_FCALHitOmega()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_FCALHitOmega::init(void)
{
  // This is called once at program startup.

  japp->RootWriteLock();

  m_treeSh = new TTree( "showerHits", "Shower Hits" );

  m_treeSh->Branch( "eSh" , &m_eSh , "eSh/F"  );
  m_treeSh->Branch( "xSh" , &m_xSh , "xSh/F"  );
  m_treeSh->Branch( "ySh" , &m_ySh , "ySh/F"  );
  m_treeSh->Branch( "dtSh", &m_dtSh, "dtSh/F" );
  m_treeSh->Branch( "nHitSh", &m_nHitSh, "nHitSh/I" );
  m_treeSh->Branch( "xHitSh", m_xHitSh , "xHitSh[nHitSh]/F" );
  m_treeSh->Branch( "yHitSh", m_yHitSh , "yHitSh[nHitSh]/F" );
  m_treeSh->Branch( "eHitSh", m_eHitSh , "eHitSh[nHitSh]/F" );
  m_treeSh->Branch( "dtHitSh", m_dtHitSh, "dtHitSh[nHitSh]/F" );

  m_treeEvt = new TTree( "allHits", "All Hits" );
  m_treeEvt->Branch( "omegaMassEvt" , &m_omegaMassEvt, "omegaMassEvt/F" );
  m_treeEvt->Branch( "nShEvt"  , &m_nShEvt,  "nShEvt/I" );
  m_treeEvt->Branch( "nHitEvt" , &m_nHitEvt, "nHitEvt/I" );
  m_treeEvt->Branch( "xHitEvt" , m_xHitEvt , "xHitEvt[nHitEvt]/F" );
  m_treeEvt->Branch( "yHitEvt" , m_yHitEvt , "yHitEvt[nHitEvt]/F" );
  m_treeEvt->Branch( "eHitEvt" , m_eHitEvt , "eHitEvt[nHitEvt]/F" );
  m_treeEvt->Branch( "dtHitEvt", m_dtHitEvt, "dtHitEvt[nHitEvt]/F" );

  japp->RootUnLock();
  
  return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_FCALHitOmega::brun(JEventLoop *eventLoop, int32_t runnumber)
{
  // This is called whenever the run number changes

  DApplication *dapp = dynamic_cast<DApplication*>(eventLoop->GetJApplication());
  const DGeometry *geom = dapp->GetDGeometry( runnumber );

  if( geom ) {

    geom->GetTargetZ( m_zTarget );
    geom->GetFCALZ( m_FCALUpstream );
  }
  else{

    cerr << "No geometry accessbile." << endl;
    return RESOURCE_UNAVAILABLE;
  }

  return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_FCALHitOmega::evnt(JEventLoop *loop, uint64_t eventnumber)
{

  vector<const DAnalysisResults*> analysisResultsVector;
  loop->Get( analysisResultsVector );

  vector<const DAnalysisUtilities*> utilityVec;
  loop->Get( utilityVec );
  const DAnalysisUtilities* analysisUtilities = utilityVec[0];
	
  vector<const DEventRFBunch*> eventRFBunches;
  loop->Get( eventRFBunches );

  double startTime = eventRFBunches.empty() ? 0.0 :
    eventRFBunches[0]->dTime;

  vector< const DFCALHit*> allHits;
  loop->Get( allHits );

  vector< const DFCALShower* > allShowers;
  loop->Get( allShowers );
  
  DLorentzVector omegaCandP4;
  list<const DFCALShower*> pi0Showers;

  DVector3 targetCenter( 0, 0, m_zTarget );
  
  // First let's deal with events where one and only one exclusive omega is reconstructed in
  // the pi+ pi- pi0 decay. We will extract the P4 of this omega and also a list of pi0 showers
  // in the FCAL
  
  for( vector< const DAnalysisResults* >::const_iterator res = analysisResultsVector.begin();
       res != analysisResultsVector.end(); ++res ){

    const DReaction* reaction = (**res).Get_Reaction();

    if( reaction->Get_ReactionName() != "p3pi_excl" ) continue;

    size_t nCombos = (**res).Get_NumPassedParticleCombos();

    // this is a strict cut, but we really want clean events
    if( nCombos != 1 ) return NOERROR;    
    deque< const DParticleCombo* > combos;
    (**res).Get_PassedParticleCombos( combos );
    const DParticleCombo* combo = combos[0];

    // this shoud be the kinematically fit reconstructed omega four vector
    omegaCandP4 = analysisUtilities->Calc_FinalStateP4( reaction, combo, 1, true);

    // now let's make a list of pi0 (from omega) showers in the FCAL
    vector< const JObject* > neutralSource = 
      combo->Get_FinalParticle_SourceObjects( d_Neutral );

    for( size_t i = 0; i < neutralSource.size(); ++i ){
      
      const DNeutralShower* neutShower =
	dynamic_cast< const DNeutralShower* >( neutralSource[i] );
      
      if( neutShower == NULL ) continue;  // shouldn't happen if sources are neutral

      if( neutShower->dDetectorSystem == SYS_FCAL ){

	pi0Showers.push_back( dynamic_cast< const DFCALShower* >( neutShower->dBCALFCALShower ) );
      }
    }
  }

  // from here down we need to run execute single thread 
  japp->RootFillLock( this );
  
  for( list<const DFCALShower*>::const_iterator shItr = pi0Showers.begin();
       shItr != pi0Showers.end(); ++shItr ){

    const DFCALShower* shower = *shItr;

    vector< const DFCALHit* > hitVec = getHitsFromShower( shower );

    DVector3 fcalPos = shower->getPosition();			
    double pathLength = ( fcalPos - targetCenter ).Mag();
    m_dtSh =  shower->getTime() - pathLength/29.9792458 - startTime;

    m_eSh = shower->getEnergy();
    m_xSh = shower->getPosition().X();
    m_ySh = shower->getPosition().Y();
    
    m_nHitSh = hitVec.size();
    for( size_t iHit = 0; iHit < hitVec.size(); ++iHit ){

      const DFCALHit* hit = hitVec[iHit];
      
      m_xHitSh[iHit] = hit->x;
      m_yHitSh[iHit] = hit->y;
      m_eHitSh[iHit] = hit->E;
      m_dtHitSh[iHit] = hit->t - shower->getTime();
    }

    m_treeSh->Fill();
  }

  m_omegaMassEvt = omegaCandP4.Mag();
  m_nShEvt = pi0Showers.size();
  m_nHitEvt = allHits.size();
  for( size_t iHit = 0; iHit < allHits.size(); ++iHit ){

      const DFCALHit* hit = allHits[iHit];
      
      m_xHitEvt[iHit] = hit->x;
      m_yHitEvt[iHit] = hit->y;
      m_eHitEvt[iHit] = hit->E;
      m_dtHitEvt[iHit] = hit->t - startTime;
  }

  m_treeEvt->Fill();

  japp->RootFillUnLock(this);

  return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_FCALHitOmega::erun(void)
{
  // This is called whenever the run number changes, before it is
  // changed to give you a chance to clean up before processing
  // events from the next run number.
  return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_FCALHitOmega::fini(void)
{
  // Called before program exit after event processing is finished.
  return NOERROR;
}

vector< const DFCALHit* >
JEventProcessor_FCALHitOmega::getHitsFromShower( const DFCALShower* shower ) const {

	vector< const DFCALHit* > allHits;

	vector< const DFCALCluster* > clusterVec;
	shower->Get( clusterVec );

	// I don't think there should ever be more than one cluster
	// associated with a shower, but I will code this anyway....
	for( unsigned int i = 0; i < clusterVec.size(); ++i ){

		vector< const DFCALHit* > hits;
		clusterVec[i]->Get( hits );
		allHits.insert( allHits.end(), hits.begin(), hits.end() );
	}

	return allHits;
}
