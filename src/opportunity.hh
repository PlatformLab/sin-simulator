/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OPPORTUNITY_HH
#define OPPORTUNITY_HH

#include "interval.hh"

struct Opportunity {
    Interval interval;
    size_t num_packets;
    size_t provider_uid; /* who is actually sending packet */

    bool operator==( const Opportunity& other ) const {
        return interval == other.interval and num_packets == other.num_packets and provider_uid == other.provider_uid;
    }
};

namespace std {
    template <>
        struct hash<Opportunity>
        {
            std::size_t operator()( const Opportunity& o ) const
            {
                return ( hash<Interval>()( o.interval) )
                    ^ ( hash<size_t>()( o.num_packets ) + 1 )
                    ^ ( hash<size_t>()( o.provider_uid ) + 2 );
            }
        };
}

#endif /* OPPORTUNITY_HH */
