// $Id$
//
//    File: DFactoryGenerator_FSFilter.h
// Created: Mon Jul 21 14:42:17 EDT 2014
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-358.6.2.el6.x86_64 x86_64)
//

#ifndef _DFactoryGenerator_FSFilter_
#define _DFactoryGenerator_FSFilter_

#include <JANA/jerror.h>
#include <JANA/JFactoryGenerator.h>

#include "DReaction_factory_FSFilter.h"

class DFactoryGenerator_FSFilter : public jana::JFactoryGenerator
{
 public:
  virtual const char* className(void){return static_className();}
  static const char* static_className(void){return "DFactoryGenerator_FSFilter";}
		
  jerror_t GenerateFactories(jana::JEventLoop* locEventLoop)
  {
    locEventLoop->AddFactory(new DReaction_factory_FSFilter());
    return NOERROR;
  }
};

#endif // _DFactoryGenerator_FSFilter_

