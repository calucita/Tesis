// Agent.cpp
// 
//
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>

#include "global.h"
#include "Agent.h"
#include "World.h"


//extern struct globalArgs_t coopGlobalArgs;

using namespace std;
  
// PASAR A PARAMETROS DEL MODELO
bool Agent::absolutMayority = true;
bool Agent::proportionalProbCT = false;
int Agent::groupSize = 2;
 
int Agent::currentId = 0;


Agent::Agent(AType pType, World *pWorld){
  
  id = currentId++;
  age = 0;
  type = pType;
  world = pWorld;
  pos = NULL;

  switch (type) {
  case COOP:
    mortalityRate = coopGlobalArgs.coopMortalityRate;
    break;
  case NOCOOP:
    mortalityRate = coopGlobalArgs.noCoopMortalityRate;
    break;

  }
}


Agent::Agent(const Agent& a){
  id = a.id;
  age = a.age;
  type = a.type;
  mortalityRate = a.mortalityRate;
  pos = a.pos;
  world = a.world;
}


void Agent::step(){

  list<Agent*> ln;
  int ncoops = 0;
  bool dead = false;

  int nNoCoops = 0;        // Numero de Vecinos No Cooperadores
  int nNeigs = 0;          // Numero de Vecinos
  float promNeigs = 0;     // El promedio de los vecinos actuales
  float coopsProp = 0;     // Proporcion de Cooperadores
  float noCoopsProp = 0;   // Proporcion de No Cooperadores

  //cout << "Agent::step::"<< *this <<"\n";

  ++age;
  ln = world-> getNeighbourgs(pos);
  ncoops = world->countType(ln,COOP);
  //cout << "vecinos cooperadores: " << ncoops << endl;
  //cout.flush();
  

  //
  // Horizontal Information Transmission
  //

  nNeigs = ln.size();
  
  // Solo se activa si hay mas de 1 vecino
  if (nNeigs > 1) {

    // Calculos de las proporciones de cada tipo de vecinos
    nNoCoops = nNeigs - ncoops; 
    promNeigs =(float)nNeigs/2;  
    coopsProp = (float)ncoops/nNeigs;
    noCoopsProp = (float)nNoCoops/nNeigs;
    

    
    // Calculos para cuando la Transmission Horizontal es propocional 
    // a la proporcion de agentes del "otro" grupo 
    
    //cout << "Agent::step::probCT= " << coopGlobalArgs.probCT  << endl;
    if (proportionalProbCT) {
      switch (type) {
      case COOP:
        coopGlobalArgs.probCT = noCoopsProp;
        break;
      case NOCOOP:
        coopGlobalArgs.probCT = coopsProp;
        break;
      }
      // cout << "Agent::step::probCT= " << coopGlobalArgs.probCT  << endl;
    }
    
    
    // Aplicando Mayority Rule
    if ((coopGlobalArgs.probCT > 0.0)){  // no ha muerto y esta activa la transmision cultural
      
      switch (type) {

      case COOP:
        // Grupos de tamamho fijo: GRADO Promedio / 2
        if ((absolutMayority && (nNoCoops > groupSize))  ||  // Mayoria Estricta
            (nNoCoops >= promNeigs) ) {                      // Mayoria Simple
          change(nNoCoops);
        }
        // El grupo tiene el tam del promedio de los vecinos reales
        /*
          if ((absolutMayority && (nNoCoops > promNeigs))  ||  // Mayoria Estricta
            (nNoCoops >= promNeigs) ) {                      // Mayoria Simple
          change(nNoCoops);
        }
        */
      break;
      case NOCOOP:
        // Grupos de tamamho fijo: GRADO Promedio / 2
        if ((absolutMayority && (ncoops > groupSize)) ||     // Mayoria Estricta
            (ncoops >= promNeigs) ) {                         // Mayoria Simple
          change(ncoops);
        }
        // El grupo tiene el tam del promedio de los vecinos reales
        /*
          if ((absolutMayority && (ncoops > promNeigs)) ||     // Mayoria Estricta
            (ncoops >= promNeigs) ) {                         // Mayoria Simple
          change(ncoops);
        }
        */
        break;
      }
    }
  }
  
  //
  // Predation
  //
  dead = predate(ncoops);
  cout.flush();

  //
  // Natural Mortality
  //
  if (!dead) {
    dead = checkToDie();
  }
  
  
  //cout << "fin step" << endl;
}




bool Agent::predate(int coopNeighbourgsN){
  
  float r = (float)rand()/RAND_MAX;
  bool res = false;

  //  cout << "Agent::predate::r=" << r << endl;
  //  cout << "vecinos=" << coopNeighbourgsN << endl;

  
  
  if (coopGlobalArgs.freeRiders) {
    
    if ((coopNeighbourgsN >= 2 && r <= coopGlobalArgs.predProb2n) ||
        (coopNeighbourgsN == 1 && r <= coopGlobalArgs.predProb1n) ||
        (coopNeighbourgsN == 0 && r <= coopGlobalArgs.predProb0n)) {
      die();
      res = true;
    }
    
  } else {
    
    switch (type){
    case COOP:
      if ((coopNeighbourgsN >= 2 && r <= coopGlobalArgs.predProb2n) ||
	  (coopNeighbourgsN == 1 && r <= coopGlobalArgs.predProb1n) ||
	  (coopNeighbourgsN == 0 && r <= coopGlobalArgs.predProb0n)) {
        die();
        res = true;
      }
      break;
    case NOCOOP:
      if (r<=coopGlobalArgs.predProbNCo) {
        die();
        res = true;
      }
      break;
    }
  }
  return res;
}



bool Agent::checkToDie(){
  bool res = false;
  float r = (float)rand()/RAND_MAX;
  if (r <= mortalityRate){
    die();
    res = true;
  }
  return res;
}


void Agent::change(int DiffNeigN){
  
  float r = (float)rand()/RAND_MAX;


  // solo se va a cambiar si hay dos o mas agentes diferentes del otro tipo
  //if (DiffNeigN >=2 && r <= coopGlobalArgs.probCT){
  if (r <= coopGlobalArgs.probCT){
    if (type == COOP){
      type = NOCOOP;
      mortalityRate = coopGlobalArgs.noCoopMortalityRate;
      //cout << "cambio de coop a ncoop\n";
    } else if (r <= coopGlobalArgs.probCT && type == NOCOOP) {
      type = COOP;
      mortalityRate = coopGlobalArgs.coopMortalityRate;
      //cout << "cambio de ncoop a coop\n";
    }
  }
}      



// Ojo al morir hay que sacarlo del mundo:
// de los nodos, el conjunto de agentes y 
// la lista de muertos

// old eliminar
void Agent::die(){
  // Marcar como muerto
  world->removeAgentAt(this->pos);

}


Agent::~Agent(){

}


ostream& operator<<(ostream& out, const Agent& a)
{
  //  out << "<" << a.id << " " << a.pos << " " << a.type << ">";
  out << "<" << a.id << " " << a.type << " " <<a.pos << ">";
  return out;
}


bool operator<(const Agent& a, const Agent& b)
{
  return a.id < b.id;
}


bool operator==(const Agent& a, const Agent& b)
{
  return a.id == b.id;
}


Position* Agent::getPos(){
  return pos; 
}


void Agent::setPos(Position *npos){
  pos = npos;
}



