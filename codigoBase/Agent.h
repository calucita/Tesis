// Agent.h
// 
//

#ifndef __AGENT__
#define __AGENT__

#include <iostream>
#include <string>
#include <list>
#include "global.h"
#include "Position.h"


using namespace std;

class World;

class Agent{

public:

  //  Agent(AType pType=COOP, Grid1D *pWorld=NULL);
  Agent(AType pType=COOP, World *pWorld=NULL);
  Agent(const Agent& a);

  //Agent(AType pType=COOP);
  bool predate(int umCoopNeighbours);
  
  void change(int DiffNeigN);
  void changeType();
  AType getType(){return type;};
  int getId(){return id;};
  
  void step();
  void die();
  void clean();
  
  Position* getPos();
  //int getPosX();
  //int getPosY();
  
  void setPos(Position *pos);
  //void setPos(int i);
  //void setPos(int i, int j);

  void removeFromWorld();
  

  friend ostream& operator<<(ostream& out, const Agent& a);
  friend bool operator<(const Agent& a, const Agent& b);
  friend bool operator==(const Agent& a, const Agent& b);
  //friend bool operator<(const Agent* a, const Agent* b);
  ~Agent();
  
  //World world;

 private:
  int id;
  int age;
  AType type;
  float mortalityRate;
  Position *pos;
  // int x;
  //int y;
  World *world;
  
  //Position pos;
  static int currentId;

  // static float coopMortalityRate;
//   static float noCoopMortalityRate;

//   static bool CT; // Cultural Transmission is Active
//   static float CTProb; // Cultural Transmission Probability

  
//   static float predProb0n;
//   static float predProb1n;
//   static float predProb2n;
  
//   static float predProbNCo;


  //  static set<Agent> agents;
  //  static list<Agent*> died;

  bool checkToDie();

  // Variables para definir el tipo de mayoria y si 
  // la Tasa de Transmision es proporcional a la mayoria
  // PASAR A PARAMETROS DEL MODELO
  static bool absolutMayority ;
  static bool proportionalProbCT;
  static int groupSize;


};

#endif
