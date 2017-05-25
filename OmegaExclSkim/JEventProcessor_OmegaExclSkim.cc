// $Id$
//
//    File: JEventProcessor_OmegaExclSkim.cc
// Created: Wed May 24 13:46:12 EDT 2017
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-642.6.2.el6.x86_64 unknown)
//

#include "JEventProcessor_OmegaExclSkim.h"
#include "DFactoryGenerator_OmegaExclSkim.h"

using namespace jana;

#include "evio_writer/DEventWriterEVIO.h"

#include <ANALYSIS/DAnalysisResults.h>

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
extern "C"{
  void InitPlugin(JApplication *app){
    InitJANAPlugin(app);
    app->AddProcessor(new JEventProcessor_OmegaExclSkim());
    app->AddFactoryGenerator( new DFactoryGenerator_OmegaExclSkim() );
  }
} // "C"


//------------------
// JEventProcessor_OmegaExclSkim (Constructor)
//------------------
JEventProcessor_OmegaExclSkim::JEventProcessor_OmegaExclSkim()
{

}

//------------------
// ~JEventProcessor_OmegaExclSkim (Destructor)
//------------------
JEventProcessor_OmegaExclSkim::~JEventProcessor_OmegaExclSkim()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_OmegaExclSkim::init(void)
{
  // This is called once at program startup. 

  return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_OmegaExclSkim::brun(JEventLoop *eventLoop, int32_t runnumber)
{
  // This is called whenever the run number changes
  return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_OmegaExclSkim::evnt(JEventLoop *loop, uint64_t eventnumber)
{
  const DEventWriterEVIO* eventWriterEVIO = NULL;
  loop->GetSingle(eventWriterEVIO);

  // write out BOR events
  if(loop->GetJEvent().GetStatusBit(kSTATUS_BOR_EVENT)) {

    eventWriterEVIO->Write_EVIOEvent(loop, "omega");
    return NOERROR;
  }

  vector<const DAnalysisResults*> analysisResultsVector;
  loop->Get( analysisResultsVector );

  if( analysisResultsVector[0]->Get_NumPassedParticleCombos() != 0 ){

    eventWriterEVIO->Write_EVIOEvent(loop, "omega");
  }
  
  return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_OmegaExclSkim::erun(void)
{
  // This is called whenever the run number changes, before it is
  // changed to give you a chance to clean up before processing
  // events from the next run number.
  return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_OmegaExclSkim::fini(void)
{
  // Called before program exit after event processing is finished.
  return NOERROR;
}

