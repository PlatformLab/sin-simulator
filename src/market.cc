/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"
        size_t time_;
        std::deque<Interval> future_intervals_{};
        std::deque<Interval> past_intervals_{};
        std::deque<MoneyExchanged> transactions_{};

void Market::add_interval(size_t uid, size_t start, size_t end, double cost)
{
    assert(start <= time_);
    future_intervals.emplace_back( { uid, start, end, cost } );
}

double Market::cost_for_intervals(size_t uid, size_t start, size_t end, size_t num_intervals)
{

}

double Market::buy_intervals(size_t uid, size_t start, size_t end, size_t num_intervals, double max_payment)
{

}

void advance_time()
{
    time_++;
    future_intervals_ starting before time_ move to past_intervals_
}
