
#ifndef __GRID1D__
#define __GRID1D__

#include <iostream>
#include <string>
#include <list>
#include "global.h"
#include "Agent.h"
#include "World.h"



using namespace std;

class World;
class Agent;



class Grid1D: public World {

public:
  
  Grid1D(int pSize, RepType pRepType=UNIFORM);
  
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
  ~Grid1D();
  
private:
  Agent **nodos;

  set<Agent*> agents;

  list<Agent*> died;
  
  //Agent **nodos;
  //int size;
  //RepType repType;
};

#endif
