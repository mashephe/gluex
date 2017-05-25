// $Id$
//
//    File: DCustomAction_Preselect.h
// Created: Mon Jul 21 14:42:17 EDT 2014
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-358.6.2.el6.x86_64 x86_64)
//

#ifndef _DCustomAction_Preselect_
#define _DCustomAction_Preselect_

#include <string>
#include <iostream>

#include "JANA/JEventLoop.h"
#include "JANA/JApplication.h"

#include "ANALYSIS/DAnalysisAction.h"
#include "ANALYSIS/DReaction.h"
#include "ANALYSIS/DParticleCombo.h"
#include "ANALYSIS/DAnalysisUtilities.h"

using namespace std;
using namespace jana;

class DCustomAction_Preselect : public DAnalysisAction
{
 public:

 DCustomAction_Preselect(const DReaction* locReaction, bool locUseKinFitResultsFlag, 
			     string locActionUniqueString = "") : 
  DAnalysisAction(locReaction, "Preselect", locUseKinFitResultsFlag, locActionUniqueString) {}

  void Initialize(JEventLoop* locEventLoop);

 private:

  DLorentzVector totalStepP4( const DParticleCombo* combo, const DParticleComboStep* step,
			      bool useKinFit, bool skipFirst = false ) const;

  bool Perform_Action(JEventLoop* locEventLoop, const DParticleCombo* locParticleCombo);

  // Optional: Useful utility functions.
  const DAnalysisUtilities* dAnalysisUtilities;

  //Store any histograms as member variables here
  TH1D* ptHist;
  TH1D* pBalHist;
};

#endif // _DCustomAction_Preselect_

