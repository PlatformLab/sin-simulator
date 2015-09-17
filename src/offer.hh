/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OFFER_HH
#define OFFER_HH

#include <vector>
#include "interval.hh"

struct Offer {
    size_t seller_uid;
    Interval interval;
    double cost;
    std::pair<bool, Interval> exchange_for_interval;

    bool operator==( const Offer& other ) const {
        return seller_uid == other.seller_uid and interval == other.interval and cost == other.cost;
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
                    ^ ( hash<double>()(o.cost) + 2 );
            }
        };
}

#endif /* OFFER_HH */
