#include <unordered_set>

#include "TagAgent.hh"

TagAgent::TagAgent(
        double p_tolerance,
        double p_tag,
        bool p_coop,
        std::unordered_set<Agent *> p_vecinos
):
        Agent::Agent(
                p_coop,
                p_vecinos
        ),
        tolerance(p_tolerance),
        tag      (p_tag      )
{}
