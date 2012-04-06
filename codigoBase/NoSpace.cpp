
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <iterator>
#include <stdlib.h>
#include "global.h"
#include "World.h"
#include "Agent.h"
#include "NoSpace.h"

//extern struct globalArgs_t coopGlobalArgs;

using namespace std;


NoSpace::NoSpace(int pSize, RepType pRepType){
  size = pSize;
  repType = pRepType;
  nodos = new Agent*[size]; 
  for (int i=0; i<size; i++){
    nodos[i] = NULL; 
  }
  cout << "NoSpace::repType= " << pRepType << endl;

  //showOptions2();
  //set<Agent> agents;
  //list<Agent*> died;
}



void NoSpace::populate(){

  float r;
  Agent *a = NULL;
  Position *pos = NULL;

  for (int i=0; i<size; ++i){
    r = (float)rand()/RAND_MAX;
    // if (r <=  coopGlobalArgs.propCoIni +  coopGlobalArgs.propNCoIni){ 
    cout << "populate: i=" << i << " r= " << r << "\n";
    a = new Agent(r<= coopGlobalArgs.propCoIni ? COOP : NOCOOP, this);
    pos = new Position;
    pos->i = i;
    a->setPos(pos);
    this->nodos[i] = a;
    agents.insert(a);
    //}
  }
  
}



void NoSpace::repopulate(int nAgents, int nCoops, int nNoCoops){

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



void NoSpace::step(){
  
  Agent* a = NULL;
  set<Agent*>::iterator i;
  
  for (i = agents.begin(); i != agents.end(); ++i) {  
    a = *i;
    //cout << *a;
    a->step();
  }
  //this->print();
}


void NoSpace::reapAgents(){

  //cout << "NoSpace::Reaping\n";
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

void NoSpace::print(){
  cout << "\nNoSpace::print" << endl;
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


  
list<Agent*> NoSpace::getNeighbourgs(Position *p){

  list<Agent*> ns;
  int pos = p->i;

  int dado = -1;
 
  for (int i=0; i<4; i++){    
    dado = (int)(size-1)*((float)rand()/RAND_MAX);

    if ((dado != pos) && (nodos[dado]!=NULL)) { // TO DO: revisar que tampoco este en la lista de vecinos
      ns.push_back(nodos[dado]);
    } else {
      --i;
    } 
  }
  
  // Codigo para verificar funcionamiento
  cout << "vecinos de " << pos << ":";
  list<Agent*>::iterator iter;
  for (iter = ns.begin(); iter != ns.end(); ++iter) {  
    if ((*iter) != NULL) {
      cout << (**iter).getId() << " ";
    }
  }  
  cout << "\n";
  cout << "----------------------\n";

  return ns;
}


int NoSpace::getNumAgents(){
  return agents.size();
}

int NoSpace::getNumCooperators(){

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


int NoSpace::countType(list<Agent*> aList, AType pType){
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


void NoSpace::removeAgentAt(Position *p){
  
  int pos = p->i;

  //cout <<"removeAgentAt: "<< pos << endl;
  died.push_back(nodos[pos]);
  //agents.erase(nodos[pos]);
  nodos[pos] = NULL;

}


NoSpace::~NoSpace(){
  delete[] nodos; 
}
