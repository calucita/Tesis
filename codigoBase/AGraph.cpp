
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <utility>
#include <set>
#include <ext/hash_map>
#include "global.h"
#include "Agent.h"
#include "World.h"
#include "AGraph.h"

using namespace std;


AGraph::AGraph(int pSize, RepType pRepType){
  size = pSize;
  repType = pRepType;
  float p = 0.3;
  float r;
  
  // creo la lista de adyacencias para cada nodo
  for (int i=0; i<pSize; i++){
    g[i].clear(); 
    nMap[i] = NULL;
  }

  cout << "Tamanho del mapa de nodos: " << nMap.size() << "\n";
  

  // crear grafo vacio modelo Erdos-Renyi (Random Graph) con
  // probabilidad p  
  for (int i=0; i<pSize; i++){
    for (int j=0; j<pSize; j++){
      r = (float) rand()/RAND_MAX;
      if (r<=p && i!=j){
        this->addArc(i,j);
      }
    }
  }

}


void AGraph::populate(){}

void AGraph::repopulate(int nAgents, int nCoops, int nNoCoops){}

void AGraph::print(){}
  
list<Agent*> AGraph::getNeighbourgs(int pos){}

int AGraph::getNumAgents(){}

int AGraph::getNumCooperators(){}

int AGraph::countType(list<Agent*> aList, AType pType){}

void AGraph::removeAgentAt(int){}

void AGraph::step(){}

void AGraph::reapAgents(){}

AGraph::~AGraph(){}




void AGraph::addArc(int i, int j){

  bool found = false;
  
  cout << "addArc ("<< i <<"," <<j<<")\n";

  vector<int>& outN = g.find(i)->second;
  if (!outN.empty()){
    for (vector<int>::iterator e=outN.begin(); e!=outN.end(); e++){
      if (*e == j) {
        found = true;
      }
    }
  }
  if (!found) {
    g[i].push_back(j);
    g[j].push_back(i);
  }
}
