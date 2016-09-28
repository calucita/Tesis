#ifndef TAGAGENT_HH
#define TAGAGENT_HH

#include <unordered_set>

#include "Agent.hh"

class TagAgent : public virtual Agent {
        public:
                double tolerance;
                double tag;
                double new_tolerance;
                double new_tag;

                TagAgent(
                        double p_tolerance,
                        double p_tag,
                        bool p_coop,
                        std::unordered_set<Agent *> p_vecinos = std::unordered_set<Agent *>()
                );
};

#endif
