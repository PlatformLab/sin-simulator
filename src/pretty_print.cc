/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "pretty_print.hh"

void print_cost( const PacketSent * ) { }

void print_cost( const SingleSlot *p )
{
    std::cout << " $";
    if ( p->has_offers() ) {
        std::cout << p->best_offer().cost;
    } else {
        std::cout << "null";
    }
}

bool matches( const PacketSent &s1, const PacketSent *s2 )
{
    return s1.owner == s2->owner;
}

bool matches( const SingleSlot &s1, const SingleSlot *s2 )
{
    bool owner_matches = s1.owner == s2->owner;
    bool cost_matches = s1.has_offers() == s2->has_offers();
    if ( cost_matches and s1.has_offers() ) {
        cost_matches = s1.best_offer().cost == s2->best_offer().cost;
    }
    return owner_matches and cost_matches;
}
