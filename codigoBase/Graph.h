
#ifndef __GRAPH__
#define __GRAPH__

#include <iostream>
#include <string>
//#include <list>
#include <vector>
#include <utility>
#include <set>
#include <ext/hash_map>
#include "global.h"
#include "Agent.h"
#include "World.h"


using namespace std;

class World;
class Agent;



// class Position1D:public Position{
  
//  public:
  
//   Position1D(int p){x=p;};
//   Position1D(const Position1D& p1d){x = p1d.x;};
//   int getPos(){return x;};
//   void setPos(int p){x = p;};
//   friend ostream& operator<<(ostream& out, const Position1D& pos);

//  private:
//   int x;

// };


class Grid1D: public World {

public:
  
  Grid1D(int pSize, RepType pRepType=UNIFORM);
  
  void populate() ;
  void repopulate(int nAgents, int nCoops, int nNoCoops);
  void print();
  
  list<Agent*> getNeighbourgs(int pos);
  int getNumAgents();
  int getNumCooperators();
  int countType(list<Agent*> aList, AType pType);
  void removeAgentAt(int);
  void step();
  void reapAgents();
  ~Grid1D();
  
private:
  Agent **nodos;

  set<Agent> agents;

  list<Agent*> died;
  
  //Agent **nodos;
  //int size;
  //RepType repType;
};

#endif
