/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "pretty_print.hh"

std::string uid_to_string( const size_t uid )
{
    if ( uid ) {
        return std::string( 1, 'A' + uid - 1 );
    } else {
        return std::string("ccast");
    }
}

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
        cost_matches = abs( s1.best_offer().cost - s2->best_offer().cost ) < 1e-9;
    }
    return owner_matches and cost_matches;
}
