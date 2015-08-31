/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"
#include <algorithm>

using namespace std;

bool cost_cmp(const Interval *a, const Interval *b)
{
    return a->cost < b->cost;
}

void Market::add_interval(size_t uid, size_t start, size_t end, double cost)
{
    cout << "uid " << uid << " trying to add (" << start << ", " << end << ") at market time " << time_ << endl;
    assert(start >= time_);
    intervals_.push_back( { uid, start, end, cost } );
    version_++;
}

vector<Interval *> Market::cheapest_interals_in_range(size_t uid, size_t start, size_t end, size_t num_intervals)
{
    assert( start >= time_ && "can't price intervals from past" );
    vector<Interval *> cheapest_intervals;
    for ( auto &i : intervals_ )
    {
        const bool can_buy = i.start >= start and i.end <= end and i.owner != uid;
        if ( can_buy )
        {
            cheapest_intervals.push_back( &i );
            push_heap( cheapest_intervals.begin(), cheapest_intervals.end(), cost_cmp );

            if ( cheapest_intervals.size() > num_intervals ) {
                pop_heap( cheapest_intervals.begin(), cheapest_intervals.end(), cost_cmp );
                cheapest_intervals.pop_back();
            }
        }
    }

    return cheapest_intervals;
}

double Market::cost_for_intervals(size_t uid, size_t start, size_t end, size_t num_intervals)
{
    vector<Interval *> cheapest_intervals = cheapest_interals_in_range( uid, start, end, num_intervals );
    if ( cheapest_intervals.size() == num_intervals ) {
        double total_cost = 0;
        for ( auto &i : cheapest_intervals ) {
            total_cost += i->cost;
        }
        return total_cost;
    } else {
        return 0;
    }
}

double Market::buy_intervals(size_t uid, size_t start, size_t end, size_t num_intervals, double max_payment)
{
    vector<Interval *> cheapest_intervals = cheapest_interals_in_range( uid, start, end, num_intervals );
    if ( cheapest_intervals.size() == num_intervals ) {
        double total_cost = 0;
        for ( auto &i : cheapest_intervals ) {
            total_cost += i->cost;
        }

        if ( total_cost <= max_payment ) {
            // buy the slots
            for ( auto *i : cheapest_intervals ) {
                transactions_.push_back( { i->owner, uid, i->cost } );
                i->owner = uid;
                i->cost = numeric_limits<double>::max();
            }

            version_++;
            return total_cost;
        }
    }
    return 0;
}

bool Market::empty()
{
    // return true if all interval start times have passed
    for ( auto &i : intervals_ )
    {
        if ( i.start >= time_ )
        {
            return false;
        }
    }
    return true;
}

void Market::advance_time()
{
    time_++;
}
