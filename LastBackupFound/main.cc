#include <algorithm>
#include <functional>
#include <iostream>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <sysexits.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Agent.hh"
#include "TagAgent.hh"

#ifdef UNICODE_AGENT_STRINGS
#       define   COOP_AGENT_STRING u8"█"
#       define NOCOOP_AGENT_STRING u8"░"
#else
#       define   COOP_AGENT_STRING "@"
#       define NOCOOP_AGENT_STRING " "
#endif


int main(int argc, char * argv[]) {
        // Procesamiento de argumentos de línea de comandos:

        // Opciones de impresión:
        bool         human_readable          ;
        bool         show_grid               ;
        bool         show_avg_fitness_coop   ;
        bool         show_avg_fitness_noncoop;
        bool         show_coop_fraction      ;
	   bool	     f_graph		;
	   bool  	     l_graph	          ;

        // Opciones de simulación:
        bool         randomg                 ;
        bool         toggle                  ;
        bool         tag                     ;
        bool         tag_c	          ;
        bool         adaptive                ;
        double       coop_fraction           ;
        double       coop_cost               ;
        double       coop_benefit            ;
        double       frac = 0                ;
        double       dif                     ;
        unsigned int iters                   ;
        unsigned int rows                    ;
        unsigned int cols                    ;
        unsigned int veci                    ;
        unsigned     con                     ;

        try {
                // Declarar todas las opciones, sus tipos, su texto de ayuda, etc:
                po::options_description desc("Options");

                po::options_description general("General");
                general.add_options()
                        ("help,h", "show this help message")
                ;
                desc.add(general);

                po::options_description output("Output control");
                output.add_options()
                        ("verbose,v"                                                                                         , "enable all output options"                             )
                        ("human-readable,r"        , po::bool_switch        (&human_readable          )->default_value(false), "use human-readable output format"                      )
                        ("show-grid"               , po::bool_switch        (&show_grid               )->default_value(false), "output grid states as text"                            )
                        ("show-avg-fitness-coop"   , po::bool_switch        (&show_avg_fitness_coop   )->default_value(false), "output average cooperator fitness on each iteration"   )
                        ("show-avg-fitness-noncoop", po::bool_switch        (&show_avg_fitness_noncoop)->default_value(false), "output average noncooperator fitness on each iteration")
                        ("show-coop-fraction"      , po::bool_switch        (&show_coop_fraction      )->default_value(false), "output cooperator fraction on each iteration"          )
		         ("first-graph"     	      , po::bool_switch	 (&f_graph	       )->default_value(false), "print igraph-readable random graph"		        )
		         ("last-graph"	      , po::bool_switch	 (&l_graph	       )->default_value(false), "print igraph-readable last adaptive graph"	        )
		         ("tag-count"	      , po::bool_switch	 (&tag_c		       )->default_value(false), "print count of different tags in the population"       )
                ;
                desc.add(output);

                po::options_description sim("Simulation control");
                sim.add_options()
                        ("randomg,g"               , po::bool_switch        (&randomg                 )->default_value(false), "Use the structure of a Random Graph"                   )
                        ("toggle,t"                , po::bool_switch        (&toggle                  )->default_value(false), "operate in toggle mode"                                )
                        ("tag"                     , po::bool_switch        (&tag                     )->default_value(false), "Use Tag selection"                                     )
                        ("adaptive,a"              , po::bool_switch        (&adaptive                )->default_value(false), "Use Adaptive Network setting"                          )
                        ("coop-fraction,f"         , po::value<double      >(&coop_fraction           )->default_value(  0.5), "set initial cooperation fraction"                      )
                        ("coop-cost,c"             , po::value<double      >(&coop_cost               )->default_value(  1  ), "set cooperation cost"                                  )
                        ("coop-benefit,b"          , po::value<double      >(&coop_benefit            )->default_value( 10  ), "set cooperation benefit"                               )
                        ("iters,i"                 , po::value<unsigned int>(&iters                   )->default_value( 50  ), "set iteration count"                                   )
                        ("rows,y"                  , po::value<unsigned int>(&rows                    )->default_value(100  ), "set grid row count"                                    )
                        ("cols,x"                  , po::value<unsigned int>(&cols                    )->default_value(100  ), "set grid column count"                                 )
                        ("dif,d"                   , po::value<double      >(&dif                     )->default_value(  0  ), "set diffusion probability"                             )
                        ("veci"                    , po::value<unsigned int>(&veci                    )->default_value(  4  ), "set number of neighbors in the grid  [1,4,8]"          )
                        ("conex,k"                 , po::value<unsigned int>(&con                     )->default_value(  4  ), "set average number of neighbors in a random g  [1,4,8]")


                ;
                desc.add(sim);

                // y luego ejecutar el procesamiento de esas opciones:
                po::variables_map vm;
                po::store(po::parse_command_line(argc, argv, desc), vm);
                po::notify(vm);

                if (vm.count("help")) {
                        std::cout << desc << std::endl;
                        std::exit(EX_OK);
                }

                // La opción “verbose”  activa todas las opciones de impresión.
                if (vm.count("verbose")) {
                        show_grid                = true;
                        show_avg_fitness_coop    = true;
                        show_avg_fitness_noncoop = true;
                        show_coop_fraction       = true;
                }

                // La opción “f_graph” requiere un random graph.
                if (f_graph) {
			randomg			 = true;
                        show_avg_fitness_coop    = false;
                        show_avg_fitness_noncoop = false;
                        show_coop_fraction       = false;
			show_grid                = false;
                }

                // La opción “l_graph” requiere un random graph adaptativo.
                if (l_graph) {
			randomg			 = true;
			adaptive		 = true;
			show_grid                = false;
                }

		// La opción “tag_c” requieres tags y una forma específica de mostrar los datos
                if (tag_c) {
                        tag	           = true;
			show_coop_fraction	 = true;
                }

                // La opción “randomg” no permite el show_grid.
                if (randomg) {
                        show_grid                =false;
                }

                // Alertar al usuario de que la corrida que mandó a hacer no imprime nada (igual puede ser útil para medir tiempos, por ejemplo).
                if (
                        !show_grid                &&
                        !show_avg_fitness_coop    &&
                        !show_avg_fitness_noncoop &&
                        !show_coop_fraction       &&
                        !f_graph		     &&
			!l_graph		     &&
			!tag_c	 	     &&
			true
                ) {
                        std::cerr << "warning: no output options active" << std::endl;
                }
        } catch(std::exception & e) {
                // Ac;a se viene a parar cuando se usan opciones inválidas:
                std::cerr << "error: " << e.what() << std::endl;
                std::exit(EX_USAGE);
        }



        // Crear el generador de números aleatorios.

        std::random_device rd;
        std::mt19937 gen { rd() };
        std::uniform_real_distribution<> dis;
        std::vector<Agent *> world;
        std::vector<std::vector<Agent *>> grid;

	// Crear el grafo aleatorio con agentes
        if (randomg){

                //Crear agentes
                for (unsigned int i=0; i<(rows*cols) ; ++i){

                        Agent * new_agent;

                        // Decidir si ser o no ser cooperador
                        auto coop = dis(gen) < coop_fraction;

			// Asignar etiquetas
                        if (tag){
                                new_agent = new TagAgent(
                                                coop ? dis(gen) : 0,
                                                dis(gen),
                                                coop
                                                );
                        } else {
                                new_agent = new Agent(coop);
                        }
                        world.push_back(new_agent);
                }

                // Para la impresión del primer grafo (formato compatible con iGraph)
		if (f_graph){
                	std::cout << "g.add_edges([";
                }

		// Formación del grafo con  conexiones aleatorias.
                for (unsigned int w=0 ; w < ((con*rows*cols)/2) ; w++){
                        // Seleccionar aleatoriamente los agentes
                        std::uniform_int_distribution<> m(0,((rows*cols)-1));
                        unsigned int n = m(gen);
                        std::uniform_int_distribution<> o(0,((rows*cols)-1));
                        unsigned int p = o(gen);

                        if (n != p){
                                if(world[n]->vecinos.find(world[p]) == world[n]->vecinos.end()){
                                        world[n]->add_vecino(world[p]);
                                        world[p]->add_vecino(world[n]);

					// Impresión de los enlaces creados
					if(f_graph){
						std::cout << "(" << n << "," << p << "), ";
					}
                                }else{ w--; }

                        } else {
                                w--;
                        }

                }
		if(f_graph){
			std::cout << "])";
		}

	// Creación del grafo regular
        } else {

                // Crear agentes y establecer sus conexiones iniciales.

                // Primero, hay que crear la red
                for (unsigned int i = 0; i < rows; ++i) {
                        // Crear una fila vacía
                        auto row = std::vector<Agent *>();

                        // llenar la fila nueva
                        for (unsigned int j = 0; j < cols; ++j) {
                                // Crear un agente nuevo
                                Agent * new_agent;

                                // decidir si será un cooperador
                                auto coop = dis(gen) < coop_fraction;

				// Asignar la Etiqueta si es necesario
                                if (tag) {
                                        new_agent = new TagAgent(
                                                coop ? dis(gen) : 0,
                                                dis(gen),
                                                coop
                                                );
                                } else {
                                        new_agent = new Agent(coop);
                                }

                                // ponerlo al final de la fila que estamos construyendo
                                row.push_back(new_agent);

                                // y agregarlo tambiÃ©n a la lista general de agentes.
                                world.push_back(new_agent);
                        }

                        // y agregar la fila a la matriz.
                        grid.push_back(std::move(row));
                }

                // y ahora hay que establecer las conexiones.

		//  Para los vecindarios de von Neuman
                if (veci==4 || veci== 1 ){
                        for (unsigned int i = 0; i < rows; ++i) {
                                for (unsigned int j = 0; j < cols; ++j) {
                                        grid[i][j]->add_vecino(grid[i                    ][(cols + j - 1) % cols]); // Con el de la columna anterior
                                        grid[i][j]->add_vecino(grid[i                    ][(cols + j + 1) % cols]); // Con el de la columna siguiente
                                        grid[i][j]->add_vecino(grid[(rows + i - 1) % rows][j                    ]); // Con el de la fila anterior
                                        grid[i][j]->add_vecino(grid[(rows + i + 1) % rows][j                    ]); // Con el de la fila siguiente
                                }
                        }
                }

		// Para el vecindario de Moore
                if (veci==8){
                        for (unsigned int i = 0; i < rows; ++i) {
                                for (unsigned int j = 0; j < cols; ++j) {
                                        grid[i][j]->add_vecino(grid[i                    ][(cols + j - 1) % cols]); // Con el de la columna anterior
                                        grid[i][j]->add_vecino(grid[i                    ][(cols + j + 1) % cols]); // Con el de la columna siguiente
                                        grid[i][j]->add_vecino(grid[(rows + i - 1) % rows][j                    ]); // Con el de la fila anterior
                                        grid[i][j]->add_vecino(grid[(rows + i + 1) % rows][j                    ]); // Con el de la fila siguiente
                                        grid[i][j]->add_vecino(grid[(rows + i - 1) % rows][(cols + j - 1) % cols]); // Esquina de la colum y fila anterior
                                        grid[i][j]->add_vecino(grid[(rows + i - 1) % rows][(cols + j + 1) % cols]); // Esquina de la colum siguiente y fila anterior
                                        grid[i][j]->add_vecino(grid[(rows + i + 1) % rows][(cols + j - 1) % cols]); // Esquina de la colum anterior y fila siguiente
                                        grid[i][j]->add_vecino(grid[(rows + i + 1) % rows][(cols + j + 1) % cols]); // Esquina de la colum y fila siguientes
}
                        }
                }
        }

        // Para cada agente en el mundo, vamos a decirle cómo calcular su fitness y cómo actualizarse
        std::for_each(
                world.begin(),
                world.end(),
                [world,adaptive,toggle, tag, dif, &dis, &gen, coop_cost, coop_benefit, veci](Agent * a) {

                        // así que a a->eval_fitness le vamos a asignar nuestra nueva función de fitness:
                        a->eval_fitness = [tag, a, dif, &dis, &gen, coop_cost, coop_benefit, veci]() {
                                // El cálculo es incremental, así que comenzamos en cero, en cada iteración.
                                a->fitness = 0;

                                // Esta función toma a otro agente e incrementa el fitness del actual con el costo y el beneficio de esta corrida.
                                std::function<void(Agent *)> incr_fitness;

                                if (tag) {
                                        auto at = dynamic_cast<TagAgent *>(a);
                                        incr_fitness = [at, coop_cost, coop_benefit](Agent * v) {
                                                auto vt = dynamic_cast<TagAgent *>(v);
                                                if (at->coop && std::abs(at->tag - vt->tag) < at->tolerance) at->fitness -= coop_cost   ;
                                                if (vt->coop && std::abs(at->tag - vt->tag) < vt->tolerance) at->fitness += coop_benefit;
                                        };
                                } else {
                                        incr_fitness = [a, coop_cost, coop_benefit](Agent * v) {
                                                if (a->coop) a->fitness -= coop_cost   ;
                                                if (v->coop) a->fitness += coop_benefit;
                                        };
                                }

                                // Esa función hay que correrla con los vecinos directos
                                        if (veci==1){
                                                        //genenrar una posición aleatoria
                                                        std::uniform_int_distribution<> m(0,3);
                                                        unsigned int n = m(gen);

                                                //auto it = a->vecinos.begin();
                                                std::unordered_set<Agent *>::iterator it = a->vecinos.begin();
                                                std::advance(it, n);

                                                        // Se selecciona aleatoriamente un vecino y se aumenta el fitness
                                                        incr_fitness(*it);
                                        }

                                        else{
                                                std::for_each(
                                                a->vecinos.begin(),
                                                a->vecinos.end(),
                                                [&incr_fitness](Agent * v) {
                                                        // La interacción con los vecinos directos es incondicional.
                                                        incr_fitness(v);
                                                        }
                                                );
                                        }

                                // y si hay interacción con los segundos vecinos, también con los indirectos.
                               if (dif) {
                                        std::for_each(
                                                a->vecinos2.begin(),
                                                a->vecinos2.end(),
                                                [dif, &incr_fitness, &dis, &gen](Agent * v) {
                                                        // La interacción con los vecinos indirectos es probabilística.
                                                        if (dis(gen) < dif) incr_fitness(v);
                                                }
                                        );
                                }

                                // Y ya calculamos el fitness.
                                return a->fitness;
                        };



                        // y a a->eval_coop le vamos a asignar nuestra nueva funciÃ³n de actualización:

                        // En el caso de toggle (intermitente), la función de actualización es así:
                        if (toggle) {
                                a->eval_coop = [a]() {
                                        for (
                                                auto it = a->vecinos.begin();
                                                it != a->vecinos.end();
                                                ++it
                                        ) {
                                                // Si el fitness de algún vecino es mayor que el nuestro, cambiamos de comportamiento
                                                if ((*it)->fitness > a->fitness) {
                                                        a->new_coop = !a->coop;
                                                        return;
                                                }
                                        }
                                };
                        }

                        //Función de actualización...
                        else {
                                a->eval_coop = [world,adaptive,a, dif, &dis, &gen, tag]() {
                                        // Por ahora, creemos que nuestro fitness es el máximo de nosotros y nuestros vecinos.
                                        auto max_fitness_agent = a;

                                        // ¿Habrá un vecino con más fitness que yo?
                                        std::for_each(
                                                a->vecinos.begin(),
                                                a->vecinos.end(),
                                                [dif, &dis, &gen, tag, &max_fitness_agent](Agent * v) {
                                                        // La interacción con los vecinos
                                                       if(tag){
                                                               if (v->fitness > max_fitness_agent->fitness) {
                                                               		// Si el vecino tiene más fitness - Recordamos quién es.
                                                               		max_fitness_agent = v;
                                                               }

                                                       } else  {
                                                                if (v->fitness > max_fitness_agent ->fitness){
                                                                        max_fitness_agent = v;
                                                                }
                                                       }
                                                }
                                        );

                                        // Copiamos el comportamiento del vecino con más fitness; si no hay, copiamos el comportamiento que teníamos antes.
                                        a->new_coop = max_fitness_agent->coop;

                                        // Si el agente en realidad es un TagAgent (y el vecino también)
                                        auto at                  = dynamic_cast<TagAgent *>(a                );
                                        auto max_fitness_agent_t = dynamic_cast<TagAgent *>(max_fitness_agent);
                                        if (at && max_fitness_agent_t) {
                                                // Copiamos todo lo demÃ¡s que tenemos que copiar de Ã©l.
                                                at->new_tolerance = max_fitness_agent_t->tolerance;
                                                at->new_tag       = max_fitness_agent_t->tag      ;

                                                }

                                        // Si el agente es un Agent normal, y no un TagAgent
                                        else {
                                                // ¿Habrá un vecino con más fitness que nosotros?
                                                std::for_each(
                                                        a->vecinos.begin(),
                                                        a->vecinos.end(),
                                                        [&max_fitness_agent](Agent * v) {
                                                                if (v->fitness > max_fitness_agent->fitness) {
                                                                        // Sí hay!  Recordamos quién es.
                                                                        max_fitness_agent = v;
                                                                }
                                                        }
                                                );
                                                a->new_coop = max_fitness_agent->coop;
                                        }

					// Funciones de la red adaptativa
                                        if (adaptive){
                                                if (a != max_fitness_agent){
							// Si el agente con mayor fitness no soy yo y es no cooperador
                                                        if (!max_fitness_agent->coop){
                                                                a->vecinos.erase(max_fitness_agent);
                                                                max_fitness_agent->vecinos.erase(a);
                                                                bool swap = false;
                                                                std::uniform_int_distribution<> m(0,world.size()-1);

								// Cambiamos de vecino
								while(!swap){
                                                                        unsigned int n = m(gen);

                                                                        //auto it = a->vecinos.begin();
                                                                        if(world[n]!=a){
                                                                                a->add_vecino(world[n]);
                                                                                swap=true;
                                                                        }
                                                                }
                                                        }
                                                }

                                        }

                                };
                        }

                        // Si hay grupos expandidos, hay que calcular el conjunto de vecinos indirectos:
                        a->eval_vecinos2();
                }
        );



        // Esta función se encarga de toda la impresión de datos en la simulación.
        auto show_state = [
                &human_readable          ,
                &show_grid               ,
                &show_avg_fitness_coop   ,
                &show_avg_fitness_noncoop,
                &show_coop_fraction      ,
                &world                   ,
                &grid                    ,
                &rows                    ,
                &randomg                 ,
                &cols                    ,
                &frac
        ](unsigned int i, bool show_fitness) {
                unsigned int n_coops = 0;

                double pfnc = 0;
                double pfc  = 0;
                static unsigned int next_field_id = 0;

                if (human_readable && show_grid) std::cout << "begin grid " << i << "\n";


                if (randomg && show_coop_fraction) {
                        std::for_each(
                                        world.begin(),
                                        world.end(),
                                        [&n_coops, &pfc, &pfnc](Agent * a){
                                        (a->coop ? pfc : pfnc) += a->fitness;
                                        n_coops += a->coop;
                                        }
                                     );

                } else {std::for_each(
                        grid.begin(),
                        grid.end(),
                        [&human_readable, &show_grid, &n_coops, &pfc, &pfnc](std::vector<Agent *> & row) {
                                std::for_each(
                                        row.begin(),
                                        row.end(),
                                        [&human_readable, &show_grid, &n_coops, &pfc, &pfnc](Agent * a) {
                                                (a->coop ? pfc : pfnc) += a->fitness;
                                                n_coops += a->coop;
                                                if (human_readable && show_grid) {
                                                        std::cout << (a->coop ? COOP_AGENT_STRING : NOCOOP_AGENT_STRING);
                                                }
                                        }
                                );
                                if (human_readable && show_grid) std::cout << "\n";
                        }
                );
                }

                if (human_readable && show_grid) std::cout << "end grid " << i << std::endl;

                if (show_coop_fraction) {
                        static unsigned int k = next_field_id++;
                        std::cout
                                << (
                                        human_readable
                                        ? ("coop fraction " + std::to_string(i) + ": ")
                                        : (std::to_string(k) + ":")
                                )
                                << static_cast<double>(n_coops)/(rows*cols)
                                << std::endl
                        ;

                        frac = double (n_coops)/(rows*cols);
                }

                if (show_fitness) {
                        if (show_avg_fitness_coop) {
                                static unsigned int k = next_field_id++;
                                if (n_coops != 0) std::cout
                                        << (
                                                human_readable
                                                ? ("average cooperator fitness " + std::to_string(i) + ": ")
                                                : (std::to_string(k) + ":")
                                        )
                                        << pfc/n_coops
                                        << std::endl
                                ;
                        }

                        if (show_avg_fitness_noncoop) {
                                static unsigned int k = next_field_id++;
                                if (n_coops != world.size()) std::cout
                                        << (
                                                human_readable
                                                ? ("average noncooperator fitness " + std::to_string(i) + ": ")
                                                : (std::to_string(k) + ":")
                                        )
                                        << pfnc/(world.size() - n_coops)
                                        << std::endl
                                ;
                        }
                }
        };



        // Ciclo principal de la simulación:

        // Mostrar el estado inicial antes de iterar.
        show_state(0, false);

	   // Variables para permitir el cierre rápido de la simulación
        unsigned int finale = 0;
        double          pf1 = 0;

        for (unsigned int i = 0; i < iters; ++i) {
                // Se hacen tres pasadas por la lista de agentes: calcular fitness, evaluar contra los vecinos, y asignar el comportamiento para la siguiente iteración.

                std::for_each(world.begin(), world.end(), [](Agent * a) { a->eval_fitness(); });
                std::for_each(world.begin(), world.end(), [](Agent * a) { a->eval_coop()   ; });

                bool all_coop   = true;
                bool all_nocoop = true;
                std::for_each(
                                world.begin(),
                                world.end(),
                                [&all_coop, &all_nocoop](Agent * a) {
                                        a->coop = a->new_coop;

                                        auto at = dynamic_cast<TagAgent *>(a);
                                        if (at) {
                                                at->tag       = at->new_tag      ;
                                                at->tolerance = at->new_tolerance;
                                        }

                                        if (a->new_coop) all_nocoop = false;
                                        else             all_coop   = false;
                                }
                             );

                // Mostrar el nuevo estado.
                show_state(i + 1, true);

		// La simulación termina inmediatamente si queremos mostrar el grafo inicial aleatorio
		if (f_graph){
			break;
		}

		//condiciones para termiar la simulación:
		// 20 iteraciónes con la misma proporción, todos coop o todos no cooperadores.
                if (pf1==frac || all_coop || all_nocoop) {
                        if (++finale > 20){
				break;
                        }
                } else finale = 0;
                pf1 = frac;
        }

	// Cálculo de las etiquetas
	if (tag_c){
		std::unordered_map<double,int> tagcount;
		double tolsum = 0;
		int cont=0;
		std::for_each(
			world.begin(),
			world.end(),
			[&tagcount, &tolsum, &cont](Agent * a){
				auto at = dynamic_cast<TagAgent *>(a);
				cont++;
				if(at){

					// Suma de todas las tolerancias
					tolsum = tolsum + (at->tolerance);
					// Conteo de las etiquetas
					if (tagcount.find(at->tag)==tagcount.end()){
						tagcount[at->tag]=1;
					} else {
						tagcount[at->tag]= tagcount[at->tag]+1;
					}
				}
			}
		);

		// Impresión de los datos calculados
		std::for_each(
			tagcount.begin(),
			tagcount.end(),
			[rows, cols](std::pair<double,int> p){
				if ( p.second > (rows*cols*0.01) ){
					std::cout<<p.first<<"    \t ->  "<<p.second<<std::endl;
				}
			}
		);

		std::cout<<"Average Tolerance = "<<(tolsum/(rows*cols))<<std::endl;

	}
}
