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


// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_ShowerShapeTree());
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
  
  m_tree->Branch( "typeSh", &m_typeSh, "typeSh/I" );
  m_tree->Branch( "xSh", &m_xSh, "xSh/F" );
  m_tree->Branch( "ySh", &m_ySh, "ySh/F" );
  m_tree->Branch( "eSh", &m_eSh, "eSh/F" );
  m_tree->Branch( "tSh", &m_tSh, "tSh/F" );

  m_tree->Branch( "qTr", &m_qTr, "qTr/I" );
  m_tree->Branch( "docaTr", &m_docaTr, "docaTr/F" );
  m_tree->Branch( "dtTr", &m_dtTr, "dtTr/F" );
  m_tree->Branch( "pTr", &m_pTr, "pTr/F" );
  
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
	// This is called for every event. Use of common resources like writing
	// to a file or filling a histogram should be mutex protected. Using
	// loop->Get(...) to get reconstructed objects (and thereby activating the
	// reconstruction algorithm) should be done outside of any mutex lock
	// since multiple threads may call this method at the same time.
	// Here's an example:
	//
	// vector<const MyDataClass*> mydataclasses;
	// loop->Get(mydataclasses);
	//
	// japp->RootFillLock(this);
	//  ... fill historgrams or trees ...
	// japp->RootFillUnLock(this);

  vector<const DAnalysisResults*> analysisResultsVector;
  loop->Get( analysisResultsVector );
  
  vector<const DDetectorMatches*> detectorMatchesVector;
  loop->Get( detectorMatchesVector );
  const DDetectorMatches* matches = detectorMatchesVector[0];
  
  vector<const DFCALShower*> showerVector;
  loop->Get( showerVector );
  
  for( vector< const DAnalysisResults* >::const_iterator res = analysisResultsVector.begin();
      res != analysisResultsVector.end(); ++res ){
    
    if( (**res).Get_Reaction()->Get_ReactionName() != "p3pi_excl" ) continue;
    
    size_t nCombos = (**res).Get_NumPassedParticleCombos();
    deque< const DParticleCombo* > combos;
    (**res).Get_PassedParticleCombos( combos );
    
    vector<DFCALShowerMatchParams> matchedShowers;
    map<const DFCALShower*, int> showerMatchIndex;
    map<const DFCALShower*, const DChargedTrackHypothesis*> showerMatchTrack;
    list<const DFCALShower*> pi0Showers;
    
    for( size_t iCombo = 0; iCombo < nCombos; ++iCombo){
     
      deque< const DChargedTrack* > chargedTracks;
      deque< const DNeutralShower* > neutralShowers;
      
      combos[iCombo]->Get_DetectedFinalChargedParticles_SourceObjects( chargedTracks );
      combos[iCombo]->Get_DetectedFinalNeutralParticles_SourceObjects( neutralShowers );

      // something is wrong if this doesn't check out
      assert( chargedTracks.size() == 2 && neutralShowers.size() == 2 );
      
      const DChargedTrackHypothesis* pipTrack = ( chargedTracks[0]->Get_Charge() > 0 ?
                                                 chargedTracks[0]->Get_Hypothesis( PiPlus ) :
                                                 chargedTracks[1]->Get_Hypothesis( PiPlus ) );
      const DChargedTrackHypothesis* pimTrack = ( chargedTracks[0]->Get_Charge() < 0 ?
                                                 chargedTracks[0]->Get_Hypothesis( PiMinus ) :
                                                 chargedTracks[1]->Get_Hypothesis( PiMinus ) );

      for( size_t i = 0; i < neutralShowers.size(); ++i ){
        
        if( neutralShowers[i]->dDetectorSystem == SYS_FCAL ){
          
          pi0Showers.push_back( dynamic_cast< const DFCALShower* >( neutralShowers[i]->dBCALFCALShower ) );
        }
      }
      
      vector<DFCALShowerMatchParams> thisMatch;
      matches->Get_FCALMatchParams( pipTrack, thisMatch );
      for( size_t i = 0; i < thisMatch.size(); ++i ){
        
        const DFCALShower* showerPtr = thisMatch[i].dFCALShower;
        // skip this match if it isn't the closest shower to the track
        if( ( showerMatchIndex.find( showerPtr ) != showerMatchIndex.end() ) &&
            ( matchedShowers[showerMatchIndex[showerPtr]].dDOCAToShower <
             thisMatch[i].dDOCAToShower ) ) continue;
        
        showerMatchTrack[showerPtr] = pipTrack;
        showerMatchIndex[showerPtr] = matchedShowers.size();
        matchedShowers.push_back( thisMatch[i] );
      }
      
      thisMatch.clear();
      matches->Get_FCALMatchParams( pimTrack, thisMatch );
      for( size_t i = 0; i < thisMatch.size(); ++i ){
        
        const DFCALShower* showerPtr = thisMatch[i].dFCALShower;
        // skip this match if it isn't the closest shower to the track
        if( ( showerMatchIndex.find( showerPtr ) != showerMatchIndex.end() ) &&
           ( matchedShowers[showerMatchIndex[showerPtr]].dDOCAToShower <
            thisMatch[i].dDOCAToShower ) ) continue;
        
        showerMatchTrack[showerPtr] = pimTrack;
        showerMatchIndex[showerPtr] = matchedShowers.size();
        matchedShowers.push_back( thisMatch[i] );
      }
    }
    
    // entering the tree-filling, thread-unsafe, region...
    
    japp->RootFillLock(this);
    
    for( size_t iShower = 0; iShower < showerVector.size(); ++iShower ){
      
      const DFCALShower* shower = showerVector[iShower];
       
      if( showerMatchIndex.find( shower ) != showerMatchIndex.end() ){
        
        DFCALShowerMatchParams match = matchedShowers[showerMatchIndex[shower]];
        
        m_typeSh = 1;
        m_docaTr = match.dDOCAToShower;
        m_dtTr = match.dFlightTime - shower->getTime();
        m_pTr = showerMatchTrack[shower]->pmag();
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
      
      fillHitsFromShower( shower );
      
      m_tree->Fill();
      
      japp->RootFillUnLock(this);

    }
  }

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

