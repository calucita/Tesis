
#ifndef __BA_GRAPH__
#define __BA_GRAPH__

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



class BAGraph: public World {

  
public:

  typedef hash_map<int, vector<int> > Adyacencies;
  typedef map<int, Agent*> NodeMap;
  
  
  BAGraph(int n, int m=3, RepType pRepType=UNIFORM);
  
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
  ~BAGraph();

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
