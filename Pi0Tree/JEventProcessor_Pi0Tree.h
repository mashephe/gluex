// $Id$
//
//    File: JEventProcessor_Pi0Tree.h
// Created: Tue Nov 25 06:10:11 EST 2014
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-358.6.2.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_Pi0Tree_
#define _JEventProcessor_Pi0Tree_

#include <JANA/JEventProcessor.h>

#include <TTree.h>

class JEventProcessor_Pi0Tree:public jana::JEventProcessor{
 public:
  JEventProcessor_Pi0Tree();
  ~JEventProcessor_Pi0Tree();
  const char* className(void){return "JEventProcessor_Pi0Tree";}

  enum { kMaxHitsCluster = 50 };

 private:
  jerror_t init(void);						///< Called once at program start.
  jerror_t brun(jana::JEventLoop *eventLoop, int runnumber);	///< Called everytime a new run number is detected.
  jerror_t evnt(jana::JEventLoop *eventLoop, int eventnumber);	///< Called every event.
  jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
  jerror_t fini(void);						///< Called after last event of last event source has been processed.

  TTree* m_tree;

  float m_eL;
  float m_eH;
  float m_tL;
  float m_tH;
  float m_tewL;
  float m_tewH;
  float m_trmsL;
  float m_trmsH;
  float m_rL;
  float m_rH;
  float m_dr;
  float m_dt;
  float m_dtew;
  float m_mass;
  float m_massTot;

  int m_nHitL;
  int m_nHitH;
  float m_eHitL[kMaxHitsCluster];
  float m_eHitH[kMaxHitsCluster];
  float m_qHitL[kMaxHitsCluster];
  float m_qHitH[kMaxHitsCluster];

  float m_eTot;
  float m_t0;
  int m_nClus;
  int m_pi0Ind;

  int m_run;
  int m_event;
};

#endif // _JEventProcessor_Pi0Tree_

