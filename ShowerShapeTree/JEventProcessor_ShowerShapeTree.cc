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
#include "FCAL/DFCALGeometry.h"

#include "ANALYSIS/DAnalysisResults.h"
#include "ANALYSIS/DParticleCombo.h"
#include "PID/DChargedTrack.h"
#include "PID/DNeutralShower.h"
#include "PID/DDetectorMatches.h"
#include "TRACKING/DTrackWireBased.h"

#include "DLorentzVector.h"
#include "DVector3.h"
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
JEventProcessor_ShowerShapeTree::JEventProcessor_ShowerShapeTree() : m_fcalGeom()
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
  m_tree->Branch( "t0RF", &m_t0RF, "t0RF/F" );

  m_tree->Branch( "typeSh", &m_typeSh, "typeSh/I" );
  m_tree->Branch( "xSh", &m_xSh, "xSh/F" );
  m_tree->Branch( "ySh", &m_ySh, "ySh/F" );
  m_tree->Branch( "zSh", &m_zSh, "zSh/F" );
  m_tree->Branch( "eSh", &m_eSh, "eSh/F" );
  m_tree->Branch( "tSh", &m_tSh, "tSh/F" );
  m_tree->Branch( "disSh", &m_disSh, "disSh/F" );
  m_tree->Branch( "dtSh", &m_dtSh, "dtSh/F" );
  m_tree->Branch( "dtTrSh", &m_dtTrSh, "dtTrSh/F" );
  m_tree->Branch( "dtTrSh2", &m_dtTrSh2, "dtTrSh2/F" );
  m_tree->Branch( "speedSh", &m_speedSh, "speedSh/F" );
  m_tree->Branch( "depthSh", &m_depthSh, "depthSh/F" );
  m_tree->Branch( "e1e9Sh", &m_e1e9Sh, "e1e9Sh/F" );
  m_tree->Branch( "e9e25Sh", &m_e9e25Sh, "e9e25Sh/F" );
  m_tree->Branch( "sumUSh", &m_sumUSh, "sumUSh/F" );
  m_tree->Branch( "sumVSh", &m_sumVSh, "sumVSh/F" );
  m_tree->Branch( "e1e9Sh2", &m_e1e9Sh2, "e1e9Sh2/F" );
  m_tree->Branch( "e9e25Sh2", &m_e9e25Sh2, "e9e25Sh2/F" );
  m_tree->Branch( "sumUSh2", &m_sumUSh2, "sumUSh2/F" );
  m_tree->Branch( "sumVSh2", &m_sumVSh2, "sumVSh2/F" );
  m_tree->Branch( "asymUVSh", &m_asymUVSh, "asymUVSh/F" );
  m_tree->Branch( "qualSh", &m_qualSh, "qualSh/F" );

  m_tree->Branch( "tCl", &m_tCl, "tCl/F" );
  m_tree->Branch( "eWtTCl", &m_eWtTCl, "eWtTCl/F" );
  m_tree->Branch( "eWtTRMSCl", &m_eWtTRMSCl, "eWtTRMSCl/F" );
  m_tree->Branch( "eCl", &m_eCl, "eCl/F" );
  m_tree->Branch( "disCl", &m_disCl, "disCl/F" );
  
  m_tree->Branch( "qTr", &m_qTr, "qTr/F" );
  m_tree->Branch( "docaTr", &m_docaTr, "docaTr/F" );
  m_tree->Branch( "docaTr2", &m_docaTr2, "docaTr2/F" );
  m_tree->Branch( "tTr", &m_tTr, "tTr/F" );
  m_tree->Branch( "pTr", &m_pTr, "pTr/F" );
  m_tree->Branch( "xTr", &m_xTr, "xTr/F" );
  m_tree->Branch( "yTr", &m_yTr, "yTr/F" );
  
  m_tree->Branch( "nHits", &m_nHits, "nHits/I" );
  m_tree->Branch( "idHit", m_idHit, "idHit[nHits]/I" );
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
jerror_t JEventProcessor_ShowerShapeTree::brun(JEventLoop *loop, int32_t runnumber)
{

  // This is called whenever the run number changes

  DApplication *dapp = dynamic_cast<DApplication*>(loop->GetJApplication());
  const DGeometry *geom = dapp->GetDGeometry(runnumber);
    
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
jerror_t JEventProcessor_ShowerShapeTree::evnt(JEventLoop *loop, uint64_t eventnumber)
{
  vector<const DAnalysisResults*> analysisResultsVector;
  loop->Get( analysisResultsVector );

  vector<const DEventRFBunch*> eventRFBunches;
  loop->Get( eventRFBunches );
  if( eventRFBunches.size() != 1 || eventRFBunches[0]->dNumParticleVotes < 2 ) return NOERROR;

  map< const DFCALShower*, double > showerQualityMap;
  vector< const DNeutralShower* > neutralShowers;
  loop->Get( neutralShowers );
  
  for( size_t i = 0; i < neutralShowers.size(); ++i ){
    
    if( neutralShowers[i]->dDetectorSystem == SYS_FCAL ){
      
      const DFCALShower* fcalShower = dynamic_cast< const DFCALShower* >( neutralShowers[i]->dBCALFCALShower );	
      showerQualityMap[fcalShower] = neutralShowers[i]->dQuality;
     }
  }
  
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

    vector< std::shared_ptr< const DFCALShowerMatchParams > > matchedShowers;
    map<const DFCALShower*, int> showerMatchIndex;
    map<const DFCALShower*, const DKinematicData*> showerMatchTrack;
    list<const DFCALShower*> pi0Showers;

    vector< const JObject* > neutralSource = 
      combo->Get_FinalParticle_SourceObjects( d_Neutral );

    for( size_t i = 0; i < neutralSource.size(); ++i ){
      
      const DNeutralShower* neutShower = dynamic_cast< const DNeutralShower* >( neutralSource[i] );
      if( neutShower == NULL ) continue;  // shouldn't happen if sources are neutral
      
      if( neutShower->dDetectorSystem == SYS_FCAL ){

	pi0Showers.push_back( dynamic_cast< const DFCALShower* >( neutShower->dBCALFCALShower ) );
      }
    }
    
    // in this section we are looking to see if the existing track-shower matching
    // algorithm geometrically matched a track to the shower
    vector<const DKinematicData*> tracks =
      combo->Get_FinalParticles( reaction, false, false, d_Charged );
    for( size_t iTrk = 0; iTrk < tracks.size(); ++iTrk ){

      const DChargedTrackHypothesis* ctHypo =
	dynamic_cast<const DChargedTrackHypothesis*>( tracks[iTrk] );
      std::shared_ptr< const DFCALShowerMatchParams > match =
	ctHypo->Get_FCALShowerMatchParams();

      if( match == NULL ) continue;
      
      const DFCALShower* showerPtr = match->dFCALShower;
      showerMatchTrack[showerPtr] = tracks[iTrk];
      showerMatchIndex[showerPtr] = matchedShowers.size();
      matchedShowers.push_back( match );	
    }

    // below here are mainly things that we expect to be able to do be done
    // for every event, independent of topology, at the time of
    // creation of DFCALShower objects
    
    vector< const DTrackWireBased* > allWBTracks;
    loop->Get( allWBTracks );
    vector< const DTrackWireBased* > wbTracks = filterWireBasedTracks( allWBTracks );
    
    vector< const DFCALShower* > showerVector;
    loop->Get( showerVector );

    // entering the tree-filling, thread-unsafe, region...
    
    japp->RootFillLock(this);

    m_event = eventnumber;
    m_t0RF = eventRFBunches[0]->dTime;

    DVector3 targetCenter( 0, 0, m_zTarget );

    // a track -> shower vector that gets used later:
    DVector3 u( 0, 0, 0 );
    
    for( size_t iShower = 0; iShower < showerVector.size(); ++iShower ){
      
      const DFCALShower* shower = showerVector[iShower];

      vector< const DFCALCluster* > clusterVec;
      shower->Get( clusterVec );
      const DFCALCluster* cluster = clusterVec[0];
      
      // this has z set such that the origin is at the center of the target:
      DVector3 clusPos = cluster->getCentroid();
      
      DVector3 fcalPos = shower->getPosition();
      DVector3 norm( 0, 0, 1 );

      m_eSh = shower->getEnergy();
      m_tSh = shower->getTime();
      m_xSh = shower->getPosition().X();
      m_ySh = shower->getPosition().Y();
      m_zSh = shower->getPosition().Z();
      m_depthSh = shower->getPosition().Z() - m_FCALUpstream;

      m_disSh = ( shower->getPosition() - targetCenter ).Mag();
      m_dtSh = shower->getTime() - m_t0RF;
      m_speedSh = m_disSh/m_dtSh;

      m_qualSh = showerQualityMap[shower];
        
      // set nonsense values in case there isn't a "nearest track"
      m_docaTr = 1E6;
      m_tTr = 1E6;
      m_pTr = 0;
      m_qTr = 0;
      m_xTr = 1E6;
      m_yTr = 1E6;

      //      double pathLength, flightTimeVariance;
      double flightTime;
      DVector3 projPos, projMom;

      // find the closest track to the shower -- here we loop over the best FOM
      // wire-based track for every track candidate not just the ones associated
      // with the topology
      for( size_t iTrk = 0; iTrk < wbTracks.size(); ++iTrk ){

	// swim the track to vertical plane the intercepts the shower position

	/*
	if( wbTracks[iTrk]->rt->
	    GetIntersectionWithPlane( fcalPos, norm, projPos, projMom, &pathLength,
				      &flightTime, &flightTimeVariance, SYS_FCAL ) !=
	    NOERROR ) continue;
	*/

	if( !wbTracks[iTrk]->GetProjection( SYS_FCAL, projPos, &projMom, &flightTime ) ) continue;
	
	// need to swim fcalPos to common z for DOCA calculation -- this really
	// shouldn't be in the loop if the z-value of projPos doesn't change
	// with each track
	
	DVector3 fcalFacePos = ( shower->getPosition() - targetCenter );
	fcalFacePos.SetMag( fcalFacePos.Mag() * projPos.Z() / fcalFacePos.Z() );
 
	double distance = ( fcalFacePos - projPos ).Mag();
	
	if( distance < m_docaTr ){

	  m_docaTr = distance;
	  m_tTr = m_t0RF + ( wbTracks[iTrk]->position().Z() - targetCenter.Z() ) / SPEED_OF_LIGHT +
	    flightTime;
	  m_xTr = projPos.X();
	  m_yTr = projPos.Y();
	  m_pTr = wbTracks[iTrk]->pmag();
	  m_qTr = wbTracks[iTrk]->charge();
	  m_dtTrSh = m_tSh - m_tTr;
	}
      }

      // see if there was geometric match to a track or it was a photon shower:
      // (this is a level of sorting that will not exist -- used for algorithm training)
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
  
      // this is lower-level cluster information that is sometimes useful
      // for calibration of shower creation
      m_disCl = cluster->getCentroid().Mag();
      m_eCl = cluster->getEnergy();
      m_tCl = cluster->getTime();
      m_eWtTCl = cluster->getTimeEWeight();
      m_eWtTRMSCl =cluster->getRMS_t();


      // now start working at the hit level
      vector< const DFCALHit* > hits = getHitsFromShower( shower );
      
      // the following four variables will be get set in the subsequent
      // loop over hits
      m_nHits = 0;
      m_eMaxSh = 0;
      m_xMaxSh = 0;
      m_yMaxSh = 0;
      fillHits( hits );

      fillUVFromHits( hits, DVector3( m_xSh, m_ySh, 0 ), DVector3( m_xTr, m_yTr, 0 ) );

      // this needs to run after the max locations have been set
      fillE1925FromHits( hits, m_xMaxSh, m_yMaxSh );

      m_docaTr2 = shower->getDocaTrack();
      m_dtTrSh2 = m_tSh - m_t0RF - shower->getTimeTrack();
      m_sumUSh2 = shower->getSumU();
      m_sumVSh2 = shower->getSumV();
      m_e1e9Sh2 = shower->getE1E9();
      m_e9e25Sh2 = shower->getE9E25();
      
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


vector< const DFCALHit* >
JEventProcessor_ShowerShapeTree::getHitsFromShower( const DFCALShower* shower ) const {

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

vector< const DTrackWireBased* >
JEventProcessor_ShowerShapeTree::filterWireBasedTracks( vector< const DTrackWireBased* >& wbTracks ) const {

  vector< const DTrackWireBased* > finalTracks;
  map< unsigned int, vector< const DTrackWireBased* > > sortedTracks;

  // first sort the wire based tracks into lists with a common candidate id
  // this means that they all come from the same track in the detector
  
  for( unsigned int i = 0; i < wbTracks.size(); ++i ){

    unsigned int id = wbTracks[i]->candidateid;

    if( sortedTracks.find( id ) == sortedTracks.end() ){
      
      sortedTracks[id] = vector< const DTrackWireBased* >();
    }

    sortedTracks[id].push_back( wbTracks[i] );
  }

  // now loop through that list of unique tracks and for each set
  // of wire based tracks, choose the one with the highest FOM
  // (this is choosing among different particle hypotheses)
  
  for( map< unsigned int, vector< const DTrackWireBased* > >::const_iterator
	 anId = sortedTracks.begin();
       anId != sortedTracks.end(); ++anId ){

    double maxFOM = 0;
    unsigned int bestIndex = 0;

    for( unsigned int i = 0; i < anId->second.size(); ++i ){

      if( anId->second[i]->Ndof < 15 ) continue;
      
      if( anId->second[i]->FOM > maxFOM ){

	maxFOM = anId->second[i]->FOM;
	bestIndex = i;
      }
    }

    finalTracks.push_back( anId->second[bestIndex] );
  }
  
  return finalTracks;
}


void
JEventProcessor_ShowerShapeTree::fillHits( const vector< const DFCALHit* >& hitVec ){
  
  // need to be sure to lock the event thread somewhere before
  // entering this function
  
  if( hitVec.size() + m_nHits > kMaxHits ) return;
  
  for( vector< const DFCALHit* >::const_iterator hit = hitVec.begin();
      hit != hitVec.end(); ++hit ){

    m_idHit[m_nHits] = m_fcalGeom.channel( (**hit).row, (**hit).column ); 
    m_xHit[m_nHits] = (**hit).x;
    m_yHit[m_nHits] = (**hit).y;
    m_eHit[m_nHits] = (**hit).E;
    m_tHit[m_nHits] = (**hit).t;

    if( (**hit).E > m_eMaxSh ){

      m_eMaxSh = (**hit).E;
      m_yMaxSh = (**hit).y;
      m_xMaxSh = (**hit).x;
    }
    
    ++m_nHits;
  }
}

void
JEventProcessor_ShowerShapeTree::fillUVFromHits( const vector< const DFCALHit* >& hits,
						 const DVector3& showerVec,
						 const DVector3& trackVec ){

  DVector3 u = ( showerVec - trackVec ).Unit();
  DVector3 z( 0, 0, 1 );
  DVector3 v = u.Cross( z );

  DVector3 hitLoc( 0, 0, 0 );

  m_sumUSh = 0;
  m_sumVSh = 0;

  double sumE = 0;
  
  for( vector< const DFCALHit* >::const_iterator hit = hits.begin();
	 hit != hits.end(); ++hit ){

    hitLoc.SetX( (**hit).x - showerVec.X() );
    hitLoc.SetY( (**hit).y - showerVec.Y() );

    m_sumUSh += (**hit).E * pow( u.Dot( hitLoc ), 2 );
    m_sumVSh += (**hit).E * pow( v.Dot( hitLoc ), 2 );

    sumE += (**hit).E;
  }

  m_sumUSh /= sumE;
  m_sumVSh /= sumE;
  m_asymUVSh = ( m_sumUSh - m_sumVSh ) / ( m_sumUSh + m_sumVSh );
}

void
JEventProcessor_ShowerShapeTree::fillE1925FromHits( const vector< const DFCALHit* >& hits,
						    float xMax, float yMax ){

  float E9 = 0;
  float E25 = 0;

  for( vector< const DFCALHit* >::const_iterator hit = hits.begin();
	 hit != hits.end(); ++hit ){
     
     if( fabs( (**hit).x - xMax ) < 4.5 && fabs( (**hit).y - yMax ) < 4.5 )
       E9 += (**hit).E;

     if( fabs( (**hit).x - xMax ) < 8.5 && fabs( (**hit).y - yMax ) < 8.5 )
       E25 += (**hit).E;
   }

   m_e1e9Sh = m_eMaxSh/E9;
   m_e9e25Sh = E9/E25;
}
