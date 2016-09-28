#include <algorithm>
#include <functional>
#include <iostream>
#include <random>
#include <unordered_set>
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

        // Opciones de simulación:
        bool         toggle                  ;
        bool         tag                     ;
        bool         tagmut                  ;
        double       coop_fraction           ;
        double       coop_cost               ;
        double       coop_benefit            ;
        unsigned int iters                   ;
        unsigned int rows                    ;
        unsigned int cols                    ;
        double       dif                     ;

        try {
                // Declarar todas las opciones, sus tipos, su texto de ayuda, etc…
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
                ;
                desc.add(output);

                po::options_description sim("Simulation control");
                sim.add_options()
                        ("toggle,t"                , po::bool_switch        (&toggle                  )->default_value(false), "operate in toggle mode"                                )
                        ("tag"                     , po::bool_switch        (&tag                     )->default_value(false), "Use Tag selection"                                     )
                        ("tagmut"                  , po::bool_switch        (&tagmut                  )->default_value(false), "Use Tag selection with mutations"                      )
                        ("coop-fraction,f"         , po::value<double      >(&coop_fraction           )->default_value(  0.5), "set initial cooperation fraction"                      )
                        ("coop-cost,c"             , po::value<double      >(&coop_cost               )->default_value(  1  ), "set cooperation cost"                                  )
                        ("coop-benefit,b"          , po::value<double      >(&coop_benefit            )->default_value( 10  ), "set cooperation benefit"                               )
                        ("iters,i"                 , po::value<unsigned int>(&iters                   )->default_value( 50  ), "set iteration count"                                   )
                        ("rows,y"                  , po::value<unsigned int>(&rows                    )->default_value(100  ), "set grid row count"                                    )
                        ("cols,x"                  , po::value<unsigned int>(&cols                    )->default_value(100  ), "set grid column count"                                 )
                        ("dif,d"                   , po::value<double      >(&dif                     )->default_value(  0  ), "set diffusion probability"                             )
                ;
                desc.add(sim);

                // …y luego ejecutar el procesamiento de esas opciones:
                po::variables_map vm;
                po::store(po::parse_command_line(argc, argv, desc), vm);
                po::notify(vm);

                if (vm.count("help")) {
                        std::cout << desc << std::endl;
                        std::exit(EX_OK);
                }

                // La opción “verbose” activa todas las opciones de impresión.
                if (vm.count("verbose")) {
                        show_grid                = true;
                        show_avg_fitness_coop    = true;
                        show_avg_fitness_noncoop = true;
                        show_coop_fraction       = true;
                }

                // Alertar al usuario de que la corrida que mandó a hacer no imprime nada (igual puede ser útil para medir tiempos, por ejemplo).
                if (
                        !show_grid                &&
                        !show_avg_fitness_coop    &&
                        !show_avg_fitness_noncoop &&
                        !show_coop_fraction       &&
                        true
                ) {
                        std::cerr << "warning: no output options active" << std::endl;
                }
        } catch(std::exception & e) {
                // Acá se viene a parar cuando se usan opciones inválidas:
                std::cerr << "error: " << e.what() << std::endl;
                std::exit(EX_USAGE);
        }



        // Crear el generador de números aleatorios.

        std::random_device rd;
        std::mt19937 gen { rd() };
        std::uniform_real_distribution<> dis;



        // Crear agentes y establecer sus conexiones iniciales.
        std::vector<std::vector<Agent *>> grid;
        std::vector<Agent *> world;

        // Primero, hay que crear la red…
        for (unsigned int i = 0; i < rows; ++i) {
                // Crear una fila vacía…
                auto row = std::vector<Agent *>();

                // …llenar la fila nueva…
                for (unsigned int j = 0; j < cols; ++j) {
                        // Crear un agente nuevo…
                        Agent * new_agent;

                        // …decidir si será un cooperador…
                        auto coop = dis(gen) < coop_fraction;

                        if (tag) {
                                new_agent = new TagAgent(
                                        coop ? dis(gen) : 0,
                                        dis(gen),
                                        coop
                                );
                        } else {
                                new_agent = new Agent(coop);
                        }

                        // …ponerlo al final de la fila que estamos construyendo…
                        row.push_back(new_agent);

                        // …y agregarlo también a la lista general de agentes.
                        world.push_back(new_agent);
                }

                // …y agregar la fila a la matriz.
                grid.push_back(std::move(row));
        }

        // …y ahora hay que establecer las conexiones.
        for (unsigned int i = 0; i < rows; ++i) {
                for (unsigned int j = 0; j < cols; ++j) {
                        grid[i][j]->add_vecino(grid[i                    ][(cols + j - 1) % cols]); // Con el de la columna anterior
                        grid[i][j]->add_vecino(grid[i                    ][(cols + j + 1) % cols]); // Con el de la columna siguiente
                        grid[i][j]->add_vecino(grid[(rows + i - 1) % rows][j                    ]); // Con el de la fila anterior
                        grid[i][j]->add_vecino(grid[(rows + i + 1) % rows][j                    ]); // Con el de la fila siguiente
                }
        }



        // Para cada agente en el mundo, vamos a decirle cómo calcular su fitness y cómo actualizarse…
        std::for_each(
                world.begin(),
                world.end(),
                [toggle, tag, dif, &dis, &gen, coop_cost, coop_benefit, tagmut](Agent * a) {
                        // …así que a a->eval_fitness le vamos a asignar nuestra nueva función de fitness:
                        a->eval_fitness = [tag, a, dif, &dis, &gen, coop_cost, coop_benefit]() {
                                // El cálculo es incremental, así que comenzamos en cero:
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

                                // Esa función hay que correrla con los vecinos directos…
                                std::for_each(
                                        a->vecinos.begin(),
                                        a->vecinos.end(),
                                        [&incr_fitness](Agent * v) {
                                                // La interacción con los vecinos directos es incondicional.
                                                incr_fitness(v);
                                        }
                                );

                                // …y si hay difusión, también con los indirectos.
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



                        // …y a a->eval_coop le vamos a asignar nuestra nueva función de actualización:

                        // En el caso de toggle, la función de actualización es así:
                        if (toggle) {
                                a->eval_coop = [a]() {
                                        for (
                                                auto it = a->vecinos.begin();
                                                it != a->vecinos.end();
                                                ++it
                                        ) {
                                                // Si el fitness de algún vecino es mayor que el nuestro…
                                                if ((*it)->fitness > a->fitness) {
                                                        a->new_coop = !a->coop;
                                                        return;
                                                }
                                        }
                                };
                        }

                        //Función de actualización...
                        else {
                                a->eval_coop = [a, dif, &dis, &gen, tag, tagmut]() {
                                        // Por ahora, creemos que nuestro fitness es el máximo de nosotros y nuestros vecinos.
                                        auto max_fitness_agent = a;

                                        // ¿Habrá un vecino con más fitness que nosotros?
                                        std::for_each(
                                                a->vecinos.begin(),
                                                a->vecinos.end(),
                                                [dif, &dis, &gen, tag, &max_fitness_agent, tagmut](Agent * v) {
                                                        // La interacción con los vecinos es probabilística solo en el caso de los TAGs.(not!)
                                                       if(tag){
                                                                // if (dis(gen) < 0.25) {
                                                                        if (v->fitness > max_fitness_agent->fitness) {
                                                                                // ¡El vecino tiene más fitness!  Recordamos quién es.
                                                                                max_fitness_agent = v;
                                                                         }
                                                                // }
                                                       }
                                                       else  {
                                                                if (v->fitness > max_fitness_agent ->fitness){
                                                                        max_fitness_agent = v;
                                                                }
                                                       }
                                                }
                                        );

                                        // Copiamos el comportamiento del vecino con más fitness; si no hay, copiamos el comportamiento que teníamos antes.
                                        a->new_coop = max_fitness_agent->coop;

                                        // Si el agente “a” en realidad es un TagAgent (y el vecino también)…
                                        auto at                  = dynamic_cast<TagAgent *>(a                );
                                        auto max_fitness_agent_t = dynamic_cast<TagAgent *>(max_fitness_agent);
                                        if (at && max_fitness_agent_t) {
                                                // Copiamos todo lo demás que tenemos que copiar de él.
                                                at->new_tolerance = max_fitness_agent_t->tolerance;
                                                at->new_tag       = max_fitness_agent_t->tag      ;

                                                        if (tagmut){
                                                        //Mutaciones con Tag
                                                        at->tolerance    +=(dis(gen)/10)        ;
                                                        at->new_tag      +=(dis(gen)/10)        ;
                                                        }
                                                }

                                        // Si el agente “a” es un Agent normal, y no un TagAgent…
                                        else {
                                                // ¿Habrá un vecino con más fitness que nosotros?
                                                std::for_each(
                                                        a->vecinos.begin(),
                                                        a->vecinos.end(),
                                                        [&max_fitness_agent](Agent * v) {
                                                                if (v->fitness > max_fitness_agent->fitness) {
                                                                        // ¡Sí hay!  Recordamos quién es.
                                                                        max_fitness_agent = v;
                                                                }
                                                        }
                                                );
                                                a->new_coop = max_fitness_agent->coop;
                                        }
                                };
                        }

                        // Si hay difusión, hay que calcular el conjunto de vecinos indirectos:
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
                &cols
        ](unsigned int i, bool show_fitness) {
                unsigned int n_coops = 0;

                double pfc  = 0;
                double pfnc = 0;

                static unsigned int next_field_id = 0;

                if (human_readable && show_grid) std::cout << "begin grid " << i << "\n";
                std::for_each(
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

        unsigned int finale = 0;

        for (unsigned int i = 0; i < iters; ++i) {
                // Se hacen tres pasadas por la lista de agentes: calcular fitness, calcular siguiente iteración, y asignar siguiente iteración.
                // TODO: Si se usa una especie de double buffering esto sería quizás más eficiente… se ahorra una pasada (pero se hacen más indirecciones).

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

                if (all_coop || all_nocoop) {
                        if (++finale > 20){
                                break;
                        }
                }
                else finale = 0;
        }

}

