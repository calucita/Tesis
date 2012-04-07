#include <algorithm>
#include <functional>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>
#include <sysexits.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

const int rows = 100;
const int cols = 100;

unsigned int agent_next_id = 0;

class Agent {
        public:
                bool coop;
                std::unordered_set<Agent *> vecinos;

                std::function<double(Agent *)> fitness_func;
                std::function<void  (void   )> update;
                const int id;

                double fitness;

                Agent(
                        bool p_coop,
                        std::unordered_set<Agent *> p_vecinos = std::unordered_set<Agent *>()
                ):
                        coop        (p_coop         ),
                        vecinos     (p_vecinos      ),
                        fitness_func(nullptr        ),
                        update      (nullptr        ),
                        id          (agent_next_id++)
                {}

                Agent & add_vecino(Agent * a) {
                        if (vecinos.find(a) == vecinos.end()) {
                                vecinos.insert(a);
                                if (a->vecinos.find(this) == a->vecinos.end()) {
                                        a->vecinos.insert(this);
                                }
                        }
                        return *this;
                }

                double eval_fitness() {
                        // Esto tira std::bad_function_call si no le
                        // definiste el fitness_func (y se quedó en el
                        // nullptr que le asigna el constructor (arriba))
                        fitness_func(this);
                        return fitness;
                }

                friend std::ostream & operator << (std::ostream & out, const Agent & a) {
                        return
                                out
                                << "Agente "
                                << a.id
                                << " ("
                                << (a.coop ? "coopera" : "no coopera")
                                << ") ["
                                << std::accumulate(
                                        a.vecinos.begin(),
                                        a.vecinos.end(),
                                        std::string(),
                                        [](std::string acc, Agent * v) {
                                                return
                                                        acc
                                                        + " "
                                                        + std::to_string(v->id);
                                        }
                                )
                                << " ] ("
                                << a.vecinos.size()
                                << " vecinos)"
                                ;
                }
};



int main(int argc, char * argv[]) {
        double coop_fraction;
        double coop_cost;
        double coop_benefit;
        unsigned int iters;

        try {
                po::options_description desc("basico");
                desc.add_options()
                        ("help", "produce help message")
                        ("coop_fraction", po::value<double>(), "set initial cooperation fraction")
                        ("coop_cost"    , po::value<double>(), "set cooperation cost"            )
                        ("coop_benefit" , po::value<double>(), "set cooperation benefit"         )
                        ("iters"        , po::value<int   >(), "set iteration count"             )
                ;

                po::variables_map vm;
                po::store(po::parse_command_line(argc, argv, desc), vm);
                po::notify(vm);

                if (vm.count("help")) {
                        std::cout << desc << std::endl;
                        std::exit(EX_OK);
                }

                coop_cost     = vm.count("coop_cost"    ) ? vm["coop_cost"    ].as<double>() : 1   ;
                coop_benefit  = vm.count("coop_benefit" ) ? vm["coop_benefit" ].as<double>() : 10  ;
                coop_fraction = vm.count("coop_fraction") ? vm["coop_fraction"].as<double>() :  0.5;
                iters         = vm.count("iters"        ) ? vm["iters"        ].as<double>() : 50  ;
        } catch(std::exception & e) {
                std::cerr << "error: " << e.what() << std::endl;
                std::exit(EX_USAGE);
        }

        std::vector<std::vector<Agent *>> grid;
        std::vector<Agent *> world;

        // Crear el generador de randoms.
        std::random_device rd;
        std::mt19937 gen { rd() };
        std::uniform_real_distribution<> dis;

        for (int i = 0; i < rows; ++i) {
                auto row = std::vector<Agent *>();
                for (int j = 0; j < cols; ++j) {
                        auto new_agent = new Agent(dis(gen) < coop_fraction);
                        row.push_back(new_agent);
                        world.push_back(new_agent);
                }
                grid.push_back(std::move(row));
        }

        for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                        grid[i][j]->add_vecino(grid[i                    ][(cols + j - 1) % cols]);
                        grid[i][j]->add_vecino(grid[i                    ][(cols + j + 1) % cols]);
                        grid[i][j]->add_vecino(grid[(rows + i - 1) % rows][j                    ]);
                        grid[i][j]->add_vecino(grid[(rows + i + 1) % rows][j                    ]);
                }
        }

        /* Para cada agente en el mundo, vamos a decirle cómo calcular
         * su fitness y cómo actualizarse…
         */
        std::for_each(
                world.begin(),
                world.end(),
                [coop_cost, coop_benefit](Agent * a) {
                        /* …así que a a->fitness_func le vamos a
                         * asignar nuestra nueva función de fitness:
                         */
                        a->fitness_func = [coop_cost, coop_benefit](Agent * af) {
                                af->fitness = 0;

                                std::for_each(
                                        af->vecinos.begin(),
                                        af->vecinos.end(),
                                        [coop_cost, coop_benefit, &af](Agent * v) {
                                                if (af->coop) af->fitness -= coop_cost   ;
                                                if (v ->coop) af->fitness += coop_benefit;
                                        }
                                );

                                return af->fitness;
                        };

                        /* …y a a->update le vamos a asignar nuestra
                         * nueva función de fritanga:
                         */
                        a->update = [&a]() {
#if TOGGLE_IF_ENVIDIA
                                for (
                                        auto it = a->vecinos.begin();
                                        it != a->vecinos.end();
                                        ++it
                                ) {
                                        if ((*it)->fitness > a->fitness) {
                                                a->coop ^= 1;
                                                return;
                                        }
                                }
#endif

                                auto max_fitness      = a->fitness;
                                auto max_fitness_coop = a->coop   ;
                                // ME ME ME ME MEEEEEEEEEEEEEEEEEEEE

                                std::for_each(
                                        a->vecinos.begin(),
                                        a->vecinos.end(),
                                        [&max_fitness, &max_fitness_coop](Agent * v) {
                                                if (v->fitness > max_fitness) {
                                                        max_fitness      = v->fitness;
                                                        max_fitness_coop = v->coop   ;
                                                }
                                        }
                                );

                                a->coop = max_fitness_coop;
                        };
                }
        );

        auto show_state = [&world, &grid](unsigned int i, bool show_fitness) {
                unsigned int n_coops = 0;

                double pfc  = 0;
                double pfnc = 0;

                std::cout
                        << "Iteración " << i << ":\n"
                        << std::accumulate(
                                grid.begin(),
                                grid.end(),
                                std::string(),
                                [&n_coops, &pfc, &pfnc](std::string acc, std::vector<Agent *> row) {
                                        return
                                                acc
                                                + std::accumulate(
                                                        row.begin(),
                                                        row.end(),
                                                        std::string(),
                                                        [&n_coops, &pfc, &pfnc](std::string acc, Agent * a) {
                                                                (a->coop ? pfc : pfnc) += a->fitness;
                                                                n_coops += a->coop;
                                                                return acc + (a->coop ? u8"█" : u8"░");
                                                        }
                                                )
                                                + "\n";
                                }
                        );

                std::cout << n_coops << " cooperadores" << "\n";

                if (show_fitness) std::cout
                        << "Iteración " << i << "[fitness de cooperadores] = " << pfc /n_coops                  << "\n"
                        << "Iteración " << i << "[fitness de traidores   ] = " << pfnc/(world.size() - n_coops) << "\n";

                std::cout << std::endl;
        };

        show_state(0, false);
        for (int i = 0; i < iters; ++i) {
                std::for_each(
                        world.begin(),
                        world.end(),
                        [](Agent * a) { a->eval_fitness(); }
                );

                std::for_each(
                        world.begin(),
                        world.end(),
                        [](Agent * a) { a->update(); }
                );

                show_state(i + 1, true);
        }
}
