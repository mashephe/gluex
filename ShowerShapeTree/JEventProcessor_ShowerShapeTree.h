// $Id$
//
//    File: JEventProcessor_ShowerShapeTree.h
// Created: Mon Jun  5 14:03:02 EDT 2017
// Creator: mashephe (on Darwin 149-161-212-208.dhcp-bl.indiana.edu 16.6.0 i386)
//

#ifndef _JEventProcessor_ShowerShapeTree_
#define _JEventProcessor_ShowerShapeTree_

#include <JANA/JEventProcessor.h>
#include "FCAL/DFCALGeometry.h"

#include "DVector3.h"
#include <vector>

using namespace std;

class DFCALHit;
class DFCALShower;
class DTrackWireBased;
class TTree;

class JEventProcessor_ShowerShapeTree:public jana::JEventProcessor{
public:
  JEventProcessor_ShowerShapeTree();
  ~JEventProcessor_ShowerShapeTree();
  const char* className(void){return "JEventProcessor_ShowerShapeTree";}
  
 private:
  
  enum{ kMaxHits = 100 };
  
  jerror_t init(void);						///< Called once at program start.
  jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
  jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
  jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
  jerror_t fini(void);						///< Called after last event of last event source has been processed.

  vector< const DFCALHit* > getHitsFromShower( const DFCALShower* shower ) const;
  vector< const DTrackWireBased* > filterWireBasedTracks( vector< const DTrackWireBased* >& wbTracks ) const;

  void fillHits( const vector< const DFCALHit* >& hitVec );
  void fillUVFromHits( const vector< const DFCALHit* >& hits,
		       const DVector3& showerVec, const DVector3& trackVec );
  void fillE1925FromHits( const vector< const DFCALHit* >& hits, float xMax, float yMax );
  
  TTree* m_tree;

  uint64_t m_event;

  DFCALGeometry m_fcalGeom;
  
  int m_typeSh;

  double m_zTarget;
  double m_FCALUpstream;

  float m_t0RF;
  
  float m_xSh;
  float m_ySh;
  float m_zSh;
  float m_eSh;
  float m_tSh;
  float m_disSh;
  float m_dtSh;
  float m_dtTrSh;
  float m_speedSh;
  float m_depthSh;
  float m_e1e9Sh;
  float m_e9e25Sh;
  float m_sumUSh;
  float m_sumVSh;
  float m_asymUVSh;

  float m_eMaxSh;
  float m_xMaxSh;
  float m_yMaxSh;
  
  float m_eCl;
  float m_tCl;
  float m_eWtTCl;
  float m_eWtTRMSCl;
  float m_disCl;
  
  float m_qTr;
  float m_docaTr;
  float m_tTr;
  float m_pTr;
  float m_xTr;
  float m_yTr;
  float m_t0Tr;

  int m_nHits;
  int m_idHit[kMaxHits];
  float m_xHit[kMaxHits];
  float m_yHit[kMaxHits];
  float m_eHit[kMaxHits];
  float m_tHit[kMaxHits];
  
};

#endif // _JEventProcessor_ShowerShapeTree_

