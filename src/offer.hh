/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OFFER_HH
#define OFFER_HH

#include <vector>
#include "interval.hh"
#include "opportunity.hh"

struct Offer {
    size_t seller_uid;
    Interval interval;
    size_t num_packets;
    double cost;
    std::vector<std::shared_ptr<Opportunity>> backing_opportunities;

    bool operator==( const Offer& other ) const {
        return seller_uid == other.seller_uid and interval == other.interval and num_packets == other.num_packets and cost == other.cost and backing_opportunities == other.backing_opportunities;
    }
};

namespace std {
    template <>
        struct hash<Offer>
        {
            std::size_t operator()(const Offer& o ) const
            {
                using std::size_t;
                using std::hash;

                return ( hash<size_t>()(o.seller_uid) )
                ^ ( hash<Interval>()(o.interval) + 1 )
                ^ ( hash<size_t>()(o.num_packets) + 2 )
                ^ ( hash<double>()(o.cost) + 3 );
                // ^ ( hash<double>(o.backing_opportunities()) + 4 ); TODO hash opportunties
            }
        };
}

#endif /* OFFER_HH */
