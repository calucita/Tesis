
#ifndef __WORLD__
#define __WORLD__

#include <iostream>
#include <string>
#include <list>
#include "global.h"
#include "Position.h"

class Agent;

using namespace std;


class World{
  
public:
  
  static float InitialCoopProp;
  static float InitialNoCoopProp;
  
  //World(int pSize);
  virtual void populate()=0;
  virtual void repopulate(int,int,int)=0;
  virtual void print()=0;
  virtual list<Agent*> getNeighbourgs(Position *pos) = 0;
  //virtual list<Agent*> getNeighbourgs(int x, int y) const;
  virtual int getNumAgents()=0;
  virtual int getNumCooperators()=0;
  virtual int countType(list<Agent*> aList, AType pType)=0;
  virtual void removeAgentAt(Position *pos) = 0;
  //virtual void removeAgentAt(int x, int y) const;
  virtual void step()=0;
  virtual void reapAgents()=0;

  virtual ~World() {};

  virtual void getRealConnectivities(ofstream& fp) {};
  
protected:
  int size;
  RepType repType;
  

  
};


#endif
