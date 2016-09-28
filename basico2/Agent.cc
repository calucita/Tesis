#include <algorithm>
#include <iostream>
#include <unordered_set>

#include "Agent.hh"



unsigned int agent_next_id = 1;



Agent::Agent(
        bool p_coop,
        std::unordered_set<Agent *> p_vecinos
):
        coop        (p_coop         ),
        vecinos     (p_vecinos      ),
        eval_fitness(nullptr        ),
        eval_coop   (nullptr        ),
        id          (agent_next_id++)
{}



Agent::~Agent() {}



Agent & Agent::add_vecino(Agent * a) {
        if (vecinos.find(a) == vecinos.end()) {
                vecinos.insert(a);
                if (a->vecinos.find(this) == a->vecinos.end()) {
                        a->vecinos.insert(this);
                }
        }
        return *this;
}



void Agent::eval_vecinos2() {
        // Para cada vecino…
        std::for_each(
                vecinos.begin(),
                vecinos.end(),
                [this](Agent * v) {
                        // …buscamos todos los vecinos del vecino…
                        std::for_each(
                                v->vecinos.begin(),
                                v->vecinos.end(),
                                [this](Agent * v2) {
                                        // …y si no es ni el mismo agente original, ni un vecino directo, entonces es indirecto:
                                        if (v2 != this && vecinos.find(v2) == vecinos.end()) vecinos2.insert(v2);
                                }
                        );
                }
        );
}



std::ostream & operator << (std::ostream & out, const Agent & a) {
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
                                        + std::to_string(v->id)
                                ;
                        }
                )
                << " ] ("
                << a.vecinos.size()
                << " vecinos)"
        ;
}
