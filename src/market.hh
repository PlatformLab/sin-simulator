/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef MARKET_HH
#define MARKET_HH

#include <iostream>
#include <deque>
#include <list>
#include <vector>
#include <cassert>
#include <memory>
#include <functional>
#include <algorithm>

#include "pretty_print.hh"
#include "interval.hh"

struct MoneyExchanged {
    size_t to;
    size_t from;
    double amount;
};

class Market {
    private:
        size_t time_;
        size_t version_ = 0;
        std::vector<Interval> intervals_{};
        std::vector<MoneyExchanged> transactions_{};

        std::vector<Interval *> cheapest_interals_in_range( size_t uid, size_t start, size_t end, size_t num_intervals );

    public:
        void add_interval( Interval interval );
        double cost_for_intervals( size_t uid, size_t start, size_t end, size_t num_intervals );
        double buy_intervals( size_t uid, size_t start, size_t end, size_t num_intervals, double max_payment );

        size_t version() const { return version_; };
        size_t time() const { return time_; };
        std::vector<Interval> intervals() const { return intervals_; };
        bool empty();
        void advance_time();
};

#endif /* MARKET_HH */
