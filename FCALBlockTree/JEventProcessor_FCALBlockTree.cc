// $Id$
//
//    File: JEventProcessor_FCALBlockTree.cc
// Created: Wed Mar 22 16:38:50 EDT 2017
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-642.6.2.el6.x86_64 x86_64)
//

#include "JEventProcessor_FCALBlockTree.h"
using namespace jana;

#include "FCAL/DFCALShower.h"
#include "FCAL/DFCALCluster.h"
#include "FCAL/DFCALHit.h"
#include "PID/DVertex.h"

#include "DLorentzVector.h"
#include "TTree.h"
#include "TH1F.h"

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
extern "C"{
  void InitPlugin(JApplication *app){
    InitJANAPlugin(app);
    app->AddProcessor(new JEventProcessor_FCALBlockTree());
  }
} // "C"


//------------------
// JEventProcessor_FCALBlockTree (Constructor)
//------------------
JEventProcessor_FCALBlockTree::JEventProcessor_FCALBlockTree()
{

  USE_PI0_BLOCKS = 1;
  gPARMS->SetDefaultParameter( "USE_PI0_BLOCKS", USE_PI0_BLOCKS );
  
}

//------------------
// ~JEventProcessor_FCALBlockTree (Destructor)
//------------------
JEventProcessor_FCALBlockTree::~JEventProcessor_FCALBlockTree()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_FCALBlockTree::init(void)
{

  japp->RootWriteLock();

  m_tree = new TTree( "fcalBlockHits", "FCAL Block Hits" );

  m_tree->Branch( "nHits", &m_nHits, "nHits/I" );
  m_tree->Branch( "x", m_x, "x[nHits]/F" );
  m_tree->Branch( "y", m_y, "y[nHits]/F" );
  m_tree->Branch( "E", m_E, "E[nHits]/F" );
  m_tree->Branch( "t", m_t, "t[nHits]/F" );
  m_tree->Branch( "iop", m_iop, "iop[nHits]/F" );

  m_massHist = new TH1F( "massHist", "2#gamma Invariant Mass", 100, 0.085, 0.185 );
  
  japp->RootUnLock();
  
  return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_FCALBlockTree::brun(JEventLoop *eventLoop, int32_t runnumber)
{
  // This is called whenever the run number changes
  return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_FCALBlockTree::evnt(JEventLoop *loop, uint64_t eventnumber)
{

  if( ! USE_PI0_BLOCKS ){

    // raw hit dump
    vector< const DFCALHit* > hits;
    loop->Get( hits );

    japp->RootFillLock(this);

    m_nHits = 0;
    fillHitBlocks( hits );

    japp->RootFillUnLock(this);
  }
  else{
  

    // pi0 mode

    vector< const DVertex* > vertexVec;
    loop->Get( vertexVec );
    DVector3 vertex = vertexVec[0]->dSpacetimeVertex.Vect();

    vector< const DFCALShower* > fcalShowers;
    loop->Get( fcalShowers );

    if( fcalShowers.size() < 2 ) return NOERROR;

    japp->RootFillLock(this);

    m_nHits = 0;
  
    for( unsigned int i = 0; i < fcalShowers.size(); ++i ){

      const DFCALShower* shower1 = fcalShowers[i];
      DVector3 pVec1 = shower1->getPosition() - vertex;
      Double_t E1 = shower1->getEnergy();
      TLorentzVector p4_1( E1*pVec1.Unit(), E1 );
    
      for( unsigned int j = i+i; j < fcalShowers.size(); ++j ){
  
	const DFCALShower* shower2 = fcalShowers[j];
	DVector3 pVec2 = shower2->getPosition() - vertex;
	Double_t E2 = shower2->getEnergy();
	TLorentzVector p4_2( E2*pVec2.Unit(), E2 );

	float twoGamMass = ( p4_1 + p4_2 ).Mag();
	m_massHist->Fill( twoGamMass );
      
	if( abs( twoGamMass - 0.135 ) < 0.05 ){

	  fillHitsFromShower( shower1 );
	  fillHitsFromShower( shower2 );
	}
      }
    }
    japp->RootFillUnLock(this);
  }


  
  if( m_nHits > 0 ){

    japp->RootFillLock(this);
    m_tree->Fill();
    japp->RootFillUnLock(this);
  }
  
  return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_FCALBlockTree::erun(void)
{
  // This is called whenever the run number changes, before it is
  // changed to give you a chance to clean up before processing
  // events from the next run number.
  return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_FCALBlockTree::fini(void)
{
  // Called before program exit after event processing is finished.

  japp->RootWriteLock();
  m_tree->Write();
  japp->RootUnLock();

  return NOERROR;
}

void
JEventProcessor_FCALBlockTree::fillHitBlocks( const vector< const DFCALHit* >& hitVec ){

  // need to be sure to lock the even thread somewhere before
  // entering this function
  
  if( hitVec.size() + m_nHits > kMaxHits ) return;
    
  for( vector< const DFCALHit* >::const_iterator hit = hitVec.begin();
       hit != hitVec.end();
       ++hit ){

    m_x[m_nHits] = (**hit).x;
    m_y[m_nHits] = (**hit).y;
    m_E[m_nHits] = (**hit).E;
    m_t[m_nHits] = (**hit).t;
    m_iop[m_nHits] = (**hit).intOverPeak;

    ++m_nHits;
  }
}

void
JEventProcessor_FCALBlockTree::fillHitsFromShower( const DFCALShower* shower ){

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
