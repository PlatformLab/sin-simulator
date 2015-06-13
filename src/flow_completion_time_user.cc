/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "flow_completion_time_user.hh" 

using namespace std;

FlowCompletionTimeUser::FlowCompletionTimeUser( const std::string &name, const size_t flow_start_time, const size_t num_packets )
: AbstractUser( name ),
    flow_start_time_( flow_start_time ),
    num_packets_( num_packets )
{
}

template <typename T>
static vector<size_t> priority_queue_to_vector( T &pq ) {
    vector<size_t> toRet;
    while ( not pq.empty() ) {
        toRet.emplace_back(pq.top());
        pq.pop();
    }
    // put stuff back
    for (size_t t : toRet) {
        pq.push( t );
    }
    return move( toRet );
}

static vector<size_t> idxs_to_buy( const deque<SingleSlot> &order_book, const string &name, size_t start_time, size_t num_packets_to_buy, const size_t latest_time_already_owned )
{
    auto compare_prices_at_idxs = [ &order_book ](const size_t &a, const size_t &b){ return order_book.at( a ).best_offer().cost < order_book.at( b ).best_offer().cost; };

    priority_queue<size_t, vector<size_t>, decltype( compare_prices_at_idxs )> idxs_to_buy( compare_prices_at_idxs );
    double idxs_to_buy_cost = 0;

    size_t idx = start_time < order_book.front().time ? 0 : order_book.front().time - start_time;
    while  (idxs_to_buy.size() != num_packets_to_buy) {
        const SingleSlot &potential_slot = order_book.at( idx );
        bool can_buy = potential_slot.owner != name and not potential_slot.offers.empty();
        if ( can_buy ) {
            idxs_to_buy.push( idx );
            idxs_to_buy_cost += potential_slot.best_offer().cost;
        }
        idx++;
        assert(idx < order_book.size() && "can't buy slots we need");
    }

    assert( idx != 0 );
    idx--; // we incremented 1 too many times in while loop

    size_t min_flow_completion_time = max(order_book.at( idx ).time, latest_time_already_owned);
    cout << name << " got min fct " <<  min_flow_completion_time << " and start time " << start_time << endl;

    vector<size_t> best_idxs = priority_queue_to_vector( idxs_to_buy );
    assert(min_flow_completion_time >= start_time);
    double flow_benefit = -((double) min_flow_completion_time - (double) start_time);
    double best_utility = flow_benefit - idxs_to_buy_cost;
    cout << "best utility starting at" << best_utility << " and benefit is " << flow_benefit << endl;

    for (size_t i = idx + 1; i < order_book.size(); i++) {
        const SingleSlot &potential_slot = order_book.at( i );
        bool can_buy = potential_slot.owner != name and not potential_slot.offers.empty();
        if ( can_buy ) {
            idxs_to_buy.push( i );
            idxs_to_buy_cost += potential_slot.best_offer().cost;

            cout << "trying slot " << i << " has cost " << potential_slot.best_offer().cost << " while most expensive slot in idxs_to_buy is " << order_book.at( idxs_to_buy.top() ).best_offer().cost << endl;
            idxs_to_buy_cost -= order_book.at( idxs_to_buy.top() ).best_offer().cost;
            idxs_to_buy.pop();
            assert(idxs_to_buy.size() == num_packets_to_buy);

            double current_benefit = - (double) max( potential_slot.time, latest_time_already_owned ) - (double) start_time;
            double current_utility = (double) current_benefit - idxs_to_buy_cost;
            cout << "benefit for " << i << " is " << current_benefit << " and utility is " << current_utility << endl;

            if (current_utility > best_utility) {
                cout << "that is better than current best, " << best_utility << " so swapping" << endl;
                best_idxs = priority_queue_to_vector( idxs_to_buy );
                best_utility = current_utility;
            }
        }
    }

    assert( best_idxs.size() == num_packets_to_buy );
    return best_idxs;
}

template <typename T>
static vector<size_t> times_owned( const T &collection, const string &name )
{
    vector<size_t> toRet {};
    for (auto & item : collection)
    {
        if ( item.owner == name ) {
            toRet.emplace_back( item.time );
        }
    }
    return toRet;
}

void FlowCompletionTimeUser::take_actions( Market& mkt )
{
    auto &order_book = mkt.order_book();
    if ( order_book.empty() or order_book.front().time < flow_start_time_)
    {
        return;
    }

    size_t num_packets_sent = times_owned( mkt.packets_sent(), name_ ).size();

    vector<size_t> order_book_times_owned =  times_owned( order_book, name_ );
    size_t num_order_book_slots_owned = order_book_times_owned.size();

    assert ( num_packets_ >= ( num_packets_sent + num_order_book_slots_owned ) );

    size_t num_packets_to_buy = num_packets_ - num_packets_sent - num_order_book_slots_owned; 

    if ( num_packets_to_buy > 0 ) {
        size_t current_flow_completion_time;

        if ( order_book_times_owned.empty() ) {
            current_flow_completion_time = 0;
        } else {
            current_flow_completion_time = order_book_times_owned.back();
            cout << "current_flow_completion_time " << current_flow_completion_time << endl;
        }

        auto buying_slots = idxs_to_buy( order_book, name_, flow_start_time_, num_packets_to_buy, current_flow_completion_time );

        cout << name_ << " is buying slots: ";
        for ( auto &idx : buying_slots ) {
            cout << idx << ", ";
            mkt.add_bid_to_slot( idx, { order_book.at( idx ).best_offer().cost, name_ } );
            assert( order_book.at( idx ).owner == name_ ); // assert we succesfully got it
        }
        cout << "done!" << endl;

        // now price all slots we own

        // TODO, all slots but last one will be priced same so only do twice
        size_t idx = 0;
        for ( auto &slot : order_book ) {
            if (slot.owner != name_) {
                idx++;
                continue;
            }

            mkt.clear_offers_from_slot( idx, name_ );

            // don't count slot we are selling for flow completion time
            size_t cur_flow_completion_time = 0;
            size_t flow_completion_time_if_sold = 0;
            for ( size_t i = 0; i < order_book.size(); i++) {
                if ( order_book.at(i).owner == name_ ) {
                    cur_flow_completion_time = order_book.at(i).time; // TODO dont calc this every time
                    if ( i != idx ) {
                        flow_completion_time_if_sold = order_book.at(i).time;
                    }
                }
            }
            cout << "got fct if sold " << flow_completion_time_if_sold << " for " << idx << endl;

            num_packets_to_buy = 1;
            size_t idx_would_buy_instead = idxs_to_buy( order_book, name_, flow_start_time_, num_packets_to_buy, flow_completion_time_if_sold ).front();
            cout << "would buy " << idx_would_buy_instead << " instead of " << idx;
            const SingleSlot &slot_would_buy_instead = order_book.at( idx_would_buy_instead );

            // if the time of slot we buy instead is later than the last time if we sold, then it decreases benefit
            double benefit_delta = min( 0., (double) cur_flow_completion_time - (double) slot_would_buy_instead.time );

            double utility_delta = benefit_delta - slot_would_buy_instead.best_offer().cost;

            mkt.add_offer_to_slot( idx, { -utility_delta + .01, name_ } );
            cout << " pricing it at " << -utility_delta + .01 << endl;

            idx++;
        }
    }
}

bool FlowCompletionTimeUser::done( const Market& mkt ) const
{
    return num_packets_ == times_owned( mkt.packets_sent(), name_ ).size();
}

void FlowCompletionTimeUser::print_stats( const Market& ) const
{
        cout << "user " << name_ << " started at time " << flow_start_time_ << " and finished at time ?" <<  endl;
}
