// $Id$
//
//    File: DFSConfig.h
// Created: Mon Jul 21 14:42:17 EDT 2014
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-358.6.2.el6.x86_64 x86_64)
//

#ifndef _DFSConfig_
#define _DFSConfig_

class DFSConfig
{

 public:
 
  enum RecoilFlag { kMissingN, kFoundN };
 
 DFSConfig( string name, map< Particle_t, int > mesonSys, 
	    Particle_t recoil, RecoilFlag recFlag = kMissingN,
	    bool wROOT = false ) :
    m_name( name ),
    m_mesonSys( mesonSys ),
    m_recoil( recoil ),
    m_recFlag( recFlag ),
    m_writeROOT( wROOT ) {}
  
  string name() const { return m_name; }
  Particle_t recoil() const { return m_recoil; }
  bool recMiss() const { return ( kMissingN == m_recFlag ); }

  const map< Particle_t, int >& mesonSys() const { return m_mesonSys; }

  bool writeROOT() const { return m_writeROOT; }

 private:

  string m_name;
  map< Particle_t, int > m_mesonSys;
  Particle_t m_recoil;
  RecoilFlag m_recFlag;
  bool m_writeROOT;
};

#endif // _DFSConfig_
