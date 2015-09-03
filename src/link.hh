/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef LINK_HH
#define LINK_HH

#include <vector>
#include "interval.hh"

struct Link {
    const size_t uid;
    const size_t start;
    const size_t num_intervals;
    const size_t propagation_time;
    const double default_interval_cost;

    std::vector<Interval> get_intervals() const
    {
        std::vector<Interval> toRet;
        for ( int i = 0; i < num_intervals; i++ ) {
            toRet.push_back( { uid, i, i+propagation_time, default_interval_cost } );
        }
        return toRet;
    }
};

#endif /* LINK_HH */
