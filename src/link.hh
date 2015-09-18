/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef LINK_HH
#define LINK_HH

#include <vector>
#include "opportunity.hh"

struct Link {
    const size_t uid;
    const size_t start;
    const size_t num_opportunities;
    const size_t propagation_time;

    std::vector<Opportunity> get_opportunities() const
    {
        std::vector<Opportunity> toRet;
        for ( size_t i = 0; i < num_opportunities; i++ ) {
            toRet.push_back( { { i, i+propagation_time }, 1, uid } ); // 1 packet opportunity
        }
        return toRet;
    }
};

#endif /* LINK_HH */
