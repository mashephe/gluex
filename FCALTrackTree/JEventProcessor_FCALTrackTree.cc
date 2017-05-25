// $Id$
//
//    File: JEventProcessor_FCALTrackTree.cc
// Created: Fri Jan 30 08:18:41 EST 2015
// Creator: shepherd (on Linux ifarm1102 2.6.32-220.7.1.el6.x86_64 x86_64)
//

#include "JEventProcessor_FCALTrackTree.h"

#include <JANA/JApplication.h>

#include "FCAL/DFCALGeometry.h"
#include "FCAL/DFCALHit.h"
#include "FCAL/DFCALCluster.h"
#include "TRACKING/DTrackTimeBased.h"

#include "DVector3.h"

#include <TTree.h>
#include "DVector3.h"

using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
extern "C"{
  void InitPlugin(JApplication *app){
    InitJANAPlugin(app);
    app->AddProcessor(new JEventProcessor_FCALTrackTree());
  }
} // "C"


//------------------
// JEventProcessor_FCALTrackTree (Constructor)
//------------------
JEventProcessor_FCALTrackTree::JEventProcessor_FCALTrackTree()
{

}

//------------------
// ~JEventProcessor_FCALTrackTree (Destructor)
//------------------
JEventProcessor_FCALTrackTree::~JEventProcessor_FCALTrackTree()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_FCALTrackTree::init(void)
{
  // This is called once at program startup. If you are creating
  // and filling historgrams in this plugin, you should lock the
  // ROOT mutex like this:
  //
  japp->RootWriteLock();

  m_tree = new TTree( "fcalTrackTree", "Tracks Match to FCAL" );

  m_tree->Branch( "p", &m_p, "p/F" );
  m_tree->Branch( "x", &m_x, "p/F" );
  m_tree->Branch( "y", &m_y, "p/F" );
  m_tree->Branch( "r", &m_r, "p/I" );
  m_tree->Branch( "c", &m_c, "p/I" );

  m_tree->Branch( "e", m_e, "e[25]/F" );
  m_tree->Branch( "q", m_q, "q[25]/F" );
  m_tree->Branch( "t", m_t, "t[25]/F" );

  m_tree->Branch( "e9", &m_e9, "e9/F" );
  m_tree->Branch( "e25", &m_e25, "e25/F" );

  m_tree->Branch( "nCl", &m_nCl, "nCl/I" );
  m_tree->Branch( "eCl", m_eCl, "eCl[nCl]/F" );
  m_tree->Branch( "tCl", m_tCl, "tCl[nCl]/F" );
  m_tree->Branch( "dCl", m_dCl, "dCl[nCl]/F" );
  m_tree->Branch( "xCl", m_xCl, "xCl[nCl]/F" );
  m_tree->Branch( "yCl", m_yCl, "yCl[nCl]/F" );

  japp->RootUnLock();

  return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_FCALTrackTree::brun(JEventLoop *eventLoop, 
					     int runnumber)
{

  // get the FCAL z position from the global geometry interface
  DApplication *dapp = 
    dynamic_cast<DApplication*>(eventLoop->GetJApplication());
  const DGeometry *geom = dapp->GetDGeometry(runnumber);
  if( geom ) {

    geom->GetFCALZ( m_FCALfront );
  }
  else{
      
    cerr << "No geometry accessbile." << endl;
    return RESOURCE_UNAVAILABLE;
  }


  // we need an FCAL Geometry object
  vector< const DFCALGeometry* > geomVec;
  eventLoop->Get( geomVec );

  if( geomVec.size() != 1 ){

    cerr << "No geometry accessbile." << endl;
    return RESOURCE_UNAVAILABLE;
  }

  m_fcalGeom = geomVec[0];

  return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_FCALTrackTree::evnt(JEventLoop *eventLoop, 
					     int eventnumber)
{
  
  vector<const DTrackTimeBased*> trkVec;
  eventLoop->Get( trkVec );

  vector<const DFCALHit*> fcalHitVec;
  eventLoop->Get( fcalHitVec );

  vector<const DFCALCluster*> clusterVec;
  eventLoop->Get( clusterVec );

  // lock now to manipulate member data
  japp->RootWriteLock();

  memset( m_eHit, 0, kArrSize * sizeof( float ) );
  memset( m_tHit, 0, kArrSize * sizeof( float ) );
  memset( m_qHit, 0, kArrSize * sizeof( float ) );

  fillHits( fcalHitVec );

  DVector3 fcalFace( 0, 0, m_FCALfront );

  for( vector< const DTrackTimeBased* >::const_iterator trkItr = trkVec.begin();
       trkItr != trkVec.end();
       ++trkItr ){

    // the vector fcalFace works to both define a point in the plane
    // of the front face of the FCAL and as a vector normal to the plane
    
    if( (**trkItr).pmag() < 1 ) continue;

    DVector3 trkPos,trkMom;
    if( (**trkItr).rt->GetIntersectionWithPlane( fcalFace, fcalFace, 
						 trkPos, trkMom ) == NOERROR ){

      // we have a track swum to the face of the FCAL
      // now fill information for this track
      
      // record the momentum of the track and (x,y) position
      // on the FCAL face
      m_p = trkMom.Mag();
      m_x = trkPos.X();
      m_y = trkPos.Y();
      
      // now get row, column corresponding to the intersection point
      m_r = m_fcalGeom->row( m_y );
      m_c = m_fcalGeom->column( m_x );

      // if this doesn't correspond to an active block in the
      // detector then move onto the next block
      if( !m_fcalGeom->isBlockActive( m_r, m_c ) ) continue;

      // copy information about the 5x5 array around this impact point
      // do this in rows:  2 before to 2 after the center row
      // copy 5 columns in each row at a time
      int rDest = 0;
      for( int rSrc = m_r - 2 + kPad; rSrc <= m_r + 2 + kPad; ++rSrc ){

	memcpy( &(m_e[rDest*5]), &(m_eHit[rSrc*kRowSize+m_c+kPad-2]), 
		5 * sizeof( float ) );
	memcpy( &(m_t[rDest*5]), &(m_tHit[rSrc*kRowSize+m_c+kPad-2]), 
		5 * sizeof( float ) );
	memcpy( &(m_q[rDest*5]), &(m_qHit[rSrc*kRowSize+m_c+kPad-2]), 
		5 * sizeof( float ) );
	++rDest;
      }

      // now compute some energy sums that may be useful
     
      m_e25 = 0;
      m_e9  = 0;

      for( int i = 0; i < 25; ++i ){

	if( ( i/5 > 0 ) && ( i/5 < 4 ) &&
	    ( i%5 > 0 ) && ( i%5 < 4 ) ){

	  m_e9 += m_e[i];
	}

	m_e25 += m_e[i];
      }

      // now loop over clusters and compute how many exist
      // with a centroid within 8 cm of the track impact
      
      m_nCl = 0;
      for( vector< const DFCALCluster* >::const_iterator 
	     clusItr = clusterVec.begin();
	   clusItr != clusterVec.end();
	   ++clusItr ){

	const DFCALCluster& clus = (**clusItr);

	// track to cluster distance
	float d =  sqrt( ( clus.getCentroid().X() - m_x ) *
			 ( clus.getCentroid().X() - m_x ) +
			 ( clus.getCentroid().Y() - m_y ) *
			 ( clus.getCentroid().Y() - m_y ) );

	if( d < 8 ){
	  
	  m_yCl[m_nCl] = clus.getCentroid().Y();
	  m_xCl[m_nCl] = clus.getCentroid().X();
	  m_eCl[m_nCl] = clus.getEnergy();
	  m_tCl[m_nCl] = clus.getTime();
	  m_dCl[m_nCl] = d;

	  ++m_nCl;
	}
      }

      m_tree->Fill();
    }
  }

  japp->RootUnLock();

  return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_FCALTrackTree::erun(void)
{
  // This is called whenever the run number changes, before it is
  // changed to give you a chance to clean up before processing
  // events from the next run number.
  return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_FCALTrackTree::fini(void)
{
  // Called before program exit after event processing is finished.
  return NOERROR;
}

void 
JEventProcessor_FCALTrackTree::fillHits( const vector< const DFCALHit* >& hitVec ){

  for( vector< const DFCALHit* >::const_iterator hitItr = hitVec.begin();
       hitItr != hitVec.end();
       ++hitItr ){

    const DFCALHit& hit = (**hitItr);

    int r = hit.row + kPad;
    int c = hit.column + kPad;

    m_eHit[r*kRowSize+c] = hit.E;
    m_tHit[r*kRowSize+c] = hit.t;
    m_qHit[r*kRowSize+c] = hit.intOverPeak;
  }
}
