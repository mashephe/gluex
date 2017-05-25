// $Id$
//
//    File: DEventProcessor_FSFilter.h
// Created: Mon Jul 21 14:42:17 EDT 2014
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-358.6.2.el6.x86_64 x86_64)
//

#ifndef _DEventProcessor_FSFilter_
#define _DEventProcessor_FSFilter_

#include <JANA/JEventProcessor.h>
#include <JANA/JApplication.h>

#include <ANALYSIS/DEventWriterROOT.h>
#include <HDDM/DEventWriterREST.h>
#include <ANALYSIS/DHistogramActions.h>

#include "DFactoryGenerator_FSFilter.h"

using namespace jana;
using namespace std;

class DEventProcessor_FSFilter : public jana::JEventProcessor
{
	public:
		const char* className(void){return "DEventProcessor_FSFilter";}

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

#endif // _DEventProcessor_FSFilter_

