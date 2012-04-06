// gloabal.h
// 
//
#include <iostream>
#include <string>


#ifndef __POSITION__
#define __POSITION__

class Agent;

typedef struct Pair {
  int x;
  int y;
} Pair;


typedef union Position {
  int i;
  Pair par;
  Agent* p;
} Position;



#endif
