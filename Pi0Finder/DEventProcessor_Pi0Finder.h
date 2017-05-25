// $Id$
//
//    File: DEventProcessor_Pi0Finder.h
// Created: Fri Jul 11 14:18:49 EDT 2014
// Creator: mashephe (on Darwin 149-160-160-51.dhcp-bl.indiana.edu 13.3.0 i386)
//

#ifndef _DEventProcessor_Pi0Finder_
#define _DEventProcessor_Pi0Finder_

#include <JANA/JEventProcessor.h>
#include <JANA/JApplication.h>

#include <ANALYSIS/DEventWriterROOT.h>
#include <HDDM/DEventWriterREST.h>
#include <ANALYSIS/DHistogramActions.h>

#include "DFactoryGenerator_Pi0Finder.h"

using namespace jana;
using namespace std;

class DEventProcessor_Pi0Finder : public jana::JEventProcessor
{
	public:
		const char* className(void){return "DEventProcessor_Pi0Finder";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop* locEventLoop, int locRunNumber);	///< Called every time a new run number is detected.
		jerror_t evnt(jana::JEventLoop* locEventLoop, int locEventNumber);	///< Called every event.
		jerror_t erun(void);						///< Called every time run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		//For non-custom reaction-independent histograms, it is recommended that you simultaneously run the monitoring_hists plugin instead of defining them here

		const DEventWriterROOT* dEventWriterROOT;
		const DEventWriterREST* dEventWriterREST;
};

#endif // _DEventProcessor_Pi0Finder_

