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

#include "interval.hh"
#include "transaction.hh"
#include "meta_interval.hh"
#include "pretty_print.hh"

class Market {
    private:
        size_t time_ = 0;
        size_t version_ = 0;
        std::vector<Interval> intervals_ { };
        std::vector<MetaInterval> meta_intervals_ { };
        std::vector<Transaction> transactions_ { };
        const bool verbose_;

        std::vector<Interval *> cheapest_intervals_in_range( size_t uid, size_t start, size_t end, size_t num_intervals );
        std::tuple<double, std::vector<MetaInterval>::iterator, std::pair<size_t, double>, std::vector<Interval *>, std::vector<Interval *>> best_meta_interval( size_t uid, size_t start, size_t end, size_t num_intervals );

    public:
        Market( const bool verbose )
            : verbose_( verbose )
        { }
        void add_interval( Interval interval );
        double cost_for_intervals( size_t uid, size_t start, size_t end, size_t num_intervals );
        double buy_intervals( size_t uid, size_t start, size_t end, size_t num_intervals,
                double max_payment, std::vector<std::pair<size_t, double>> offers );

        size_t version() const { return version_; };
        size_t verbose() const { return verbose_; };
        size_t time() const { return time_; };

        size_t intervals_owned_by_user( const size_t uid ) const;

        std::vector<Interval> intervals() const { return intervals_; };
        std::vector<MetaInterval> meta_intervals() const { return meta_intervals_; };
        std::vector<Transaction> transactions() const { return transactions_; };
        bool empty();
        void advance_time();
};

#endif /* MARKET_HH */
