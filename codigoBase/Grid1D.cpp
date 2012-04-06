
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <iterator>
#include <stdlib.h>
#include "global.h"
#include "World.h"
#include "Agent.h"
#include "Grid1D.h"

//extern struct globalArgs_t coopGlobalArgs;

using namespace std;

//float World::InitialCoopProp =  0.2;
//float World::InitialNoCoopProp = 0.2;

void showOptions2(){

  cout << "Show Options en Grid 1D\n";

  cout << "world: " << coopGlobalArgs.world << endl; 
  cout << "size: " << coopGlobalArgs.size << endl; 
  cout << "iters: "<< coopGlobalArgs.iters << endl;
  cout << "propCoIni: " << coopGlobalArgs.propCoIni<< endl;
  cout << "propNCoIni: " << coopGlobalArgs.propNCoIni<< endl;
  cout << "predProb0n: " << coopGlobalArgs.predProb0n << endl;
  cout << "predProb1n: " << coopGlobalArgs.predProb1n << endl;
  cout << "predProb2n: " << coopGlobalArgs.predProb2n << endl;
  cout << "predProbNCo: " << coopGlobalArgs.predProbNCo << endl;
  cout << "CT:" << coopGlobalArgs.CT << endl;
  cout << "probCT: "<< coopGlobalArgs.probCT << endl;
  cout << "coopMortalityRate: " << coopGlobalArgs.coopMortalityRate << endl;
  cout << "noCoopMortalityRate: " <<coopGlobalArgs.noCoopMortalityRate << endl;
  cout << "repType: " <<coopGlobalArgs.repType << endl;
  cout << "migration: " << coopGlobalArgs.migration << endl;
  cout << "randomize: "<< coopGlobalArgs.randomize << endl;
  cout << "file: " <<coopGlobalArgs.outFileName << endl;
  cout << endl;
}


Grid1D::Grid1D(int pSize, RepType pRepType){
  size = pSize;
  repType = pRepType;
  nodos = new Agent*[size]; 
  for (int i=0; i<size; i++){
    nodos[i] = NULL; 
  }
  cout << "Grid1D::repType= " << pRepType << endl;

  //showOptions2();
  //set<Agent> agents;
  //list<Agent*> died;
}



void Grid1D::populate(){
  //cout<<"populate"<<"\n";
  float r;
  Agent *a = NULL;
  Position *pos = NULL;

  for (int i=0; i<size; ++i){
    r = (float)rand()/RAND_MAX;
    if (r <=  coopGlobalArgs.propCoIni +  coopGlobalArgs.propNCoIni){ 
      a = new Agent(r<= coopGlobalArgs.propCoIni ? COOP : NOCOOP, this);
      pos = new Position;
      pos->i = i;
      a->setPos(pos);
      this->nodos[i] = a;
      agents.insert(a);
    }
  }
  
  cout << "Grid1D::populate: " << agents.size() << endl;

}



void Grid1D::repopulate(int nAgents, int nCoops, int nNoCoops){

  const int MaxNeighbourgs = 2;
  float pCoopRep = 0.0;
  float pNoCoopRep = 0.0;
  float r;
  Agent *a = NULL;
  Position *pos = NULL;

  Position paux;

  //cout<<"repopulate::repType:" << repType << "\n";
  
  if (repType == UNIFORM) {
    pCoopRep =  coopGlobalArgs.propCoIni;
    pNoCoopRep =  coopGlobalArgs.propNCoIni;
  } 
  else if (repType == PROPORTIONAL){
    if (nAgents > 0){
      pCoopRep =  (float)nCoops/nAgents;
      pNoCoopRep = (float)nNoCoops/nAgents;
    }
  }

  //cout << "propCoop: " << pCoopRep << "\n";
  //cout << "propNoCoop: " << pNoCoopRep << "\n\n";
  
  list<Agent*> ln;
  int nc = 0;
  int nnc = 0;
  int nvecs = 0;

  for (int i=0; i<size; i++){
    
    if (this->nodos[i] == NULL){
      //cout << "i: " << i << " vacio \n";
      r = (float)rand()/RAND_MAX;
      
      if (repType == PROP_LOCAL){
        cout << "No deberia entrar aqui\n";
        ln.clear();
        pCoopRep = 0.0f;
        pNoCoopRep = 0.0f;

        paux.i = i;
        ln = getNeighbourgs(&paux);
        nvecs = ln.size();
        //cout << "vecinos:" << nvecs <<"\n";
        nc = this->countType(ln,COOP);
        nnc = nvecs - nc;

        if (nvecs>0) {
           pCoopRep =  (float)nc/MaxNeighbourgs;
           pNoCoopRep = (float)nnc/MaxNeighbourgs;
           //cout << "propCoop: " << pCoopRep << "\n";
           //cout << "propNoCoop: " << pNoCoopRep << "\n";

           if (r <= pCoopRep + pNoCoopRep){
             a = new Agent(r<= pCoopRep ? COOP : NOCOOP, this);
             pos = new Position;
             pos->i = i;
             a->setPos(pos);
             this->nodos[i] = a; 
             agents.insert(a);
            
           }
        }
      } 
      else { // UNIFORM or PROPORTIONAL
        if (r <= pCoopRep +  pNoCoopRep) {
          a = new Agent(r<= pCoopRep ? COOP : NOCOOP, this);
          pos = new Position;
          pos->i = i;
          a->setPos(pos);
          this->nodos[i] = a;
          agents.insert(a);

        }
      }
    }
  }
}



void Grid1D::step(){
  
  Agent* a = NULL;
  set<Agent*>::iterator i;
  
  for (i = agents.begin(); i != agents.end(); ++i) {  
    a = *i;
    //cout << *a;
    a->step();
  }
  //this->print();
}


void Grid1D::reapAgents(){

  //cout << "Grid1D::Reaping\n";
  list<Agent*>::iterator iter;
  Agent* pa;
  for (iter = died.begin(); iter != died.end(); ++iter) {  
    pa = *iter;
    // sacarlo de la lista de agentes
    agents.erase(pa);
    delete(pa);
  }
  died.clear();
}

void Grid1D::print(){
  cout << "\nGrid1D::print" << endl;
  for (int i=0; i<size; i++){
    if (nodos[i]) {
      cout << *nodos[i] << "|";
    } else {
      cout <<  "< >|";
    }
  }
  cout << "\n";
  cout << "num Agentes: " << agents.size() << endl;
  cout << "Lista Agentes " << endl;
  //copy(agents.begin(), agents.end(), ostream_iterator<Agent*>(cout," "));
  set<Agent*>::iterator iter;
  for (iter = agents.begin(); iter != agents.end(); ++iter) {  
    cout << **iter << " ";
  }  
  cout << "\n";
  cout << "Muertos:";
  list<Agent*>::iterator iter2;
  for (iter2 = died.begin(); iter2 != died.end(); ++iter2) {  
    cout << **iter2 << " ";
  }
  cout << endl << endl;

}


  
list<Agent*> Grid1D::getNeighbourgs(Position *p){

  list<Agent*> ns;
  int pos = p->i;

  if (pos == 0){
    if (nodos[size-1])
      ns.push_back(nodos[size-1]);
    if (nodos[pos+1])
      ns.push_back(nodos[pos+1]);
  
   } else if (pos == size-1){
    if (nodos[pos-1])
      ns.push_back(nodos[pos-1]);
    if (nodos[0])
      ns.push_back(nodos[0]);
  } else {
    if (nodos[pos-1])
      ns.push_back(nodos[pos-1]);
    if (nodos[pos+1])
      ns.push_back(nodos[pos+1]);
  }

  return ns;
}


int Grid1D::getNumAgents(){
  return agents.size();
}

int Grid1D::getNumCooperators(){

  list<Agent*> ags;
  int count = 0;
  Agent* a = NULL;

  set<Agent*>::iterator i;
  for (i = agents.begin(); i != agents.end(); ++i) {  
    a = *i;
    if ((a->getType()) == COOP){
      count++;
    }
  }
  return count;
}


int Grid1D::countType(list<Agent*> aList, AType pType){

  int cont = 0;
  list<Agent *>::iterator i;

  for (i = aList.begin(); i != aList.end(); ++i) {  
    if (*i != NULL){
      if ((*i)->getType() == pType){
        cont++;
      }
    }
  }
  return cont;
}


void Grid1D::removeAgentAt(Position *p){
  
  int pos = p->i;

  //cout <<"removeAgentAt: "<< pos << endl;

  // lo agrego a la lista de muertos
  died.push_back(nodos[pos]);

  // lo elimino del conjunto de agentes
  // Hay que hacerlo bien!
  //agents.erase(nodos[pos]);

  // Lo elimino saco del mundo
  nodos[pos] = NULL;




}


Grid1D::~Grid1D(){
  delete[] nodos; 
}
