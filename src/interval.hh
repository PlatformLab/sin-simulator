/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef INTERVAL_HH
#define INTERVAL_HH

struct Interval {
    size_t owner;
    size_t start;
    size_t end;
    double cost;
};

#endif /* INTERVAL_HH */
