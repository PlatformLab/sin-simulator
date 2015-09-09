/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"

using namespace std;

void Market::add_interval( Interval interval )
{
    //    cout << uid_to_string( interval.owner ) << " trying to add (" << interval.start << ", " << interval.end << ") at market time " << time_ << endl;
    assert( interval.start >= time_ );
    intervals_.push_back( interval );
    version_++;
}

vector<Interval *> Market::cheapest_intervals_in_range( size_t uid, size_t start, size_t end, size_t num_intervals )
{
    assert( start >= time_ && "can't price intervals from past" );

    vector<Interval *> cheapest_intervals;
    for ( auto &i : intervals_ ) {
        if ( i.start >= start and i.end <= end ) {
            auto cost_compare = [ uid ] ( const Interval *a, const Interval *b )
            { double acost = a->owner == uid ? 0 : a->cost; double bcost = b->owner == uid ? 0 : b->cost; return acost < bcost; };

            double i_cost = ( i.owner == uid ) ? 0 : i.cost;
            if ( cheapest_intervals.size() == num_intervals and i_cost < cheapest_intervals[0]->cost ) {
                pop_heap( cheapest_intervals.begin(), cheapest_intervals.end(), cost_compare );
                cheapest_intervals.back() = &i;
                push_heap( cheapest_intervals.begin(), cheapest_intervals.end(), cost_compare );
            } else if ( cheapest_intervals.size() < num_intervals ) {
                cheapest_intervals.push_back( &i );
                push_heap( cheapest_intervals.begin(), cheapest_intervals.end(), cost_compare );
            }
        }
    }

    return cheapest_intervals;
}

std::tuple<double, std::vector<MetaInterval>::iterator, std::pair<size_t, double>, std::vector<Interval *>, std::vector<Interval *>> Market::best_meta_interval ( size_t uid, size_t start, size_t end, size_t num_intervals ) {
    double cheapest_with_meta_intervals = numeric_limits<double>::max();
    std::vector<MetaInterval>::iterator best_meta_interval = meta_intervals_.end();
    std::pair<size_t, double> best_offer;
    std::vector<Interval *> intervals_to_move;
    std::vector<Interval *> replacements;

    /* filters out invalid meta_intervals */
    for ( auto m = meta_intervals_.begin(); m != meta_intervals_.end(); ) {
        bool valid = true;
        for ( auto &i : m->intervals ) {
            if ( i->start < time_ or i->owner != m->owner ) {
                valid = false;
                break;
            }
        }
        if ( valid ) {
            m++;
        } else {
            m = meta_intervals_.erase( m );
            //cout << "WOW erased meta interval from " << uid_to_string( m->owner ) << endl;
        }
    }

    for ( auto m = meta_intervals_.begin(); m != meta_intervals_.end(); ++m ) {
        if ( m->owner != uid and m->intervals.size() >= num_intervals ) {
            std::vector<Interval *> intervals_to_use;
            for ( auto &i : m->intervals ) {
                if ( i->start >= start and i->end <= end ) {
                    intervals_to_use.push_back( i );
                    if ( intervals_to_use.size() == num_intervals ) {
                        break;
                    }
                }
            }

            if ( intervals_to_use.size() == num_intervals ) {
                for ( auto &offer_pair : m->offers ) {
                    size_t alternative_end = offer_pair.first;
                    double cost_to_move = offer_pair.second;
                    std::vector<Interval *> potential_replacements = cheapest_intervals_in_range( uid, start, alternative_end, num_intervals );
                    if ( potential_replacements.size() == num_intervals ) {
                        double cost_for_replacements = 0;
                        for ( auto &i : potential_replacements ) {
                            double i_cost = ( i->owner == uid ) ? 0 : i->cost;
                            cost_for_replacements += i_cost;
                        }

                        double cost_using_this_meta_interval = cost_for_replacements + cost_to_move;

                        if ( cost_using_this_meta_interval <= cheapest_with_meta_intervals ) {
                            cheapest_with_meta_intervals = cost_using_this_meta_interval;
                            best_meta_interval = m;
                            best_offer = offer_pair;
                            intervals_to_move = intervals_to_use;
                            replacements = potential_replacements;
                        }
                    }
                }
            }
        }
    }
    return std::tie( cheapest_with_meta_intervals, best_meta_interval, best_offer, intervals_to_move, replacements );
}

double Market::cost_for_intervals( size_t uid, size_t start, size_t end, size_t num_intervals )
{
    double cheapest_with_meta_intervals = std::get<0>( best_meta_interval( uid, start, end, num_intervals ) );

    vector<Interval *> cheapest_intervals = cheapest_intervals_in_range( uid, start, end, num_intervals );
    double cheapest_with_intervals = 0;

    if ( cheapest_intervals.size() == num_intervals ) {
        for ( auto &i : cheapest_intervals ) {
            double i_cost = ( i->owner == uid ) ? 0 : i->cost;
            cheapest_with_intervals += i_cost;
            //cout << "cheapest " << i->start << " owned by " << uid_to_string( i->owner ) << " with cost " << i_cost << endl;
        }
        //cout << "cheapest with intervals was " << cheapest_with_intervals << endl;
    } else {
        cheapest_with_intervals = numeric_limits<double>::max();
    }

    return min( cheapest_with_intervals, cheapest_with_meta_intervals );
}

double Market::buy_intervals( size_t uid, size_t start, size_t end, size_t num_intervals, double max_payment, vector<pair<size_t, double>> offers )
{
    auto meta_interval_results = best_meta_interval( uid, start, end, num_intervals );
    double cheapest_with_meta_intervals = std::get<0>( meta_interval_results );
    vector<Interval *> cheapest_intervals = cheapest_intervals_in_range( uid, start, end, num_intervals );

    double cheapest_with_intervals = 0;
    if ( cheapest_intervals.size() != num_intervals ) {
        cheapest_with_intervals = numeric_limits<double>::max();
    } else {
        for ( auto &i : cheapest_intervals ) {
            double i_cost = ( i->owner == uid ) ? 0 : i->cost;
            cheapest_with_intervals += i_cost;
        }
    }

    if ( cheapest_with_intervals <= cheapest_with_meta_intervals and cheapest_with_intervals != numeric_limits<double>::max() ) {
        assert( cheapest_with_intervals <= max_payment );
        double market_changed = false;

        MetaInterval new_meta_interval;
        new_meta_interval.owner = uid;
        new_meta_interval.offers = move( offers );

        bool add_meta_interval = true;
        // buy the slots
        for ( auto *i : cheapest_intervals ) {
            if ( i->owner != uid ) {
                market_changed = true;
                transactions_.push_back( { i->owner, uid, i->cost } );
                i->owner = uid;
                i->cost = numeric_limits<double>::max();

            }
            new_meta_interval.intervals.insert( i ); // TODO uhh do we double add these then?
            for ( auto &m : meta_intervals_ ) {
                if ( m.owner == uid ) {
                    for ( auto *existing_i : m.intervals ) {
                        if ( i == existing_i ) {
                            add_meta_interval = false;
                        }
                    }
                }
            }

        }

        if ( add_meta_interval ) {
            meta_intervals_.push_back( new_meta_interval );
            market_changed = true;
        }
        if ( market_changed ) {
            version_++;
        }
        return cheapest_with_intervals;
    } else if ( cheapest_with_meta_intervals < cheapest_with_intervals ) { 
        std::vector<MetaInterval>::iterator best_meta_interval;
        std::pair<size_t, double> best_offer;
        std::vector<Interval *> intervals_to_move;
        std::vector<Interval *> replacements;
        std::tie<double, std::vector<MetaInterval>::iterator, std::pair<size_t, double>, std::vector<Interval *>, std::vector<Interval *>>
            ( cheapest_with_meta_intervals, best_meta_interval, best_offer, intervals_to_move, replacements ) = meta_interval_results;

        double total_payments = 0;
        transactions_.push_back( { best_meta_interval->owner, uid, best_offer.second } );
        total_payments += best_offer.second;
        if ( verbose_ ) {
            cout << uid_to_string( uid ) << " paying $" << best_offer.second << " for meta interval from " << uid_to_string( best_meta_interval->owner ) << " moving end to " << best_offer.first << " for $" << best_offer.second << endl;
            //cout << replacements.size() << " replacements and intervals to move "  << intervals_to_move.size() << endl;
        }
        assert( replacements.size() == intervals_to_move.size() );
        for ( auto *i : replacements ) {
            if ( i->owner != uid ) {
                transactions_.push_back( { i->owner, uid, i->cost } );
                total_payments += i->cost;
            }
            i->owner = best_meta_interval->owner;
            i->cost = numeric_limits<double>::max();

            best_meta_interval->intervals.insert( i );
        }
        for ( auto *i : intervals_to_move ) {
            i->owner = uid;
            i->cost = numeric_limits<double>::max();
            best_meta_interval->intervals.erase( i );
        }

        // change offer prices to reflect an offer was taken ( if interval end moves back one time unit that means the cost to move back two time units should be cheaper than before )
        const double offers_price_offset = best_offer.second;
        for ( auto &offer_pair : best_meta_interval->offers ) {
            offer_pair.second -= offers_price_offset;
        }

        // add new meta interval
        MetaInterval new_meta_interval;
        new_meta_interval.owner = uid;
        new_meta_interval.offers = move( offers );
        for ( auto *i : intervals_to_move ) {
            new_meta_interval.intervals.insert( i );
        }
        meta_intervals_.push_back( new_meta_interval );


        version_++;
        if ( verbose_ ) {
            cout << uid_to_string( uid ) << " made total payments " << total_payments << " and expected " << cheapest_with_meta_intervals << endl;
        }
        assert( abs( total_payments - cheapest_with_meta_intervals ) < .0001 ); // damn rounding errors
        return total_payments;
    } else {
        return numeric_limits<double>::max();
    }
}

size_t Market::intervals_owned_by_user( const size_t uid ) const
{
    size_t toRet = 0;

    for ( auto &i : intervals_ ) {
        if ( i.start < time_ and i.owner == uid ) {
            toRet++;
        }
    }
    return toRet;
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
