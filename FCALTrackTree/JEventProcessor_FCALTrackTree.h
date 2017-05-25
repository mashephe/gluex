// $Id$
//
//    File: JEventProcessor_FCALTrackTree.h
// Created: Fri Jan 30 08:18:41 EST 2015
// Creator: shepherd (on Linux ifarm1102 2.6.32-220.7.1.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_FCALTrackTree_
#define _JEventProcessor_FCALTrackTree_

#include <JANA/JEventProcessor.h>
#include <FCAL/DFCALGeometry.h>

class DFCALHit;
class TTree;

class JEventProcessor_FCALTrackTree:public jana::JEventProcessor{
 public:

  // pad the array sizes by two units around the perimiter of
  // the detector so we can look up 5x5 arrays centered on
  // every block

  enum { kPad = 2 };
  enum { kRowSize = ( DFCALGeometry::kBlocksWide + 2*kPad ) };
  enum { kArrSize = kRowSize * kRowSize };
  enum { kMaxClus = 10 };

  JEventProcessor_FCALTrackTree();
  ~JEventProcessor_FCALTrackTree();
  const char* className(void){return "JEventProcessor_FCALTrackTree";}

 private:

  jerror_t init(void);						///< Called once at program start.
  jerror_t brun(jana::JEventLoop *eventLoop, int runnumber);	///< Called everytime a new run number is detected.
  jerror_t evnt(jana::JEventLoop *eventLoop, int eventnumber);	///< Called every event.
  jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
  jerror_t fini(void);						///< Called after last event of last event source has been processed.

  void fillHits( const vector< const DFCALHit* >& hitVec );

  const DFCALGeometry* m_fcalGeom;

  TTree* m_tree;

  double m_FCALfront;

  float m_p;
  float m_x;
  float m_y;
  int m_r;
  int m_c;

  float m_e[25];
  float m_q[25];
  float m_t[25];

  float m_e9;
  float m_e25;

  float m_eHit[kArrSize];
  float m_tHit[kArrSize];
  float m_qHit[kArrSize];

  int m_nCl;
  float m_eCl[kMaxClus];
  float m_tCl[kMaxClus];
  float m_dCl[kMaxClus];
  float m_xCl[kMaxClus];
  float m_yCl[kMaxClus];

};

#endif // _JEventProcessor_FCALTrackTree_

