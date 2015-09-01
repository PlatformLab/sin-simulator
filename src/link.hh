/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef LINK_HH
#define LINK_HH

struct Link {
    const size_t uid;
    const size_t num_intervals;
    const size_t propagation_time;
    const double default_interval_cost;
};

#endif /* LINK_HH */
