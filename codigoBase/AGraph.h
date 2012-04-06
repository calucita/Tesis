
#ifndef __AGRAPH__
#define __AGRAPH__

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <utility>
#include <set>
#include <map>
#include <ext/hash_map>
#include "global.h"
#include "Agent.h"
#include "World.h"

using namespace __gnu_cxx;
using namespace std;

class World;
class Agent;



class AGraph: public World {

public:

  typedef hash_map<int, vector<int> > Adyacencies;
  typedef map<int, Agent*> NodeMap;  

  AGraph(int pSize, RepType pRepType=UNIFORM);
  
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
  ~AGraph();
  
private:
  //Agent **nodos;
  //set<Agent> agents;
  
  list<Agent*> died;
  NodeMap nMap;
  Adyacencies g;

  void addArc(int i, int j);

 
};

#endif
