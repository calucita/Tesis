Anexo 1.3 Agent.hh

#ifndef AGENT_HH
#define AGENT_HH
#include <functional>
#include <iostream>
#include <unordered_set>

extern unsigned int agent_next_id;
class Agent {
        public:
                bool coop;
                std::unordered_set<Agent *> vecinos;
                std::unordered_set<Agent *> vecinos2;

                std::function<void(void)> eval_fitness;
                std::function<void(void)> eval_coop   ;
                const int id;

                double fitness;
                bool new_coop;
                Agent* old_copy;

                Agent(
                        bool p_coop,
                        std::unordered_set<Agent *> p_vecinos = std::unordered_set<Agent *>()
                );

                virtual ~Agent();

                Agent & add_vecino(Agent * a);

                void eval_vecinos2();

                friend std::ostream & operator << (std::ostream & out, const Agent & a);
};

#endif
