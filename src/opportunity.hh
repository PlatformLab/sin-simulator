/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OPPORTUNITY_HH
#define OPPORTUNITY_HH

#include "interval.hh"

struct Opportunity {
    size_t provider; /* uid of who will actually carry the packet */
    size_t owner; /*  uid of who is sending their packet for this opportunity */
    Interval interval;

    /*
    bool operator==(const Opportunity &other) {
        return owner == other.owner and interval == other.interval;
    }
    */
};


#endif /* OPPORTUNITY_HH */
