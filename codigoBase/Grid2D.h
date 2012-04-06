
#ifndef __GRID2D__
#define __GRID2D__

#include <iostream>
#include <string>
#include <list>
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


class Grid2D: public World {

public:
  
  Grid2D(int pSize, RepType pRepType=UNIFORM);

  //  Grid2D(int pSize, RepType pRepType);
  
  void populate();
  void repopulate(int nAgents, int nCoops, int nNoCoops);
  void print();
  
  list<Agent*> getNeighbourgs(Position *pos);
  int getNumAgents();
  int getNumCooperators();
  int countType(list<Agent*> aList, AType pType);
  void removeAgentAt(Position *pos);
  void step();
  void reapAgents();
  ~Grid2D();
  
private:

  Agent ***nodos;

  set<Agent*> agents;

  list<Agent*> died;
  
};

#endif
