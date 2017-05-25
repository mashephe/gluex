// $Id$
//
//    File: JEventProcessor_Pi0Tree.cc
// Created: Tue Nov 25 06:10:11 EST 2014
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-358.6.2.el6.x86_64 x86_64)
//

#include "JEventProcessor_Pi0Tree.h"
using namespace jana;

#include "FCAL/DFCALCluster.h"
#include "DLorentzVector.h"
#include "DVector3.h"
#include "DANA/DApplication.h"
#include "units.h"

#include <TTree.h>

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
extern "C"{
  void InitPlugin(JApplication *app){
    InitJANAPlugin(app);
    app->AddProcessor(new JEventProcessor_Pi0Tree());
  }
} // "C"


//------------------
// JEventProcessor_Pi0Tree (Constructor)
//------------------
JEventProcessor_Pi0Tree::JEventProcessor_Pi0Tree()
{

}

//------------------
// ~JEventProcessor_Pi0Tree (Destructor)
//------------------
JEventProcessor_Pi0Tree::~JEventProcessor_Pi0Tree()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_Pi0Tree::init(void)
{
  // This is called once at program startup. If you are creating
  // and filling historgrams in this plugin, you should lock the
  // ROOT mutex like this:
  //
  japp->RootWriteLock();

  m_tree = new TTree( "pi0Tree", "Pi0 Reconstruction" );
  
  m_tree->Branch( "eL", &m_eL, "eL/F" );
  m_tree->Branch( "eH", &m_eH, "eH/F" );
  m_tree->Branch( "tL", &m_tL, "tL/F" );
  m_tree->Branch( "tH", &m_tH, "tH/F" );
  m_tree->Branch( "tewL", &m_tewL, "tewL/F" );
  m_tree->Branch( "tewH", &m_tewH, "tewH/F" );
  m_tree->Branch( "trmsL", &m_trmsL, "trmsL/F" );
  m_tree->Branch( "trmsH", &m_trmsH, "trmsH/F" );
  m_tree->Branch( "rL", &m_rL, "rL/F" );
  m_tree->Branch( "rH", &m_rH, "rH/F" );
  m_tree->Branch( "dr", &m_dr, "dr/F" );
  m_tree->Branch( "dt", &m_dt, "dt/F" );
  m_tree->Branch( "dtew", &m_dtew, "dtew/F" );
  m_tree->Branch( "mass", &m_mass, "mass/F" );

  m_tree->Branch( "massTot", &m_massTot, "massTot/F" );
  m_tree->Branch( "eTot", &m_eTot, "eTot/F" );
  m_tree->Branch( "t0", &m_t0, "t0/F" );

  m_tree->Branch( "nHitL", &m_nHitL, "nHitL/I" );
  m_tree->Branch( "nHitH", &m_nHitH, "nHitH/I" );

  m_tree->Branch( "eHitL", m_eHitL, "eHitL[nHitL]/F" );
  m_tree->Branch( "eHitH", m_eHitH, "eHitL[nHitH]/F" );
  m_tree->Branch( "qHitL", m_qHitL, "qHitL[nHitL]/F" );
  m_tree->Branch( "qHitH", m_qHitH, "qHitH[nHitH]/F" );

  m_tree->Branch( "run", &m_run, "run/I" );
  m_tree->Branch( "event", &m_event, "event/I" );
  m_tree->Branch( "nClus", &m_nClus, "nClus/I" );
  m_tree->Branch( "nPi0Ind", &m_pi0Ind, "nPi0Ind/I" );
  
  japp->RootUnLock();
 
  return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_Pi0Tree::brun(JEventLoop *eventLoop, int runnumber)
{
  // This is called whenever the run number changes

  return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_Pi0Tree::evnt(JEventLoop *loop, int eventnumber)
{

  vector< const DFCALCluster* > clusterVec;

  vector< const DFCALCluster* > clusterFactoryVec;
  loop->Get( clusterFactoryVec );

  double t0 = 0;
  double e = 0;

  for( vector< const DFCALCluster*>::const_iterator clusItr = clusterFactoryVec.begin();
       clusItr != clusterFactoryVec.end(); ++clusItr ){
    
    e += (**clusItr).getEnergy();
    t0 += (**clusItr).getEnergy() * (**clusItr).getTime();
  }

  t0 /= e;

  for( vector< const DFCALCluster*>::const_iterator clusItr = clusterFactoryVec.begin();
       clusItr != clusterFactoryVec.end(); ++clusItr ){
    
    // remove this cut for now to allow all clusters to enter
    // the analysis window
    
    //    if( fabs( (**clusItr).getTime() - t0 ) < 6*k_nsec ){

      clusterVec.push_back( *clusItr );
      //    }
  }
  
  if( clusterVec.size() < 2 ) return NOERROR;
  if( clusterVec.size() > 8 ) return NOERROR;

  /*
  if( ( clusterVec.size() < 3 ) ||
      ( clusterVec.size() > 4 ) ) return NOERROR;
  */

  japp->RootWriteLock();

  m_event = eventnumber;
  m_t0 = t0;
  m_nClus = clusterVec.size();  

  m_eTot = 0;
  DLorentzVector totGam( 0, 0, 0, 0 );

  for( vector< const DFCALCluster*>::const_iterator clusItr = clusterVec.begin();
       clusItr != clusterVec.end(); ++clusItr ){

    m_eTot += (**clusItr).getEnergy();

    DVector3 gamMom = (**clusItr).getCentroid();
    gamMom.SetMag( (**clusItr).getEnergy() );
    totGam += DLorentzVector( gamMom, gamMom.Mag() );
  }

  m_massTot = totGam.M();

  //  if( m_massTot < 0.5 ) return NOERROR;

  m_pi0Ind = 0;
  for( vector< const DFCALCluster*>::const_iterator clus1Itr = clusterVec.begin();
       clus1Itr != clusterVec.end(); ++clus1Itr ){
    for( vector< const DFCALCluster*>::const_iterator clus2Itr = clus1Itr + 1;
	 clus2Itr != clusterVec.end(); ++clus2Itr ){

      const DFCALCluster& clusL = 
	( (**clus1Itr).getEnergy() > (**clus2Itr).getEnergy() ? 
	  (**clus2Itr) : (**clus1Itr) );

      const DFCALCluster& clusH = 
	( (**clus1Itr).getEnergy() > (**clus2Itr).getEnergy() ? 
	  (**clus1Itr) : (**clus2Itr) );

      // fill low energy cluster data

      double clusLX = clusL.getCentroid().X();
      double clusLY = clusL.getCentroid().Y();
      m_rL = sqrt( clusLX * clusLX + clusLY * clusLY );
      m_eL = clusL.getEnergy();
      m_tL = clusL.getTime();
      m_tewL = clusL.getTimeEWeight();
      m_trmsL = clusL.getRMS_t();

      const vector<DFCALCluster::DFCALClusterHit_t>& hitsL = clusL.GetHits();
      m_nHitL = hitsL.size();
      for( int ihit = 0; ihit < m_nHitL; ++ihit ){
	
	m_eHitL[ihit] = hitsL[ihit].E;
	m_qHitL[ihit] = hitsL[ihit].intOverPeak;
      }

      // fill high energy cluster data

      double clusHX = clusH.getCentroid().X();
      double clusHY = clusH.getCentroid().Y();
      m_rH = sqrt( clusHX * clusHX + clusHY * clusHY );
      m_eH = clusH.getEnergy();
      m_tH = clusH.getTime();
      m_tewH = clusH.getTimeEWeight();
      m_trmsH = clusH.getRMS_t();

      const vector<DFCALCluster::DFCALClusterHit_t>& hitsH = clusH.GetHits();
      m_nHitH = hitsH.size();
      for( int ihit = 0; ihit < m_nHitH; ++ihit ){
	
	m_eHitH[ihit] = hitsH[ihit].E;
	m_qHitH[ihit] = hitsH[ihit].intOverPeak;
      }     

      // now use clusters together

      DVector3 clusLMom = clusL.getCentroid(); 
      clusLMom.SetMag( clusL.getEnergy() );

      DVector3 clusHMom = clusH.getCentroid(); 
      clusHMom.SetMag( clusH.getEnergy() );
    
      m_dr = sqrt( ( clusHX - clusLX ) * ( clusHX - clusLX ) +
		   ( clusHY - clusLY ) * ( clusHY - clusLY ) );

      m_dt = fabs( m_tL - m_tH );
      m_dtew = fabs( m_tewL - m_tewH );

      DLorentzVector gamL( clusLMom, clusLMom.Mag() );
      DLorentzVector gamH( clusHMom, clusHMom.Mag() );

      m_mass = ( gamL + gamH ).M();

      m_tree->Fill();

      m_pi0Ind++;
    }
  }

  japp->RootUnLock();

  return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_Pi0Tree::erun(void)
{
  // This is called whenever the run number changes, before it is
  // changed to give you a chance to clean up before processing
  // events from the next run number.
  return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_Pi0Tree::fini(void)
{
  // Called before program exit after event processing is finished.
  return NOERROR;
}

