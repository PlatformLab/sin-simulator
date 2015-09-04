/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"

using namespace std;

bool cost_cmp(const Interval *a, const Interval *b)
{
    return a->cost < b->cost;
}

void Market::add_interval( Interval interval )
{
//    cout << uid_to_string( interval.owner ) << " trying to add (" << interval.start << ", " << interval.end << ") at market time " << time_ << endl;
    assert( interval.start >= time_ );
    intervals_.push_back( interval );
    version_++;
}

vector<Interval *> Market::cheapest_interals_in_range( size_t uid, size_t start, size_t end, size_t num_intervals )
{
    assert( start >= time_ && "can't price intervals from past" );

    vector<Interval *> cheapest_intervals;
    for ( auto &i : intervals_ ) {
        const bool can_buy = i.start >= start and i.end <= end and i.owner != uid and i.cost > 0;
        if ( can_buy ) {
            if ( cheapest_intervals.size() == num_intervals and i.cost < cheapest_intervals[0]->cost ) {
                pop_heap( cheapest_intervals.begin(), cheapest_intervals.end(), cost_cmp );
                cheapest_intervals.back() = &i;
                push_heap( cheapest_intervals.begin(), cheapest_intervals.end(), cost_cmp );
            } else if ( cheapest_intervals.size() < num_intervals ) {
                cheapest_intervals.push_back( &i );
                push_heap( cheapest_intervals.begin(), cheapest_intervals.end(), cost_cmp );
            }
        }
    }

    return cheapest_intervals;
}

double Market::cost_for_intervals( size_t uid, size_t start, size_t end, size_t num_intervals )
{
    double cheapest_with_meta_intervals = 0;
    for ( auto &m : meta_intervals_ ) {
        const bool can_buy = m.start >= start and m.end <= end and m.owner != uid and m.intervals.size() >= num_intervals;
        if ( can_buy ) {
            for ( auto &offer_pair : m.offers ) {
                size_t alternative_end = offer_pair.first;
                double cost_to_move = offer_pair.second;
                double cost_for_replacements = cost_for_intervals( m.owner, m.start, alternative_end, num_intervals );
                double cost_using_this_meta_interval = cost_for_replacements + cost_to_move;
                if ( cost_for_replacements > 0 ) {
                    if ( cheapest_with_meta_intervals == 0 or cost_using_this_meta_interval < cheapest_with_meta_intervals ) {
                        cheapest_with_meta_intervals = cost_using_this_meta_interval;
                    }
                }
            }
        }
    }

    vector<Interval *> cheapest_intervals = cheapest_interals_in_range( uid, start, end, num_intervals );
    double cheapest_with_intervals = 0;

    if ( cheapest_intervals.size() == num_intervals ) {
        for ( auto &i : cheapest_intervals ) {
            cheapest_with_intervals += i->cost;
        }
    }

    if ( cheapest_with_intervals and cheapest_with_meta_intervals ) {
        return min( cheapest_with_intervals, cheapest_with_meta_intervals );
    } else {
        return max( cheapest_with_intervals, cheapest_with_meta_intervals ); // one or both is 0
    }
}

double Market::buy_intervals( size_t uid, size_t start, size_t end, size_t num_intervals, double max_payment, vector<pair<size_t, double>> offers )
{
    vector<Interval *> cheapest_intervals = cheapest_interals_in_range( uid, start, end, num_intervals );
    if ( cheapest_intervals.size() == num_intervals ) {
        double total_cost = 0;
        for ( auto &i : cheapest_intervals ) {
            total_cost += i->cost;
        }

        if ( total_cost <= max_payment ) {
            MetaInterval new_meta_interval;
            new_meta_interval.owner = uid;
            new_meta_interval.offers = move( offers );

            // buy the slots
            for ( auto *i : cheapest_intervals ) {
                transactions_.push_back( { i->owner, uid, i->cost } );
                i->owner = uid;
                i->cost = numeric_limits<double>::lowest();

                new_meta_interval.intervals.emplace_back( i );
                new_meta_interval.start = std::min( new_meta_interval.start, i->start ); 
                new_meta_interval.end = std::max( new_meta_interval.end, i->end );
            }
            meta_intervals_.push_back( new_meta_interval );

            version_++;
            return total_cost;
        }
    }
    return 0;
}

bool Market::empty()
{
    // return true if all interval start times have passed
    for ( auto &i : intervals_ ) {
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
