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
    std::vector<Opportunity> backing_opportunities;

    /*
    bool operator==(const Offer& other) {
        return seller_uid == other.seller_uid and interval == other.interval and num_packets == other.num_packets and cost == other.cost and backing_opportunities == other.backing_opportunities;
    }
    */
};

#endif /* OFFER_HH */
