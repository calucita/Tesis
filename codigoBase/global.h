// gloabal.h
// 
//
#include <iostream>
#include <string>

#ifndef __GLOBAL__
#define __GLOBAL__


enum AType{COOP, NOCOOP};
enum RepType{UNIFORM, PROPORTIONAL, PROP_LOCAL};
enum WorldType{GRID1D, GRID2D, RND_GRAPH, BA_GRAPH, SW_GRAPH, WELL_MIXED};

struct globalArgs_t {
  int world; 		  /* --world -2 option: world type */ 
  int size; 		  /* --size -s option: world size */ 
  int iters;          /* --iters -i option: # of Iterations */ 
  float propCoIni;       /* --coop  option: Initial proportion of
                         cooperators */  
  float propNCoIni;     /* --ncoop option: Initial proportion of
                         non cooperators */ 
  float predProb0n;   /* --p0n option: Predation probability for 
                               a cooperator without cooperator neighbour */
  float predProb1n;   /* --p1n option: Predation probability for
                               a cooperator with one cooperator neighbour */
  float predProb2n;   /* --p2n option: Predation probability for
                               a cooperator with two cooperator neighbour */
  float predProbNCo;  /* --pNCo, Predation probability for a non
                               cooperator */ 
  bool CT;            /* --CT option: activate CT */
  float probCT;       /* --pCT option: Cultural Transmission
                                      Probability */ 
  float coopMortalityRate;    /* --pMCo option: Cooperator
                                         Mortality rate */
  float noCoopMortalityRate;  /* --pMNCo option: Non Cooperator
                                 Mortality rate */ 

  RepType repType;        /* --repType option: ReproductionType,
                             choices=["uniform","proportional","localP"] */    
  
  char* outFileName;          /* -o option: Output File: <#iter>
                                       <#Cooperators> <#Non Cooperators> */   
  bool migration;           /* --migration option: activate migration */
  bool randomize;                   /* -r option: activate random */

  double p;              /* p parameter of a Random Graph G(n,p)*/
  int m;                 /* m parameter of a Barabasi-Albert Graph G(n,m)*/

  int k;                 /* k parameter of a Small World Graph G(n,k,p)*/
  double pRewire;        /* p parameter of a Small World Graph G(n,k,p)*/
  
  bool freeRiders;
  bool strictMajority;
  bool proportionalHTrans;


};


extern globalArgs_t coopGlobalArgs;
//struct globalArgs_t coopGlobalArgs;

#endif
