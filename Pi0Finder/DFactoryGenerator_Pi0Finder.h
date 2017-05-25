// $Id$
//
//    File: DFactoryGenerator_Pi0Finder.h
// Created: Fri Jul 11 14:18:49 EDT 2014
// Creator: mashephe (on Darwin 149-160-160-51.dhcp-bl.indiana.edu 13.3.0 i386)
//

#ifndef _DFactoryGenerator_Pi0Finder_
#define _DFactoryGenerator_Pi0Finder_

#include <JANA/jerror.h>
#include <JANA/JFactoryGenerator.h>

#include "DReaction_factory_Pi0Inclusive.h"
#include "DReaction_factory_Pi0Omega.h"

class DFactoryGenerator_Pi0Finder : public jana::JFactoryGenerator
{
 public:
  virtual const char* className(void){return static_className();}
  static const char* static_className(void){return "DFactoryGenerator_Pi0Finder";}
		
  jerror_t GenerateFactories(jana::JEventLoop* locEventLoop)
  {
    locEventLoop->AddFactory(new DReaction_factory_Pi0Inclusive());
    locEventLoop->AddFactory(new DReaction_factory_Pi0Omega());
    return NOERROR;
  }
};

#endif // _DFactoryGenerator_Pi0Finder_

