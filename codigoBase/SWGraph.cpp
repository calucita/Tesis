
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <fstream>
#include <iterator>
#include "global.h"
#include "World.h"
#include "Agent.h"
#include "SWGraph.h"


using namespace std;


// Newman-Watts'  Small World  Network (Small_World Graph)
// G(n,k,p)
// n --> pSize
// k --> k connections on each direction
// p --> probability of rewiring for each arc.
// pRepType --> reproduction Type
// El constructor crea la estructura del grafo, no coloca agentes en el.

SWGraph::SWGraph(int n, int k, double p,  RepType pRepType){
    
  float r;
  int rNode;
  int j;

  size = n;
  repType = pRepType;
 
  cout << "SWGraph::constructor: N=" <<n << " k="<< k << " p=" << p << "\n";


  // Creo la lista de adyacencias para cada nodo
  // Cada nodo se identifica con un entero 
  for (int i=0; i<n; i++){
    g[i].clear();
    nMap[i] = NULL;
  }
  
  // Grafo inicial. Anillo + k conecciones
  for (int i=0; i<n; i++){
    for (int j=1; j<=k; j++) {
      this->addArc(i,(i+j)%n);
      this->addArc(i,(n+(i-j))%n);
      //cout << "arco: (" << i <<"," << (i+j)%n <<")"<< endl;
      //cout << "arco: (" << i <<"," << (n+(i-j))%n <<")"<< endl;
    }
  }

  // Limpiar arcos repetidos
  for (int i=0; i<size; i++){
    std::sort(g[i].begin(), g[i].end());
    g[i].erase(std::unique(g[i].begin(), g[i].end()), g[i].end()); 
  }  

  // Reconexiones
  for (int i=0; i<n; i++){
    r = (float)rand()/RAND_MAX;
    if (r<=p){
      rNode = ((int)rand()) % n;
      //cout << "rNode:" << rNode << endl;
      // cambiando un enlace hacia rNode
      j = rand() %  g[i].size();
      vector<int>& neigs = g.find(i)->second;
      //cout << "antes: " << neigs[j] << endl;
      neigs[j] = rNode; 
      //cout << "despues:" << neigs[j] << endl;
    }
  }


}




void SWGraph::populate(){
  //cout<<"populate"<<"\n";
  float r;
  Agent *a = NULL;
  Position *pos = NULL;

  cout<<"SWGraph:populate:size = "<<size <<"\n";
  cout<<"SWGraph:populate:proporcionTotal = "<<  coopGlobalArgs.propCoIni +  coopGlobalArgs.propNCoIni <<"\n";

  for (int i=0; i<size; i++){
    r = (float)rand()/RAND_MAX;
    if (r <=  coopGlobalArgs.propCoIni +  coopGlobalArgs.propNCoIni){ 
      a = new Agent(r<= coopGlobalArgs.propCoIni ? COOP : NOCOOP, this);
      pos = new Position;
      pos->i = i;

      //cout<<"cree agente en la posicion "<< i <<"\n";
      a->setPos(pos);

      this->nMap[i] = a;

      agents.insert(a);
    }
  }
  
  //cout << "SWGraph::populate: " << agents.size() << endl;

}



void SWGraph::repopulate(int nAgents, int nCoops, int nNoCoops){

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



void SWGraph::step(){
  
  Agent* a = NULL;
  set<Agent*>::iterator i;
 
  //cout << "antes del step. NAgs =" << agents.size() <<"\n";

  for (i = agents.begin(); i != agents.end(); ++i) {  
    a = *i;
    //cout << *a;
    a->step();
  }
  //this->print();
  reapAgents();

  //cout << "despues del step. NAgs =" << agents.size() <<"\n";

}


void SWGraph::reapAgents(){

  //cout << "SWGraph::Reaping\n";
  list<Agent*>::iterator iter;
  
  //cout << "hay " << died.size() << " muertos \n";
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
void SWGraph::print(){
  cout << "\nSWGraph::print" << endl;
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


  
list<Agent*> SWGraph::getNeighbourgs(Position *p){

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


int SWGraph::getNumAgents(){
  return agents.size();
}

int SWGraph::getNumCooperators(){

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


int SWGraph::countType(list<Agent*> aList, AType pType){

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


void SWGraph::removeAgentAt(Position *p){
  
  int pos = p->i;
  //cout <<"removeAgentAt: "<< pos << endl;
  died.push_back(nMap[pos]);
  nMap[pos] = NULL;
}


//OJO: Preguntarle a Julio como hacerlo
SWGraph::~SWGraph(){
  //delete[] nMap;
  //delete[] g;
}



void SWGraph::addArc(int i, int j){

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


//void SWGraph::removeRandomArc(int iNode){
//}

void SWGraph::getRealConnectivities(ofstream& fp){
  
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


