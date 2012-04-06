
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <fstream>
#include <iterator>
#include "global.h"
#include "World.h"
#include "Agent.h"
#include "BAGraph.h"


using namespace std;


// Barabasi-Albert Graph (Scale_Free Graph)
// G(n,m)
// n --> pSize
// m --> mean degree
// pRepType --> reporduction Type
// El constructor crea la estructura del grafo, no coloca agentes en el.

BAGraph::BAGraph(int n, int m, RepType pRepType){

  float r;
  size = n;
  repType = pRepType;
  int m0 = 3;
  int sumK = 0;
  int tamActual = m0;
  int n1 = n - m0;
  vector<int> k;
  vector<float> p;
  vector<float> cum_pi;
  int k1,k2;

  
  cout << "BAGraph::constructor: N=" <<n << " M="<< m;
  
  // Creo la lista de adyacencias para cada nodo
  // Cada nodo se identifica con un entero 
  for (int i=0; i<n; i++){
    g[i].clear();
    nMap[i] = NULL;
    // Inicializo vectores auxiliares
    k.push_back(0);
    p.push_back(0.0);
    cum_pi.push_back(0.0);
  }
  
  // Grafo inicial. Completamente conexo
  for (int i=0; i<m0; i++){
    for (int j=i+1; j<m0; j++) {
      this->addArc(i,j);
      //cout << "arco: (" << i <<"," << j << ") \n";
    }
  }
  

  for (int i=0; i<m0; i++){
    k[i] = m0-1;
    //cout << "k[" << i << "]=" << k[i] << endl;
    sumK += k[i];
  }
  //cout << "sumK=" << sumK << endl; 
  
  for (int i=m0; i<n; i++){
    //cout << "Agregando nodo: " << i << endl;

  // Calculo cum_pi del grafo actual
    p[0] = (float)k[0]/sumK;
    cum_pi[0] = (float)p[0];
    for (int j=1; j <tamActual; j++){
      p[j] =(float)k[j]/sumK;
      cum_pi[j] = cum_pi[j-1] + p[j];
    }

    /*
      for (int j=0; j <tamActual; j++){
      cout << "p[" << j << "]=" << p[j] << endl;
      cout << "cum_p[" << j << "]=" << cum_pi[j] << endl;
    }
    cout << endl;
    */

    // Agrego m enlaces de acuerdo a cum_pi
    for (int j=0; j<m; j++){
    
      r = (float)rand()/RAND_MAX;
      //cout << "r: " << r << endl;
      int cont = 0;
      while (r >= cum_pi[cont]) {
        cont++;
      }
      addArc(i,cont);
      //cout << "arco: (" << i <<"," << cont << ") \n";
      
      k[i] = k[i]+1;
      k[cont] = k[cont]+1;
      sumK+=2;
    }
    //cout << "termine con el nodo\n\n";
    tamActual++;

  }

}




void BAGraph::populate(){
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

      this->nMap[i] = a;

      agents.insert(a);
    }
  }
  
  cout << "BAGraph::populate: " << agents.size() << endl;

}



void BAGraph::repopulate(int nAgents, int nCoops, int nNoCoops){

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
    
    if (this->nMap[i] == NULL){
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
             this->nMap[i] = a; 
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
          this->nMap[i] = a;
          agents.insert(a);

        }
      }
    }
  }
}



void BAGraph::step(){
  
  Agent* a = NULL;
  set<Agent*>::iterator i;
  
  for (i = agents.begin(); i != agents.end(); ++i) {  
    a = *i;
    //cout << *a;
    a->step();
  }
  //this->print();
}


void BAGraph::reapAgents(){

  //cout << "BAGraph::Reaping\n";
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


// Pedirle ayuda a Julio
void BAGraph::print(){
  cout << "\nBAGraph::print" << endl;
  for (int i=0; i<size; i++){
    if (nMap[i]) {
      cout << *nMap[i] << "|";
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


  
list<Agent*> BAGraph::getNeighbourgs(Position *p){

  list<Agent*> ns;
  int pos = p->i;

  vector<int>& outN = g.find(pos)->second;
  if (!outN.empty()){
    for (vector<int>::iterator vi=outN.begin(); vi!=outN.end(); vi++){
      ns.push_back(nMap[*vi]);
    }
  }
  return ns;
}


int BAGraph::getNumAgents(){
  return agents.size();
}

int BAGraph::getNumCooperators(){

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


int BAGraph::countType(list<Agent*> aList, AType pType){

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


void BAGraph::removeAgentAt(Position *p){
  
  int pos = p->i;
  //cout <<"removeAgentAt: "<< pos << endl;
  died.push_back(nMap[pos]);
  nMap[pos] = NULL;
}


//OJO: Preguntarle a Julio como hacerlo
BAGraph::~BAGraph(){
  //delete[] nMap;
  //delete[] g;
}



void BAGraph::addArc(int i, int j){

  bool found = false;

  // verifica arcos repetidos: LENTO
  //  vector<int>& outN = g.find(i)->second;
//   if (!outN.empty()){
//     for (vector<int>::iterator e=outN.begin(); e!=outN.end(); e++){
//       if (*e == j) {
//         found = true;
//       }
//     }
//   }
//   if (!found) {
//     g[i].push_back(j);
//     g[j].push_back(i);
//   }

  g[i].push_back(j);
  g[j].push_back(i);

}


void BAGraph::getRealConnectivities(ofstream& fp){
  
  // Para cada agente obtener su posicion 
  // y con esa posicion buscar los vecinos 
  // y contarlos.
  
  list<Agent*> vecinos;
  Agent* a = NULL;
  Position* pos=NULL;

  set<Agent*>::iterator i;
  for (i = agents.begin(); i != agents.end(); ++i) {  
    a = *i;
    pos = a->getPos();
    vecinos = getNeighbourgs(pos);
    //cout << a->getId()<< "\t" << vecinos.size()<<"\n";
    fp << a->getId()<< "\t" << vecinos.size()<<"\n";}

}

