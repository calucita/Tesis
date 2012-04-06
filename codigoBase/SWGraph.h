
#ifndef __SW_GRAPH__
#define __SW_GRAPH__

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



class SWGraph: public World {

  
public:

  typedef hash_map<int, vector<int> > Adyacencies;
  typedef map<int, Agent*> NodeMap;
  
  
  SWGraph(int n, int k=2, double p=0.01, RepType pRepType=UNIFORM);
  
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
  ~SWGraph();

 // Auxiliares
  void getRealConnectivities(ofstream& fp);
  
  
private:
  

  //  Agent **nodos;
  NodeMap nMap;
  Adyacencies g;

  set<Agent*> agents;
  list<Agent*> died;

  void addArc(int i, int j);

};

#endif
