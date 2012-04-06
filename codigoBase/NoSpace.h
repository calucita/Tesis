
#ifndef __NOSPACE__
#define __NOSPACE__

#include <iostream>
#include <string>
#include <list>
#include "global.h"
#include "Agent.h"
#include "World.h"



using namespace std;

class World;
class Agent;



class NoSpace: public World {

public:
  
  NoSpace(int pSize, RepType pRepType=UNIFORM);
  
  void populate() ;
  void repopulate(int nAgents, int nCoops, int nNoCoops);
  void print();
  
  list<Agent*> getNeighbourgs(Position *pos);
  int getNumAgents();
  int getNumCooperators();
  int countType(list<Agent*> aList, AType pType);
  void removeAgentAt(Position *pos);
  void step();
  void reapAgents();
  ~NoSpace();
  
private:
  Agent **nodos;

  set<Agent*> agents;

  list<Agent*> died;
  
};

#endif
