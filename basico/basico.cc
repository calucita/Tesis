#include <algorithm>
#include <functional>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>



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
        (void)argc;
        (void)argv;

        std::vector<Agent *> world;

        // Crear el generador de randoms.
        std::random_device rd;
        std::mt19937 gen { rd() };
        std::uniform_real_distribution<> dis;

        std::generate_n(
                std::back_inserter(world),
                100,
                [&]() { return new Agent(dis(gen) > 0.2); }
        );

        Agent * anterior = nullptr;
        std::for_each(
                world.begin(),
                world.end(),
                [&anterior](Agent * a) { if (anterior) a->add_vecino(anterior); anterior = a; }
        );
        world[0]->add_vecino(anterior);

        /* Para cada agente en el mundo, vamos a decirle cómo calcular
         * su fitness y cómo actualizarse…
         */
        std::for_each(
                world.begin(),
                world.end(),
                [](Agent * a) {
                        /* …así que a a->fitness_func le vamos a
                         * asignar nuestra nueva función de fitness:
                         */
                        a->fitness_func = [](Agent * af) {
                                af->fitness = 0;

                                std::for_each(
                                        af->vecinos.begin(),
                                        af->vecinos.end(),
                                        [&af](Agent * v) {
                                                if (af->coop) af->fitness -=  1;
                                                if (v ->coop) af->fitness += 10;
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

        auto show_state = [&world](unsigned int i, bool show_fitness) {
                unsigned int n_coops = 0;

                double pfc  = 0;
                double pfnc = 0;

                std::cout
                        << "Iteración " << i << ":\t"
                        << std::accumulate(
                                world.begin(),
                                world.end(),
                                std::string(),
                                [&n_coops, &pfc, &pfnc](std::string acc, Agent * a) {
                                        (a->coop ? pfc : pfnc) += a->fitness;
                                        n_coops += a->coop;
                                        return acc + (a->coop ? "1" : "0");
                                }
                        );

                std::cout << " ("
                        << n_coops
                        << " cooperadores)";

                if (show_fitness) std::cout
                        << " "
                        << pfc / n_coops
                        << "/"
                        << pfnc / (world.size() - n_coops);

                std::cout << std::endl;
        };

        show_state(0, false);
        for (int i = 0; i < 30; ++i) {
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
