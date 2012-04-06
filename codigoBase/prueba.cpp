#include <set>
#include <iterator>
#include <algorithm>
#include <iostream>
//#include <string>
//#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <getopt.h>
#include "global.h"
#include "Agent.h"
#include "Grid1D.h"
#include "Grid2D.h"
#include "RandomGraph.h"
#include "BAGraph.h"
#include "SWGraph.h"
#include "NoSpace.h"

extern struct globalArgs_t coopGlobalArgs;

using namespace std;


extern char *optarg;
//extern int optind, opterr;
//char* outFileName;


static const char *optString = "w:s:i:o:p:m:k:trh?f";

static const struct option longOpts[] = {
  {"world", required_argument, NULL, 'w'},
  {"size", required_argument, NULL, 's'},
  {"iters", required_argument, NULL, 'i'},
  {"coop", required_argument, NULL, 0},
  {"ncoop", required_argument, NULL, 0},
  {"p0n", required_argument, NULL, 0},
  {"p1n", required_argument, NULL, 0},
  {"p2n", required_argument, NULL, 0},
  {"pNCo", required_argument, NULL, 0},
  {"CT", no_argument, NULL, 't'},  
  {"pCT", required_argument, NULL, 0},
  {"pMCo", required_argument, NULL, 0},
  {"pMNCo", required_argument, NULL, 0},

  {"pRew", required_argument, NULL, 0},

  {"repType", required_argument, NULL, 0},

  {"output", required_argument, NULL, 'o' },
  {"migration", no_argument, NULL, 0 },
  {"randomize", no_argument, NULL, 'r' },

  {"freeRiders", no_argument, NULL, 'f' },
  {"simpleMajority", no_argument, NULL, 0 },
  {"propHT", no_argument, NULL, 0 },




  {"help", no_argument, NULL, 'h' },


  { NULL, no_argument, NULL, 0 }
};


void runSimulation1D(){

  World* theWorld; 
  int nAgents = 0;
  int nCoops = 0;
  int nNoCoops = 0;
  string myStr = "";
  char buffer[1000];
  ofstream outFile;

    
  string s1= coopGlobalArgs.outFileName;

  // Abro archivos de datos
  ofstream outStream((s1+".dat").c_str());
  ofstream finalOutStream((s1+".dat.final").c_str(), 
                          std::ios::out|std::ios::app|std::ios::ate);
  


  
  theWorld = new Grid1D(coopGlobalArgs.size, coopGlobalArgs.repType);
  theWorld->populate();
  

  nAgents = theWorld->getNumAgents();
  nCoops = theWorld->getNumCooperators();
  nNoCoops = nAgents - nCoops;
  sprintf(buffer, "%d\t %d\t %d\t %d\n", 0, nAgents, nCoops, nNoCoops);
  myStr = buffer;
  cout << myStr;
  outStream << myStr;

  for (int t=1; t<=coopGlobalArgs.iters;++t){
    
    //cout << "\nMundo antes de step" << endl;
    //theWorld->print(); 

    theWorld->step();
    //cout << "\nMundo despues de step" << endl;
    //theWorld->print(); 
    
    theWorld->reapAgents();
    
    //cout << "\nMundo despues de reap" << endl;
    //theWorld->print(); 

    nAgents = theWorld->getNumAgents();
    nCoops = theWorld->getNumCooperators();
    nNoCoops = nAgents - nCoops;
    

    theWorld->repopulate(nAgents, nCoops, nNoCoops);

    sprintf(buffer, "%d\t %d\t %d\t %d\n", t, nAgents, nCoops, nNoCoops);
    myStr = buffer;
    //cout << myStr;
    cout << ".";
    outStream << myStr;
  }
 
   cout << "\n";
  finalOutStream << myStr;

  outStream.close();
  finalOutStream.close();
  
}


void runSimulation2D(){

  World* theWorld; 
  int nAgents = 0;
  int nCoops = 0;
  int nNoCoops = 0;
  string myStr = "";
  char buffer[50];
  ofstream outFile;

  string s1= coopGlobalArgs.outFileName;
  
  cout << "Corriendo Grid 2D\n";
  cout << "iters = " << coopGlobalArgs.iters <<endl;

  ofstream outStream((s1+".dat").c_str());
  ofstream finalOutStream((s1+".dat.final").c_str(), ofstream::out | ofstream::app);

 
  theWorld = new Grid2D(coopGlobalArgs.size,coopGlobalArgs.repType);
  theWorld->populate();
  //theWorld->print();

  nAgents = theWorld->getNumAgents();
  nCoops = theWorld->getNumCooperators();
  nNoCoops = nAgents - nCoops;
  sprintf(buffer, "%d\t %d\t %d\t %d\n", 0, nAgents, nCoops, nNoCoops);
  myStr = buffer;
  cout << myStr;
  outStream << myStr;

  for (int t=1; t<=coopGlobalArgs.iters;++t){
    theWorld->step();
    theWorld->reapAgents();
    //theWorld->print();

    nAgents = theWorld->getNumAgents();
    nCoops = theWorld->getNumCooperators();
    nNoCoops = nAgents - nCoops;

    //cout << "\n world repopulate\n";
    theWorld->repopulate(nAgents, nCoops, nNoCoops);

    sprintf(buffer, "%d\t %d\t %d\t %d\n", t, nAgents, nCoops, nNoCoops);
    myStr = buffer;
    //cout << myStr;
    cout << ".";
    outStream << myStr;
    
  }
  cout << "\n";

  finalOutStream << myStr;
  outStream.close();
  finalOutStream.close();
}




void runSimulationRndGraph(){

  World* theWorld; 
  int nAgents = 0;
  int nCoops = 0;
  int nNoCoops = 0;
  string myStr = "";
  char buffer[50];
  ofstream outFile;

  string s1= coopGlobalArgs.outFileName;
  
  cout << "Corriendo Erdos (Random Graph)\n";
  cout << "iters = " << coopGlobalArgs.iters <<endl;
  cout << "p = " << coopGlobalArgs.p <<endl;

  ofstream outStream((s1+".dat").c_str());
  ofstream finalOutStream((s1+".dat.final").c_str(), ofstream::out | ofstream::app);
  ofstream vecinosStream((s1+".vecinos.dat").c_str(), ofstream::out | ofstream::app);
  ofstream vecinosIniStream((s1+".vecinos.ini.dat").c_str(), ofstream::out | ofstream::app);

  
  // Creo un grafo el Erdos-Renyi
  theWorld = new RandomGraph(coopGlobalArgs.size, coopGlobalArgs.p, coopGlobalArgs.repType); 
  theWorld->populate();
  //theWorld->print();

  theWorld->getRealConnectivities(vecinosIniStream);

  nAgents = theWorld->getNumAgents();
  nCoops = theWorld->getNumCooperators();
  nNoCoops = nAgents - nCoops;
  sprintf(buffer, "%d\t %d\t %d\t %d\n", 0, nAgents, nCoops, nNoCoops);
  myStr = buffer;
  cout << myStr;
  outStream << myStr;

  for (int t=1; t<=coopGlobalArgs.iters;++t){
    theWorld->step();
    theWorld->reapAgents();
    //theWorld->print();

    nAgents = theWorld->getNumAgents();
    nCoops = theWorld->getNumCooperators();
    nNoCoops = nAgents - nCoops;

    //cout << "\n world repopulate\n";
    theWorld->repopulate(nAgents, nCoops, nNoCoops);

    sprintf(buffer, "%d\t %d\t %d\t %d\n", t, nAgents, nCoops, nNoCoops);
    myStr = buffer;
    //cout << myStr;
    cout << ".";
    outStream << myStr;
    
    // Estadísticas 
   
  }

  cout << "\n";
  finalOutStream << myStr;


  theWorld->getRealConnectivities(vecinosStream);

  outStream.close();
  finalOutStream.close();  
  vecinosStream.close();
  vecinosIniStream.close();

}




void runSimulationBAGraph(){

  World* theWorld; 
  int nAgents = 0;
  int nCoops = 0;
  int nNoCoops = 0;
  string myStr = "";
  char buffer[50];
  ofstream outFile;

  string s1= coopGlobalArgs.outFileName;
  
  cout << "Corriendo Barabasi Albert\n";
  cout << "iters = " << coopGlobalArgs.iters <<endl;
  cout << "n = " << coopGlobalArgs.size <<endl;
  cout << "m = " << coopGlobalArgs.m <<endl;


  ofstream outStream((s1+".dat").c_str());
  ofstream finalOutStream((s1+".dat.final").c_str(), ofstream::out | ofstream::app);
  ofstream vecinosStream((s1+".vecinos.dat").c_str(), ofstream::out | ofstream::app);
  ofstream vecinosIniStream((s1+".vecinos.ini.dat").c_str(), ofstream::out | ofstream::app);
  

  // Creo el grafo Barabasi-Albert
  theWorld = new BAGraph(coopGlobalArgs.size, coopGlobalArgs.m, coopGlobalArgs.repType); 
  theWorld->populate();
  //theWorld->print();

  theWorld->getRealConnectivities(vecinosIniStream);

  nAgents = theWorld->getNumAgents();
  nCoops = theWorld->getNumCooperators();
  nNoCoops = nAgents - nCoops;
  sprintf(buffer, "%d\t %d\t %d\t %d\n", 0, nAgents, nCoops, nNoCoops);
  myStr = buffer;
  cout << myStr;
  outStream << myStr;

  for (int t=1; t<=coopGlobalArgs.iters;++t){
    theWorld->step();
    theWorld->reapAgents();
    //theWorld->print();

    nAgents = theWorld->getNumAgents();
    nCoops = theWorld->getNumCooperators();
    nNoCoops = nAgents - nCoops;

    //cout << "\n world repopulate\n";
    theWorld->repopulate(nAgents, nCoops, nNoCoops);

    sprintf(buffer, "%d\t %d\t %d\t %d\n", t, nAgents, nCoops, nNoCoops);
    myStr = buffer;
    //cout << myStr;
    cout << ".";
    outStream << myStr;
    
  }

  cout << "\n";
  finalOutStream << myStr;
  theWorld->getRealConnectivities(vecinosStream);

  outStream.close();
  finalOutStream.close();
  vecinosIniStream.close(); 
  vecinosStream.close();

}



void runSimulationSWGraph(){

  World* theWorld; 
  int nAgents = 0;
  int nCoops = 0;
  int nNoCoops = 0;
  string myStr = "";
  char buffer[50];
  ofstream outFile;

  string s1= coopGlobalArgs.outFileName;
  
  cout << "Corriendo Small World\n";
  cout << "iters = " << coopGlobalArgs.iters <<endl;
  cout << "n = " << coopGlobalArgs.size <<endl;
  cout << "k = " << coopGlobalArgs.k <<endl;
  cout << "pRewire = " << coopGlobalArgs.pRewire <<endl;


  ofstream outStream((s1+".dat").c_str());
  ofstream finalOutStream((s1+".dat.final").c_str(), ofstream::out | ofstream::app);
  ofstream vecinosStream((s1+".vecinos.dat").c_str(), ofstream::out | ofstream::app);
  ofstream vecinosIniStream((s1+".vecinos.ini.dat").c_str(), ofstream::out | ofstream::app);
  
  // Creo un grafo Newman-Watts
  theWorld = new SWGraph(coopGlobalArgs.size, coopGlobalArgs.k, coopGlobalArgs.pRewire, coopGlobalArgs.repType); 
  theWorld->populate();
  //theWorld->print();

  theWorld->getRealConnectivities(vecinosIniStream);

  nAgents = theWorld->getNumAgents();
  nCoops = theWorld->getNumCooperators();
  nNoCoops = nAgents - nCoops;
  sprintf(buffer, "%d\t %d\t %d\t %d\n", 0, nAgents, nCoops, nNoCoops);
  myStr = buffer;
  cout << myStr;
  outStream << myStr;

  for (int t=1; t<=coopGlobalArgs.iters;++t){
    theWorld->step();
    theWorld->reapAgents();
    //theWorld->print();

    nAgents = theWorld->getNumAgents();
    nCoops = theWorld->getNumCooperators();
    nNoCoops = nAgents - nCoops;

    //cout << "\n world repopulate\n";
    theWorld->repopulate(nAgents, nCoops, nNoCoops);

    sprintf(buffer, "%d\t %d\t %d\t %d\n", t, nAgents, nCoops, nNoCoops);
    myStr = buffer;
    //cout << myStr;
    cout << ".";
    outStream << myStr;
    
  }

  cout << "\n";
  finalOutStream << myStr;
  theWorld->getRealConnectivities(vecinosStream);

  outStream.close();
  finalOutStream.close(); 
  vecinosStream.close(); 
  vecinosIniStream.close();
}




void runSimulationWellMixed(){

  World* theWorld; 
  int nAgents = 0;
  int nCoops = 0;
  int nNoCoops = 0;
  string myStr = "";
  char buffer[50];
  ofstream outFile;

  string s1= coopGlobalArgs.outFileName;
  
  cout << "Corriendo Well Mixed\n";
  cout << "iters = " << coopGlobalArgs.iters <<endl;
  cout << "n = " << coopGlobalArgs.size <<endl;
  
  ofstream outStream((s1+".dat").c_str());
  ofstream finalOutStream((s1+".dat.final").c_str(), ofstream::out | ofstream::app);
  ofstream vecinosStream((s1+".vecinos.dat").c_str(), ofstream::out | ofstream::app);
  ofstream vecinosIniStream((s1+".vecinos.ini.dat").c_str(), ofstream::out | ofstream::app);
  
  
  // Creo una poblacion Well Mixed
  theWorld = new NoSpace(coopGlobalArgs.size, coopGlobalArgs.repType); 
  theWorld->populate();
  //theWorld->print();

  theWorld->getRealConnectivities(vecinosIniStream);

  nAgents = theWorld->getNumAgents();
  nCoops = theWorld->getNumCooperators();
  nNoCoops = nAgents - nCoops;
  sprintf(buffer, "%d\t %d\t %d\t %d\n", 0, nAgents, nCoops, nNoCoops);
  myStr = buffer;
  cout << myStr;
  outStream << myStr;

  for (int t=1; t<=coopGlobalArgs.iters;++t){
    theWorld->step();
    theWorld->reapAgents();
    //theWorld->print();

    nAgents = theWorld->getNumAgents();
    nCoops = theWorld->getNumCooperators();
    nNoCoops = nAgents - nCoops;

    //cout << "\n world repopulate\n";
    theWorld->repopulate(nAgents, nCoops, nNoCoops);

    sprintf(buffer, "%d\t %d\t %d\t %d\n", t, nAgents, nCoops, nNoCoops);
    myStr = buffer;
    //cout << myStr;
    cout << ".";
    outStream << myStr;
    
  }

  cout << "\n";
  finalOutStream << myStr;
  theWorld->getRealConnectivities(vecinosStream);

  outStream.close();
  finalOutStream.close(); 
  vecinosStream.close(); 
  vecinosIniStream.close();
}







void display_usage(){

  cout << "cooperation1D [options]" << endl;
  cout << "--world|-w <WT>: World Type WT=[grid1D, grid2D, erdos, barabasi, watts]" << endl;
  cout << "--size|-s <N>: World Size" << endl;
  cout << "--iters|-i <N>: Iterations number" << endl;
  cout << "--coop <N>: Initial proportion of cooperators, N in [0,1]" <<endl;
  cout << "--ncoop <N>: Initial proportion of non cooperators, N in [0,1]" <<endl;
  cout << "--p0n <N>: Predation probability for a cooperator without cooperator neighbour" <<endl; 
  cout << "--p1n <N>: Predation probability for a cooperator with one cooperator neighbour" <<endl; 
  cout << "--p2n <N>: Predation probability for a cooperator with two cooperator neighbour" <<endl; 
  cout << "--pNCo <N>: Predation probability for a non cooperator" <<endl; 
  cout << "--CT: activate Cultural Transmission" <<endl;
  cout << "--pCT <N>: Cultural Transmission Probability" <<endl;
  cout << "--pMCo <N>: Cooperator's Mortality rate" <<endl;
  cout << "--pMNCo <N>: Non Cooperator's Mortality rate" <<endl;
  cout << "--repType <T>: ReproductionType T=[uniform,proportional,localP]" <<endl;
  cout << "--migration: activate migration" <<endl;
  cout << "--randomize|-r: random seed with local clock" <<endl;
  cout << "--output <file>: Output File: <#iter> <#Cooperators> <#Non Cooperators>" <<endl;

  cout << "--freeRiders: activate free riders " <<endl;
  cout << "--simpleMajority: activate simple majority " <<endl;
  cout << "--propHT: activate proportional horizontal information transmission" <<endl; 


}


// Setting the default parameters
void initOptions(){

  coopGlobalArgs.world = GRID1D;
  coopGlobalArgs.size = 10000;
  coopGlobalArgs.iters = 500;
  coopGlobalArgs.propCoIni = 0.5;
  coopGlobalArgs.propNCoIni = 0.5;
  coopGlobalArgs.predProb0n = 0.01;
  coopGlobalArgs.predProb1n = 0.01;
  coopGlobalArgs.predProb2n = 0.01;
  coopGlobalArgs.predProbNCo = 0.01; 
  coopGlobalArgs.CT = false;
  coopGlobalArgs.probCT = 0.5;
  coopGlobalArgs.coopMortalityRate = 0.1;
  coopGlobalArgs.noCoopMortalityRate = 0.1;
  coopGlobalArgs.repType = UNIFORM;
  coopGlobalArgs.migration = false;
  coopGlobalArgs.randomize = false;
  coopGlobalArgs.outFileName = "output";

  coopGlobalArgs.p = 0.004;   // 0.025; //=0.1
  coopGlobalArgs.m = 2; // Para el modelo Barabasi-Albert (Small-World)

  coopGlobalArgs.k = 2;
  coopGlobalArgs.pRewire = 0.01;

  coopGlobalArgs.freeRiders = false;
  coopGlobalArgs.strictMajority = true;
  coopGlobalArgs.proportionalHTrans = false;
  
}

void printOptions(){
  
  string name = coopGlobalArgs.outFileName;
  string ext = ".params";
  ofstream os((name+ext).c_str());
  os << "world: " << coopGlobalArgs.world << endl; 
  os << "size: " << coopGlobalArgs.size << endl; 
  os << "iters: "<< coopGlobalArgs.iters << endl;
  os << "propCoIni: " << coopGlobalArgs.propCoIni<< endl;
  os << "propNCoIni: " << coopGlobalArgs.propNCoIni<< endl;
  os << "predProb0n: " << coopGlobalArgs.predProb0n << endl;
  os << "predProb1n: " << coopGlobalArgs.predProb1n << endl;
  os << "predProb2n: " << coopGlobalArgs.predProb2n << endl;
  os << "predProbNCo: " << coopGlobalArgs.predProbNCo << endl;
  os << "CT:" << coopGlobalArgs.CT << endl;
  os << "probCT: "<< coopGlobalArgs.probCT << endl;
  os << "coopMortalityRate: " << coopGlobalArgs.coopMortalityRate << endl;
  os << "noCoopMortalityRate: " <<coopGlobalArgs.noCoopMortalityRate << endl;
  os << "repType: " <<coopGlobalArgs.repType << endl;
  os << "migration: " << coopGlobalArgs.migration << endl;
  os << "randomize: "<< coopGlobalArgs.randomize << endl;
  os << "Random Graph's p: "<< coopGlobalArgs.p << endl;
  os << "Barabasi-Albert Graph's m: "<< coopGlobalArgs.m << endl;

  os << "Small-World' s k: " << coopGlobalArgs.k << endl;;
  os << "Small-World' s p: " <<coopGlobalArgs.pRewire << endl;

  os << "file: " <<coopGlobalArgs.outFileName << endl;
  
  os << "freeRiders: "<< coopGlobalArgs.freeRiders << endl;
  os << "strictMayority: "<< coopGlobalArgs.strictMajority << endl;
  os << "proportionalHorizontalTransmission: "<< coopGlobalArgs.proportionalHTrans << endl;

  os.close();
}

void showOptions(){

  cout << "Show Options \n";

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

  cout << "Random Graph's p: "<< coopGlobalArgs.p << endl;
  cout << "Barabasi-Albert Graph's m: "<< coopGlobalArgs.m << endl;

  cout << "Small-World' s k: " << coopGlobalArgs.k << endl;;
  cout << "Small-World' s p: " <<coopGlobalArgs.pRewire << endl;

  cout << "file: " <<coopGlobalArgs.outFileName << endl;
  cout << endl;
  
  cout << "freeRiders: "<< coopGlobalArgs.freeRiders << endl;
  cout << "strictMayority: "<< coopGlobalArgs.strictMajority << endl;
  
  cout << "proportionalHorizontalTransmission: "<< coopGlobalArgs.proportionalHTrans << endl;  

}


void readOptions(int argc, char* argv[]){
  int opt = 0;
  int longIndex = 0;
  char* woptarg = NULL;

  cout << "Leyendo Opciones\n";
  opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
  while( opt != -1 ) {
    switch( opt ) {
    case 'w': 
      cout << "argumento: "<< optarg <<"\n"; 
      if ( strcmp("grid1D", optarg) == 0 ) {
        coopGlobalArgs.world = GRID1D;
      }
      else if ( strcmp("grid2D", optarg) == 0 ) {
        coopGlobalArgs.world = GRID2D;
      }
      else if ( strcmp("rndGraph", optarg) == 0 ) {
        coopGlobalArgs.world = RND_GRAPH;
      }
      else if ( strcmp("BAGraph", optarg) == 0 ) {
        coopGlobalArgs.world = BA_GRAPH;
      }
      else if ( strcmp("SWGraph", optarg) == 0 ) {
        coopGlobalArgs.world = SW_GRAPH;
      }
      else if ( strcmp("WellMixed", optarg) == 0 ) {
        coopGlobalArgs.world = WELL_MIXED;
      }
      else {
        cout << "worldType: Incorrect option\n";
        exit(-1);
      }
      cout << "worldType: "<< coopGlobalArgs.world <<"\n"; 
      break;
    case 's': 
      coopGlobalArgs.size = atoi(optarg);
      cout << "size: "<< coopGlobalArgs.size <<"\n"; 
      break;
    case 'i':
      coopGlobalArgs.iters = atoi(optarg);
      cout << "iters: "<< coopGlobalArgs.iters <<"\n";
      break;
    case 'p':
      coopGlobalArgs.p = atof(optarg);
      cout << "Random Graph's p: "<< coopGlobalArgs.p <<"\n";
      break;
    case 't':
      coopGlobalArgs.CT = true;
      cout << "CT" << "\n"; 
      break;
    case 'o':
      coopGlobalArgs.outFileName = optarg;
      //outFileName = optarg;
      cout << "file: "<< coopGlobalArgs.outFileName <<"\n"; 
      //cout << "file: "<< outFileName <<"\n"; 
      break;
    case 'm':
      coopGlobalArgs.m = atoi(optarg);
      cout << "Barabasi-Albert's m: "<< coopGlobalArgs.m <<"\n";
      break;

    case 'k':
      coopGlobalArgs.m = atoi(optarg);
      cout << "Newman-Watts' k: "<< coopGlobalArgs.k <<"\n";
      break;

    case 'r':
      coopGlobalArgs.randomize = true;
      cout << "randomize"<<"\n"; 
      break;
    case 'h':	/* fall-through is intentional */
    case '?':
      display_usage();
      exit(1);
      break;
    case 'f':
      coopGlobalArgs.freeRiders = true;
      cout << "free riders\n";
      break;
    case 0:		/* long option without a short arg */
      if( strcmp( "coop", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.propCoIni = atof(optarg);
        cout << "propCoIni: "<< coopGlobalArgs.propCoIni <<"\n"; 
      }
      if( strcmp( "ncoop", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.propNCoIni = atof(optarg);
        cout << "propNCoIni: "<< coopGlobalArgs.propNCoIni <<"\n"; 
      }
      if( strcmp( "p0n", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.predProb0n = atof(optarg);
        cout << "predProb0n: "<< coopGlobalArgs.predProb0n <<"\n"; 
      }
      if( strcmp( "p1n", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.predProb1n = atof(optarg);
        cout << "predProb1n: "<< coopGlobalArgs.predProb1n <<"\n"; 
      }
      if( strcmp( "p2n", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.predProb2n = atof(optarg);
        cout << "predProb2n: "<< coopGlobalArgs.predProb2n <<"\n"; 
      }
      if( strcmp( "pNCo", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.predProbNCo = atof(optarg);
        cout << "predProbNCo: "<< coopGlobalArgs.predProbNCo <<"\n"; 
      }
      if( strcmp( "pCT", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.probCT = atof(optarg);
        cout << "probCT: "<< coopGlobalArgs.probCT <<"\n"; 
      }
      if( strcmp( "pMCo", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.coopMortalityRate = atof(optarg);
        cout << "coopMortalityRate: "<< coopGlobalArgs.coopMortalityRate <<"\n"; 
      }
      if( strcmp( "pMNCo", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.noCoopMortalityRate = atof(optarg);
        cout << "noCoopMortalityRate: "<< coopGlobalArgs.noCoopMortalityRate <<"\n"; 
      }
      if( strcmp( "pRew", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.pRewire = atof(optarg);
        cout << "pRewire: "<< coopGlobalArgs.pRewire <<"\n"; 
      }
      if( strcmp( "repType", longOpts[longIndex].name ) == 0 ) {
        if ( strcmp("uniform", optarg) ==0 ) {
          coopGlobalArgs.repType = UNIFORM;
        }
        if ( strcmp("proportional", optarg) ==0 ) {
          coopGlobalArgs.repType = PROPORTIONAL ;
        }
        if ( strcmp("localP", optarg) ==0 ) {
          coopGlobalArgs.repType = PROP_LOCAL;
        }
        cout << "repType: "<< coopGlobalArgs.repType <<"\n";
      }

      if( strcmp( "migration", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.migration = true;
        cout << "migration "<<"\n"; 
      }

      if( strcmp( "simpleMajority", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.strictMajority = false;
        cout << "simple majority "<<"\n"; 
      }

      if( strcmp( "propHT", longOpts[longIndex].name ) == 0 ) {
        coopGlobalArgs.proportionalHTrans = true;
        cout << "proportional horizontal transmission"<<"\n"; 
      }
      
      break;
    default:
      /* You won't actually get here. */
      break;
    }
		opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	}
}




int main (int argc, char* argv[])
{
  //AGraph g(10);
  initOptions();
  readOptions(argc, argv);
  printOptions();
  //showOptions();

  if (coopGlobalArgs.randomize)
    srand((unsigned)time( NULL ));
 
  switch (coopGlobalArgs.world){
  case GRID1D: // Valor por defecto
    runSimulation1D();
    break;
  case GRID2D:
    runSimulation2D();
    break;
  case RND_GRAPH:
    runSimulationRndGraph();
    break;
  case BA_GRAPH:
    runSimulationBAGraph();
    break;
  case SW_GRAPH:
    runSimulationSWGraph();
    break;
  case WELL_MIXED:
    runSimulationWellMixed();
    break;
  }
  return 0;
}
