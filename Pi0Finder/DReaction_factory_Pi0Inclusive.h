// $Id$
//
//    File: DReaction_factory_Pi0Inclusive.h
// Created: Fri Jul 11 14:18:49 EDT 2014
// Creator: mashephe (on Darwin 149-160-160-51.dhcp-bl.indiana.edu 13.3.0 i386)
//

#ifndef _DReaction_factory_Pi0Inclusive_
#define _DReaction_factory_Pi0Inclusive_

#include <iostream>
#include <iomanip>

#include <JANA/JFactory.h>
#include <ANALYSIS/DReaction.h>
#include <ANALYSIS/DHistogramActions.h>
#include <ANALYSIS/DCutActions.h>

using namespace std;
using namespace jana;

class DReaction_factory_Pi0Inclusive : public jana::JFactory<DReaction>
{
	public:
		DReaction_factory_Pi0Inclusive()
		{
			// This is so that the created DReaction objects persist throughout the life of the program instead of being cleared each event. 
			SetFactoryFlag(PERSISTANT);
		}
		const char* Tag(void){return "Pi0Inclusive";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		deque<DReactionStep*> dReactionStepPool; //to prevent memory leaks
};

#endif // _DReaction_factory_Pi0Inclusive_

