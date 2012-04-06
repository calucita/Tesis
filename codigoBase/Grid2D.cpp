
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <iterator>
#include <math.h>
#include <stdlib.h>
#include "global.h"
#include "World.h"
#include "Agent.h"
#include "Grid2D.h"


using namespace std;


Grid2D::Grid2D(int pSize, RepType pRepType){
  
  size = (int)sqrt(pSize);
  repType = pRepType;
  cout << "numFilas: " << size; 

  nodos = new Agent**[size]; 
  for (int i=0; i<size; i++){
    nodos[i] = new Agent*[size];
  }
  for (int i=0; i<size; i++){  
    for (int j=0; j<size; j++){
      nodos[i][j] = NULL;
    }
  }
}



void Grid2D::populate(){
  //cout<<"Grid2D::populate"<<"\n";
  float r;
  Agent *a = NULL;
  Position *pos = NULL;
  Pair* par1 = NULL;

  for (int i=0; i<size; ++i){
    for (int j=0; j<size; ++j){
      r = (float)rand()/RAND_MAX;
      if (r <=  coopGlobalArgs.propCoIni +  coopGlobalArgs.propNCoIni){ 
        a = new Agent(r<= coopGlobalArgs.propCoIni ? COOP : NOCOOP, this);
        par1 = new Pair;
        par1->x = i; par1->y = j;
        pos = new Position;
        pos->par = *par1;        
        a->setPos(pos);
        this->nodos[i][j] = a;
        agents.insert(a);
      }
    }
  }
  
  cout << "Grid2D::populate: " << agents.size() << endl;
  
}



void Grid2D::repopulate(int nAgents, int nCoops, int nNoCoops){

  const int MaxNeighbourgs = 4;
  float pCoopRep = 0.0;
  float pNoCoopRep = 0.0;
  float r;
  Agent *a = NULL;
  Position *pos = NULL;
  Pair* par1 = NULL;
  Position paux;
  //cout<<"repopulate" << "\n";
  
  if (repType == UNIFORM) {
    pCoopRep =  coopGlobalArgs.propCoIni;
    pNoCoopRep =  coopGlobalArgs.propNCoIni;
  } 
  else if (repType == PROPORTIONAL){
    pCoopRep =  (float)nCoops/nAgents;
    pNoCoopRep = (float)nNoCoops/nAgents;
  }
  
  list<Agent*> ln;
  int nc = 0;
  int nnc = 0;
  int nvecs = 0;
  //Position p1;

  for (int i=0; i<size; i++){
    for (int j=0; j<size; j++){
      if (this->nodos[i][j] == NULL){
        r = (float)rand()/RAND_MAX;
        if (repType == PROP_LOCAL){
          //cout << "Repopulate: Proporcional local\n";
          ln.clear();
          pCoopRep = 0.0f;
          pNoCoopRep = 0.0f;
          
          paux.par.x = i; 
          paux.par.y = j;
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
              par1 = new Pair;
              par1->x = i; par1->y = j;
              pos = new Position;
              pos->par = *par1;  
              a->setPos(pos);
              this->nodos[i][j] = a; 
              agents.insert(a);
            }
          }
        } 
        else { // UNIFORM or PROPORTIONAL
          if (r <= pCoopRep +  pNoCoopRep) {
            a = new Agent(r<= pCoopRep ? COOP : NOCOOP, this);
            par1 = new Pair;
            par1->x = i; par1->y = j;
            pos = new Position;
            pos->par = *par1;  
            a->setPos(pos);
            this->nodos[i][j] = a;
            agents.insert(a);
            
          }
        }
      }
    }
  }
}


void Grid2D::step(){
  
  Agent* a = NULL;
  set<Agent*>::iterator i;
  
  for (i = agents.begin(); i != agents.end(); ++i) {  
    a = *i;
    a->step();
  }
}


void Grid2D::reapAgents(){

  //cout << "reaping\n";
  list<Agent*>::iterator iter;
  Agent* pa;
  for (iter = died.begin(); iter != died.end(); ++iter) {  
    pa = *iter;
    //cout << "reaping:"<< (*pa)<< endl;
    agents.erase(pa);
    delete(pa);
  }
  died.clear();
}

void Grid2D::print(){
  for (int i=0; i<size; i++){
    for (int j=0; j<size; j++){
      if (nodos[i][j]) {
        cout << *nodos[i][j] << "|";
      } else {
        cout <<  "< >|";
      }
    }
  }
  cout << "\n";
  cout << "num Agentes: " << agents.size() << endl;
  cout << "Lista Agentes:";
  //copy(agents.begin(), agents.end(), ostream_iterator<Agent>(cout," "));
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


  
list<Agent*> Grid2D::getNeighbourgs(Position *pos){



  list<Agent*> ns;

  int xIzq, xDer, yUp, yDown = -1;
  int posX = pos->par.x;
  int posY = pos->par.y;

  xIzq = posX - 1;
  xDer = posX + 1;
  yUp = posY - 1;
  yDown = posY + 1;
  
  if (posX == 0)
    xIzq = size -1;
  if (posX == size-1)
    xDer = 0;
  if (posY == 0)
    yUp = size -1;
  if (posY == size-1)
    yDown = 0;
  
  if (nodos[xIzq][posY])
    ns.push_back(nodos[xIzq][posY]);
  if (nodos[xDer][posY])
    ns.push_back(nodos[xDer][posY]);
  if (nodos[posX][yUp])
    ns.push_back(nodos[posX][yUp]);
  if (nodos[posX][yDown])
    ns.push_back(nodos[posX][yDown]);

  return ns;
}


int Grid2D::getNumAgents(){
  return agents.size();
}

int Grid2D::getNumCooperators(){

  list<Agent*> ags;
  int count = 0;
  Agent* a;

  set<Agent*>::iterator i;
  for (i = agents.begin(); i != agents.end(); ++i) {  
    a = *i;
    if ((a->getType()) == COOP){
      count++;
    }
  }
  return count;
}


int Grid2D::countType(list<Agent*> aList, AType pType){

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


void Grid2D::removeAgentAt(Position *pos){
  
  int x = pos->par.x;
  int y = pos->par.y;
  //cout <<"removeAgentAt: ("<< x <<"," << y <<")" << endl;
  died.push_back(nodos[x][y]);
  //agents.erase(*nodos[x][y]);
  nodos[x][y] = NULL;
}


Grid2D::~Grid2D(){
  for (int i = 0 ; i< size; i++) {
    delete[] nodos[i];
  }
  delete[] nodos; 
}
