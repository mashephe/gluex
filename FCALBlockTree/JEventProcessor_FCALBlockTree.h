// $Id$
//
//    File: JEventProcessor_FCALBlockTree.h
// Created: Wed Mar 22 16:38:50 EDT 2017
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-642.6.2.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_FCALBlockTree_
#define _JEventProcessor_FCALBlockTree_

#include <JANA/JEventProcessor.h>

#include <vector>

using namespace std;

class DFCALHit;
class DFCALShower;
class TTree;
class TH1F;

class JEventProcessor_FCALBlockTree:public jana::JEventProcessor{
 public:
  JEventProcessor_FCALBlockTree();
  ~JEventProcessor_FCALBlockTree();
  const char* className(void){return "JEventProcessor_FCALBlockTree";}

  enum { kMaxHits = 2800 };
  
 private:
  jerror_t init(void);						///< Called once at program start.
  jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
  jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
  jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
  jerror_t fini(void);						///< Called after last event of last event source has been processed.

  void fillHitBlocks( const vector< const DFCALHit* >& hitVec );
  void fillHitsFromShower( const DFCALShower* shower );

  int USE_PI0_BLOCKS;
  
  TTree* m_tree;
  TH1F* m_massHist;

  int m_nHits;
  float m_x[kMaxHits];
  float m_y[kMaxHits];
  float m_E[kMaxHits];
  float m_t[kMaxHits];
  float m_iop[kMaxHits];

};

#endif // _JEventProcessor_FCALBlockTree_

