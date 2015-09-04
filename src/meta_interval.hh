/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef META_INTERVAL_HH
#define META_INTERVAL_HH

#include <vector>

struct MetaInterval {
    size_t owner;
    size_t start;
    size_t end;
    std::vector<Interval *> intervals;
    std::vector<std::pair<size_t, double>> offers;
};

#endif /* META_INTERVAL_HH */
