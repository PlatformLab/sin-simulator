/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#define DEBUG_PRINT 0

#include "flow_completion_time_user.hh" 

using namespace std;

FlowCompletionTimeUser::FlowCompletionTimeUser( const std::string &name, const size_t flow_start_time, const size_t num_packets )
: AbstractUser( name ),
    flow_start_time_( flow_start_time ),
    num_packets_( num_packets ),
    done_( false )
{
}

/* Returns the benefit score for a given flow completion time */
double FlowCompletionTimeUser::get_benefit(size_t flow_completion_time) const
{
    /*
    if (flow_completion_time < flow_start_time_)
        cout << "AAAA " << flow_completion_time <<" greater than "<<flow_start_time_ << endl;
    assert(flow_completion_time >= flow_start_time_);
    */
    return - (double) (flow_completion_time + 1 - flow_start_time_);
}

bool FlowCompletionTimeUser::can_buy(const SingleSlot &slot) const
{
    return slot.owner != name_ and slot.has_offers();
}

vector<size_t> FlowCompletionTimeUser::pick_n_slots_to_buy( const deque<SingleSlot> &order_book, size_t num_packets_to_buy, const size_t latest_time_already_owned )
{
    priority_queue<pair<double, size_t>> idxs_to_buy;
    double idxs_to_buy_cost = 0;

    size_t idx = 0;
    while  (idxs_to_buy.size() < num_packets_to_buy) {
        assert(idx < order_book.size() && "can't buy slots we need");
        const SingleSlot &potential_slot = order_book.at( idx );
        if ( can_buy( potential_slot ) ) {
            double slot_cost = potential_slot.best_offer().cost;
            idxs_to_buy.push( {slot_cost, idx} );
            idxs_to_buy_cost += slot_cost;
        }
        idx++;
    }

    assert( idx != 0 );

    // keep a copy of best slots seen so far
    priority_queue<pair<double, size_t>> best_idxs = idxs_to_buy;
    double flow_benefit = get_benefit( max(order_book.at( idx - 1 ).time, latest_time_already_owned) ); // idx - 1 is idx of latest potential slot on pq
    double best_utility =  flow_benefit - idxs_to_buy_cost;
    if (DEBUG_PRINT)
        cout << "best utility starting at" << best_utility << " and benefit is " << flow_benefit << endl;

    double most_expensive_slot_cost = idxs_to_buy.top().first;
    for (size_t i = idx; i < order_book.size(); i++) {
        const SingleSlot &potential_slot = order_book.at( i );
        if ( can_buy( potential_slot ) and potential_slot.best_offer().cost < most_expensive_slot_cost ) {
            double slot_cost = potential_slot.best_offer().cost;
            idxs_to_buy.push( {slot_cost, i} );
            idxs_to_buy_cost += slot_cost;

            if (DEBUG_PRINT)
                cout << "trying slot " << i << " has cost " << slot_cost << " while most expensive slot in idxs_to_buy is " << most_expensive_slot_cost << endl;
            idxs_to_buy_cost -= idxs_to_buy.top().first;
            idxs_to_buy.pop();

            assert(idxs_to_buy.size() == num_packets_to_buy);
            most_expensive_slot_cost = idxs_to_buy.top().first;

            double current_benefit = get_benefit( max( potential_slot.time, latest_time_already_owned ) );
            double current_utility = current_benefit - idxs_to_buy_cost;
            if (DEBUG_PRINT)
                cout << "benefit for " << i << " is " << current_benefit << " and utility is " << current_utility << endl;

            if (current_utility > best_utility) {
                if (DEBUG_PRINT)
                    cout << "that is better than current best, " << best_utility << " so swapping" << endl;
                best_idxs = idxs_to_buy;
                best_utility = current_utility;
            }
        }
    }

    vector<size_t> toRet {};
    while ( not best_idxs.empty() ) {
        toRet.push_back( best_idxs.top().second );
        best_idxs.pop();
    }

    assert( toRet.size() == num_packets_to_buy );

    return move( toRet );
}

template <typename T>
static double money_earned( const T &collection, const string &name )
{
    double toRet = 0;
    for (auto &item : collection)
    {
        if ( item.to == name ) {
            toRet += item.amount;
        }
    }
    return toRet;
}

template <typename T>
static size_t num_owned( const T &collection, const string &name )
{
    size_t toRet = 0;
    for (auto &item : collection)
    {
        if ( item.owner == name ) {
            toRet++;
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

    const size_t num_packets_sent = num_owned( mkt.packets_sent(), name_ );

    const size_t num_order_book_slots_owned = num_owned( order_book, name_ );

    assert ( num_packets_ >= ( num_packets_sent + num_order_book_slots_owned ) );

    size_t num_packets_to_buy = num_packets_ - num_packets_sent - num_order_book_slots_owned; 

    if ( num_packets_to_buy > 0 ) {
        size_t current_flow_completion_time = 0;

        if ( num_order_book_slots_owned > 0 ) {
            size_t idx = order_book.size() - 1;
            while ( idx != 0 ) { // skip idx 0 dont care
                if (order_book.at(idx).owner == name_) {
                    current_flow_completion_time = order_book.at(idx).time;
                    break;
                }
                idx--;
            }
        }
        if (DEBUG_PRINT)
            cout << "current_flow_completion_time " << current_flow_completion_time << endl;

        vector<size_t> buying_slots = pick_n_slots_to_buy( order_book, num_packets_to_buy, current_flow_completion_time );

        if (DEBUG_PRINT)
            cout << name_ << " is buying slots: ";

        for ( size_t idx : buying_slots )
        {
            if (DEBUG_PRINT)
                cout << idx << ", ";

            const double slot_cost = order_book.at( idx ).best_offer().cost;
            mkt.add_bid_to_slot( idx, { slot_cost, name_ } );

            current_flow_completion_time = max( order_book.at( idx ).time, current_flow_completion_time );// might not be necessary
            money_spent_ += slot_cost; // assumes bid worked
            assert( order_book.at( idx ).owner == name_ ); // assert we succesfully got it
        }

        double new_expected_utility = - (double) (current_flow_completion_time - flow_start_time_) - money_spent_ + money_earned( mkt.money_exchanged(), name_ );
        if (DEBUG_PRINT)
            cout << name_ << "'s new expected utility " << new_expected_utility << " while previous was " << expected_utility_ << " and previous best was " << best_expected_utility_ << endl;
        if ( new_expected_utility > best_expected_utility_ ) {
            if (DEBUG_PRINT)
                cout << "new best expected utility" << endl;
            best_expected_utility_ = new_expected_utility;
        }
        if ( new_expected_utility <= expected_utility_ ) {
           if (DEBUG_PRINT)
                cout << "!!DECREASE FROM PREVIOUS EXPECTED UTILITY" << endl;
        }
        //assert( new_expected_utility > expected_utility_ );
        expected_utility_ = new_expected_utility;

        if (DEBUG_PRINT)
            cout << "done!" << endl;
    }

    // now price all slots we own, there are two prices, one for the non-last slot, which selling couldnt change flow completion time, and one for the last slot

    if ( num_order_book_slots_owned == 0 and num_packets_to_buy == 0) {
        return;
    }

    // first price last slot
    size_t current_flow_completion_time = 0;
    size_t flow_completion_time_if_sold_back = 0;
    bool has_more_than_one_slot = ( num_order_book_slots_owned + num_packets_to_buy ) > 1;

    size_t idx = order_book.size() - 1;
    while ( idx != 0 ) { // skip idx 0 dont care
        if ( order_book.at(idx).owner == name_ ) {
            if ( current_flow_completion_time == 0 ) {
                current_flow_completion_time = order_book.at(idx).time;
                if ( not has_more_than_one_slot ) {
                    break;
                }
            } else {
                flow_completion_time_if_sold_back = order_book.at(idx).time;
                break;
            }
        }
        idx--;
    }

    num_packets_to_buy = 1;
    size_t back_replacement_idx  = pick_n_slots_to_buy( order_book, num_packets_to_buy, flow_completion_time_if_sold_back ).front();
    size_t non_back_replacement_idx = pick_n_slots_to_buy( order_book, num_packets_to_buy, current_flow_completion_time ).front();

    double current_benefit = get_benefit( current_flow_completion_time );
    double back_benefit_delta = get_benefit( max( flow_completion_time_if_sold_back, order_book.at( back_replacement_idx ).time ) ) - current_benefit;
    double non_back_benefit_delta = get_benefit( max( current_flow_completion_time, order_book.at( non_back_replacement_idx ).time ) ) - current_benefit;
    //cout << "new calc gets: " << non_back_benefit_delta;

    non_back_benefit_delta = min( (double) current_flow_completion_time - (double) order_book.at( non_back_replacement_idx ).time, 0. );
    //cout << "old calc gets: " << non_back_benefit_delta;
    //cout << endl << endl;

    double back_sell_price = .01 - ( back_benefit_delta - order_book.at( back_replacement_idx ).best_offer().cost );
    double non_back_sell_price = .01 - ( non_back_benefit_delta - order_book.at( non_back_replacement_idx ).best_offer().cost );

    idx = 0;
    for ( auto &slot : order_book ) {
        if (slot.owner == name_) {
            if ( slot.time == current_flow_completion_time ) {
                if ( not slot.has_offers() or slot.best_offer().cost != back_sell_price ) {
                    mkt.clear_offers_from_slot( idx, name_ );
                    mkt.add_offer_to_slot( idx, { back_sell_price, name_ } );
                }
                break;
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

bool FlowCompletionTimeUser::done( const Market& mkt )
{
    if (not done_) {
        done_ = num_packets_ == num_owned( mkt.packets_sent(), name_ );
    }
    return done_;
}

void FlowCompletionTimeUser::print_stats( const Market& ) const
{
        cout << name_ << " had best expected utility " << best_expected_utility_ << endl;
}
