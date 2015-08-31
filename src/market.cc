/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"

void Market::add_interval(size_t uid, size_t start, size_t end, double cost)
{
    assert(start <= time_);
    future_intervals_.push_back( { uid, start, end, cost } );
}

double Market::cost_for_intervals(size_t uid, size_t start, size_t end, size_t num_intervals) const
{
    //pq then go through whole thing
    return 0;
}

double Market::buy_intervals(size_t uid, size_t start, size_t end, size_t num_intervals, double max_payment)
{
    return 0; //cost to them, 0 if fail.
}

void Market::advance_time()
{
    time_++;

    //future_intervals_ starting before time_ move to past_intervals_
    for ( auto it = future_intervals_.begin(); it != future_intervals_.end(); it++ ) {
        if ( it->start < time_ )
        {
            past_intervals_.splice( past_intervals_.end(), future_intervals_, it );
        }
    }
}
