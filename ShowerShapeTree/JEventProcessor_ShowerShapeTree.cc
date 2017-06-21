// $Id$
//
//    File: JEventProcessor_ShowerShapeTree.cc
// Created: Mon Jun  5 14:03:02 EDT 2017
// Creator: mashephe (on Darwin 149-161-212-208.dhcp-bl.indiana.edu 16.6.0 i386)
//

#include "JEventProcessor_ShowerShapeTree.h"
using namespace jana;

#include "TTree.h"

#include "FCAL/DFCALShower.h"
#include "FCAL/DFCALCluster.h"
#include "FCAL/DFCALHit.h"

#include "ANALYSIS/DAnalysisResults.h"
#include "ANALYSIS/DParticleCombo.h"
#include "PID/DChargedTrack.h"
#include "PID/DNeutralShower.h"
#include "PID/DDetectorMatches.h"

#include "DLorentzVector.h"
#include "TTree.h"

#include "DFactoryGenerator_OmegaSkim.h"

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_ShowerShapeTree());
	app->AddFactoryGenerator( new DFactoryGenerator_OmegaSkim() );
}
} // "C"


//------------------
// JEventProcessor_ShowerShapeTree (Constructor)
//------------------
JEventProcessor_ShowerShapeTree::JEventProcessor_ShowerShapeTree()
{

}

//------------------
// ~JEventProcessor_ShowerShapeTree (Destructor)
//------------------
JEventProcessor_ShowerShapeTree::~JEventProcessor_ShowerShapeTree()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_ShowerShapeTree::init(void)
{
  // This is called once at program startup.
  japp->RootWriteLock();
  
  m_tree = new TTree( "fcalShowers", "Showers" );

  m_tree->Branch( "event", &m_event, "event/L" );
  
  m_tree->Branch( "typeSh", &m_typeSh, "typeSh/I" );
  m_tree->Branch( "xSh", &m_xSh, "xSh/F" );
  m_tree->Branch( "ySh", &m_ySh, "ySh/F" );
  m_tree->Branch( "eSh", &m_eSh, "eSh/F" );
  m_tree->Branch( "tSh", &m_tSh, "tSh/F" );
  m_tree->Branch( "disSh", &m_disSh, "disSh/F" );
  m_tree->Branch( "dtSh", &m_dtSh, "dtSh/F" );

  m_tree->Branch( "qTr", &m_qTr, "qTr/F" );
  m_tree->Branch( "docaTr", &m_docaTr, "docaTr/F" );
  m_tree->Branch( "tTr", &m_tTr, "tTr/F" );
  m_tree->Branch( "pTr", &m_pTr, "pTr/F" );
  m_tree->Branch( "xTr", &m_xTr, "xTr/F" );
  m_tree->Branch( "yTr", &m_yTr, "yTr/F" );
  
  m_tree->Branch( "nHits", &m_nHits, "nHits/I" );
  m_tree->Branch( "xHit", m_xHit, "xHit[nHits]/F" );
  m_tree->Branch( "yHit", m_yHit, "yHit[nHits]/F" );
  m_tree->Branch( "eHit", m_eHit, "eHit[nHits]/F" );
  m_tree->Branch( "tHit", m_tHit, "tHit[nHits]/F" );
  
  japp->RootUnLock();
  
  return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_ShowerShapeTree::brun(JEventLoop *eventLoop, int32_t runnumber)
{
	// This is called whenever the run number changes
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_ShowerShapeTree::evnt(JEventLoop *loop, uint64_t eventnumber)
{

  vector<const DAnalysisResults*> analysisResultsVector;
  loop->Get( analysisResultsVector );

  DApplication* app = dynamic_cast<DApplication*>(loop->GetJApplication());
  const DMagneticFieldMap* fieldMap = app->GetBfield(loop->GetJEvent().GetRunNumber());

  for( vector< const DAnalysisResults* >::const_iterator res = analysisResultsVector.begin();
      res != analysisResultsVector.end(); ++res ){

    if( (**res).Get_Reaction()->Get_ReactionName() != "p3pi_excl" ) continue;
    
    size_t nCombos = (**res).Get_NumPassedParticleCombos();

    // this is a strict cut, but we really want clean events
    if( nCombos != 1 ) return NOERROR;    
    deque< const DParticleCombo* > combos;
    (**res).Get_PassedParticleCombos( combos );
    const DParticleCombo* combo = combos[0];

    vector<const DFCALShowerMatchParams*> matchedShowers;
    map<const DFCALShower*, int> showerMatchIndex;
    map<const DFCALShower*, const DKinematicData*> showerMatchTrack;
    list<const DFCALShower*> pi0Showers;
	  	  
    deque< const DNeutralShower* > neutralShowers;      
    combo->Get_DetectedFinalNeutralParticles_SourceObjects( neutralShowers );
    
    for( size_t i = 0; i < neutralShowers.size(); ++i ){
        
      if( neutralShowers[i]->dDetectorSystem == SYS_FCAL ){
	
	pi0Showers.push_back( dynamic_cast< const DFCALShower* >( neutralShowers[i]->dBCALFCALShower ) );
      }
    }

    deque<const DKinematicData*> tracks;
    combo->Get_DetectedFinalChargedParticles( tracks );
    for( size_t iTrk = 0; iTrk < tracks.size(); ++iTrk ){

      const DChargedTrackHypothesis* ctHypo = dynamic_cast<const DChargedTrackHypothesis*>( tracks[iTrk] );
      const DFCALShowerMatchParams* match = ctHypo->Get_FCALShowerMatchParams();

      if( match == NULL ) continue;
      
      const DFCALShower* showerPtr = match->dFCALShower;
      showerMatchTrack[showerPtr] = tracks[iTrk];
      showerMatchIndex[showerPtr] = matchedShowers.size();
      matchedShowers.push_back( match );	
    }
    

    vector<const DFCALShower*> showerVector;
    loop->Get( showerVector );
 
    // entering the tree-filling, thread-unsafe, region...
    
    japp->RootFillLock(this);

    m_event = eventnumber;
    
    for( size_t iShower = 0; iShower < showerVector.size(); ++iShower ){
      
      const DFCALShower* shower = showerVector[iShower];

      DVector3 fcalPos = shower->getPosition();
      DVector3 norm( 0, 0, 1 );
	  
      m_docaTr = 1E6;
      m_tTr = 1E6;
      m_pTr = 0;
      m_qTr = 0;

      double pathLength, flightTime, flightTimeVariance;
      DVector3 projPos, projMom;
      
      // loop over tracks and find the one closest to this shower
      for( size_t iTrk = 0; iTrk < tracks.size(); ++iTrk ){

	DReferenceTrajectory rt( fieldMap );
	rt.SetMass( tracks[iTrk]->mass() );
	rt.q = tracks[iTrk]->charge();
	rt.Swim( tracks[iTrk]->position(), tracks[iTrk]->momentum(), rt.q );
	
	// swim the track to vertical plane the intercepts the shower position
	if( rt.GetIntersectionWithPlane( fcalPos, norm, projPos, projMom, &pathLength,
					 &flightTime, &flightTimeVariance, SYS_FCAL ) !=
	    NOERROR ) continue;

	double distance = ( fcalPos - projPos ).Mag();
	
	if( distance < m_docaTr ){

	  m_docaTr = distance;
	  m_tTr = tracks[iTrk]->t0() + flightTime;
	  m_xTr = projPos.X();
	  m_yTr = projPos.Y();
	  m_pTr = tracks[iTrk]->pmag();
	  m_qTr = tracks[iTrk]->charge();
	}
      }
      
      if( showerMatchIndex.find( shower ) != showerMatchIndex.end() ){

	// showers matched to a track by existing algorithm
        m_typeSh = 1;
      }
      else if( find( pi0Showers.begin(), pi0Showers.end(), shower ) !=
              pi0Showers.end() ){
 
        // showers matched to a pi0
        m_typeSh = 0;
      }
      else{
        
        // showers that are unmatched
        m_typeSh = 2;
      }
      
      m_eSh = shower->getEnergy();
      m_tSh = shower->getTime();
      m_xSh = shower->getPosition().X();
      m_ySh = shower->getPosition().Y();

      m_disSh = ( shower->getPosition() - tracks[0]->position() ).Mag();
      m_dtSh = shower->getTime() - tracks[0]->t0();

      // this hit counter will get incremented in the fill function
      m_nHits = 0;
      fillHitsFromShower( shower );
      
      m_tree->Fill();
    }
  }

  japp->RootFillUnLock(this);
  
  return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_ShowerShapeTree::erun(void)
{
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_ShowerShapeTree::fini(void)
{
  
  // Called before program exit after event processing is finished.
  japp->RootWriteLock();
  m_tree->Write();
  japp->RootUnLock();
 
  return NOERROR;
}

void
JEventProcessor_ShowerShapeTree::fillHitBlocks( const vector< const DFCALHit* >& hitVec ){
  
  // need to be sure to lock the event thread somewhere before
  // entering this function
  
  if( hitVec.size() + m_nHits > kMaxHits ) return;
  
  for( vector< const DFCALHit* >::const_iterator hit = hitVec.begin();
      hit != hitVec.end();
      ++hit ){
    
    m_xHit[m_nHits] = (**hit).x;
    m_yHit[m_nHits] = (**hit).y;
    m_eHit[m_nHits] = (**hit).E;
    m_tHit[m_nHits] = (**hit).t;
    
    ++m_nHits;
  }
}

void
JEventProcessor_ShowerShapeTree::fillHitsFromShower( const DFCALShower* shower ){
  
  
  vector< const DFCALCluster* > clusterVec;
  shower->Get( clusterVec );
  
  // I don't think there should ever be more than one cluster
  // associated with a shower, but I will code this anyway....
  
  for( unsigned int i = 0; i < clusterVec.size(); ++i ){
    
    vector< const DFCALHit* > hits;
    clusterVec[i]->Get( hits );
    fillHitBlocks( hits );
  }
}

