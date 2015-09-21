/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OFFER_HH
#define OFFER_HH

#include <vector>
#include "interval.hh"

struct Offer {
    size_t seller_uid;
    Opportunity opportunity;
    double cost;
    std::vector<std::pair<Opportunity, double>> opportunities_in_exchange; /* TODO comment */

    bool operator==( const Offer& other ) const {
        return seller_uid == other.seller_uid and opportunity == other.opportunity and cost == other.cost;// and exchange_for_interval == other.exchange_for_interval;
    }
};

namespace std {
    template <>
        struct hash<Offer>
        {
            std::size_t operator()(const Offer& o ) const
            {
                return ( hash<size_t>()( o.seller_uid ) )
                    ^ ( hash<Opportunity>()( o.opportunity ) + 1 )
                    ^ ( hash<double>()( o.cost ) + 2 );
            }
        };
}

#endif /* OFFER_HH */
