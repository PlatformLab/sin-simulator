/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OFFER_HH
#define OFFER_HH

#include <vector>
#include "interval.hh"

struct Offer {
    size_t seller_uid;
    Interval interval;
    double cost;
    std::pair<bool, Interval> exchange_for_interval; /* if first bool true, this offer's cost includes trading for the following interval */

    bool operator==( const Offer& other ) const {
        return seller_uid == other.seller_uid and interval == other.interval and cost == other.cost and exchange_for_interval == other.exchange_for_interval;
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

                return ( hash<size_t>()( o.seller_uid ) )
                    ^ ( hash<Interval>()( o.interval ) + 1 )
                    ^ ( hash<double>()( o.cost) + 2 )
                    ^ ( hash<bool>()( o.exchange_for_interval.first ) + 3 )
                    ^ ( hash<Interval>()( o.exchange_for_interval.second ) + 4 );
            }
        };
}

#endif /* OFFER_HH */
