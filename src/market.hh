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

        std::vector<Interval *> cheapest_interals_in_range(size_t uid, size_t start, size_t end, size_t num_intervals);

    public:
        void add_interval(size_t uid, size_t start, size_t end, double offer);
        double cost_for_intervals(size_t uid, size_t start, size_t end, size_t num_intervals);
        double buy_intervals(size_t uid, size_t start, size_t end, size_t num_intervals, double max_payment);

        size_t version() const { return version_; };
        bool empty();
        void advance_time();
        void print_intervals() const;
};

#endif /* MARKET_HH */
