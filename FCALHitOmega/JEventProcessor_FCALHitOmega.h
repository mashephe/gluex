// $Id$
//
//    File: JEventProcessor_FCALHitOmega.h
// Created: Wed Jul 13 09:42:29 EDT 2022
// Creator: mashephe (on Linux stanley-1.physics.indiana.edu 3.10.0-1160.71.1.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_FCALHitOmega_
#define _JEventProcessor_FCALHitOmega_

#include <JANA/JEventProcessor.h>

#include "ANALYSIS/DTreeInterface.h"

class DFCALHit;
class DFCALShower;
class TTree;

class JEventProcessor_FCALHitOmega : public jana::JEventProcessor {

 public:

  JEventProcessor_FCALHitOmega();
  ~JEventProcessor_FCALHitOmega();
  const char* className(void){return "JEventProcessor_FCALHitOmega";}

 private:
  
  jerror_t init(void);						///< Called once at program start.
  jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
  jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
  jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
  jerror_t fini(void);						///< Called after last event of last event source has been processed.

  vector< const DFCALHit* > getHitsFromShower( const DFCALShower* shower ) const;

  enum{ kMaxHitsSh = 100   };
  enum{ kMaxHitsEvt = 2800 }; 
  
  double m_zTarget;
  double m_FCALUpstream;

  TTree* m_treeEvt;
  TTree* m_treeSh;
  
  float m_eSh;
  float m_xSh;
  float m_ySh;
  float m_dtSh;
  int m_nHitSh;
  float m_xHitSh[kMaxHitsSh];
  float m_yHitSh[kMaxHitsSh];
  float m_eHitSh[kMaxHitsSh];
  float m_dtHitSh[kMaxHitsSh];

  float m_omegaMassEvt;
  int m_nHitEvt;
  int m_nShEvt;
  float m_xHitEvt[kMaxHitsEvt];
  float m_yHitEvt[kMaxHitsEvt];
  float m_eHitEvt[kMaxHitsEvt];
  float m_dtHitEvt[kMaxHitsEvt];  

};

#endif // _JEventProcessor_FCALHitOmega_

