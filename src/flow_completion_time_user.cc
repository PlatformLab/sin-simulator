/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#define DEBUG_PRINT 0

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
        assert(idx < order_book.size() && "can't buy slots we need");
        const SingleSlot &potential_slot = order_book.at( idx );
        bool can_buy = potential_slot.owner != name and potential_slot.has_offers();
        if ( can_buy ) {
            idxs_to_buy.push( idx );
            idxs_to_buy_cost += potential_slot.best_offer().cost;
        }
        idx++;
    }

    assert( idx != 0 );
    idx--; // we incremented 1 too many times in while loop

    size_t min_flow_completion_time = max(order_book.at( idx ).time, latest_time_already_owned);
    if (DEBUG_PRINT)
        cout << name << " got min fct " <<  min_flow_completion_time << " and start time " << start_time << " and latest already owned" << latest_time_already_owned << endl;

    vector<size_t> best_idxs = priority_queue_to_vector( idxs_to_buy );
    assert(min_flow_completion_time >= start_time);
    double flow_benefit = -((double) min_flow_completion_time - (double) start_time);
    double best_utility = flow_benefit - idxs_to_buy_cost;
    if (DEBUG_PRINT)
        cout << "best utility starting at" << best_utility << " and benefit is " << flow_benefit << endl;

    for (size_t i = idx + 1; i < order_book.size(); i++) {
        const SingleSlot &potential_slot = order_book.at( i );
        bool can_buy = potential_slot.owner != name and potential_slot.has_offers();
        if ( can_buy ) {
            idxs_to_buy.push( i );
            idxs_to_buy_cost += potential_slot.best_offer().cost;

            if (DEBUG_PRINT)
                cout << "trying slot " << i << " has cost " << potential_slot.best_offer().cost << " while most expensive slot in idxs_to_buy is " << order_book.at( idxs_to_buy.top() ).best_offer().cost << endl;
            idxs_to_buy_cost -= order_book.at( idxs_to_buy.top() ).best_offer().cost;
            idxs_to_buy.pop();
            assert(idxs_to_buy.size() == num_packets_to_buy);

            double current_benefit = - ( (double) max( potential_slot.time, latest_time_already_owned ) - (double) start_time );
            double current_utility = (double) current_benefit - idxs_to_buy_cost;
            if (DEBUG_PRINT)
                cout << "benefit for " << i << " is " << current_benefit << " and utility is " << current_utility << endl;

            if (current_utility > best_utility) {
                if (DEBUG_PRINT)
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
    for (auto &item : collection)
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

    vector<size_t> order_book_times_owned = times_owned( order_book, name_ );
    size_t num_order_book_slots_owned = order_book_times_owned.size();

    assert ( num_packets_ >= ( num_packets_sent + num_order_book_slots_owned ) );

    size_t num_packets_to_buy = num_packets_ - num_packets_sent - num_order_book_slots_owned; 

    if ( num_packets_to_buy > 0 ) {
        size_t current_flow_completion_time;

        if ( order_book_times_owned.empty() ) {
            current_flow_completion_time = 0;
        } else {
            current_flow_completion_time = order_book_times_owned.back();
            if (DEBUG_PRINT)
                cout << "current_flow_completion_time " << current_flow_completion_time << endl;
        }

        auto buying_slots = idxs_to_buy( order_book, name_, flow_start_time_, num_packets_to_buy, current_flow_completion_time );

        if (DEBUG_PRINT)
            cout << name_ << " is buying slots: ";
        for ( auto &idx : buying_slots ) {
            if (DEBUG_PRINT)
                cout << idx << ", ";
            mkt.add_bid_to_slot( idx, { order_book.at( idx ).best_offer().cost, name_ } );
            assert( order_book.at( idx ).owner == name_ ); // assert we succesfully got it
        }
        if (DEBUG_PRINT)
            cout << "done!" << endl;
    }

    // now price all slots we own, there are two prices, one for the non-last slot, which selling couldnt change flow completion time, and one for the last slot

    order_book_times_owned = times_owned( order_book, name_ ); // redo now that we may have bought slots
    if ( order_book_times_owned.empty() ) {
        return;
    }

    // first price last slot
    size_t current_flow_completion_time = order_book_times_owned.back();
    size_t flow_completion_time_if_sold_back = 0;
    if (order_book_times_owned.size() > 1) {
        flow_completion_time_if_sold_back = order_book_times_owned.at( order_book_times_owned.size() - 2 );
    }
    num_packets_to_buy = 1;
    size_t back_replacement_idx = idxs_to_buy( order_book, name_, flow_start_time_, num_packets_to_buy, flow_completion_time_if_sold_back ).front();
    size_t non_back_replacement_idx = idxs_to_buy( order_book, name_, flow_start_time_, num_packets_to_buy, current_flow_completion_time ).front();

    double back_benefit_delta = (double) current_flow_completion_time - (double) max( flow_completion_time_if_sold_back, order_book.at( back_replacement_idx ).time);
    double non_back_benefit_delta = min( (double) current_flow_completion_time - (double) order_book.at( non_back_replacement_idx ).time, 0. );

    double back_sell_price = .01 - ( back_benefit_delta - order_book.at( back_replacement_idx ).best_offer().cost );
    double non_back_sell_price = .01 - ( non_back_benefit_delta - order_book.at( non_back_replacement_idx ).best_offer().cost );

    size_t idx = 0;
    for ( auto &slot : order_book ) {
        if (slot.owner == name_) {
            if ( slot.time == current_flow_completion_time ) {
                if ( not slot.has_offers() or slot.best_offer().cost != back_sell_price ) {
                    mkt.clear_offers_from_slot( idx, name_ );
                    mkt.add_offer_to_slot( idx, { back_sell_price, name_ } );
                }
            } else {
                if ( not slot.has_offers() or slot.best_offer().cost != non_back_sell_price ) {
                    mkt.clear_offers_from_slot( idx, name_ );
                    mkt.add_offer_to_slot( idx, { non_back_sell_price, name_ } );
                }
            }
        }
        idx++;
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
