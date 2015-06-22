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
    assert(flow_completion_time >= flow_start_time_);
    return - (double) (flow_completion_time + 1 - flow_start_time_);
}

bool FlowCompletionTimeUser::can_buy(const SingleSlot &slot) const
{
    return slot.owner != name_ and slot.has_offers();
}

void FlowCompletionTimeUser::price_owned_slots( Market &mkt )
{
    auto &order_book = mkt.order_book();
    size_t flow_completion_time = 0;
    size_t flow_completion_time_if_sold_back = 0;

    for ( auto rit = order_book.rbegin(); rit != order_book.rend(); rit++) {
        if ( rit->owner == name_ ) {
            if (flow_completion_time == 0 ) {
                flow_completion_time = rit->time;
            } else {
                flow_completion_time_if_sold_back = rit->time;
                break;
            }
        }
    }

    size_t back_replacement_idx  = pick_n_slots_to_buy( order_book, 1, flow_completion_time_if_sold_back ).front();
    size_t non_back_replacement_idx = pick_n_slots_to_buy( order_book, 1, flow_completion_time ).front();

    double current_benefit = get_benefit( flow_completion_time );
    double benefit_with_back_replacement = get_benefit( max( flow_completion_time_if_sold_back, order_book.at( back_replacement_idx ).time ) );
    double benefit_with_non_back_replacement = get_benefit( max( flow_completion_time, order_book.at( non_back_replacement_idx ).time ) );

    double back_benefit_delta = benefit_with_back_replacement - current_benefit;
    double non_back_benefit_delta = benefit_with_non_back_replacement - current_benefit;

    double back_sell_price = .01 - ( back_benefit_delta - order_book.at( back_replacement_idx ).best_offer().cost );
    double non_back_sell_price = .01 - ( non_back_benefit_delta - order_book.at( non_back_replacement_idx ).best_offer().cost );

    size_t idx = 0;
    for ( const SingleSlot &slot : order_book ) {
        if ( slot.owner == name_ ) {
            if ( slot.time == flow_completion_time ) {
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

// fills and then keeps a priority queue of the cheapest n (=num_packets_to_buy) slots
// then keeps a copy of the set of cheapest slots with the most utility and returns that
vector<size_t> FlowCompletionTimeUser::pick_n_slots_to_buy( const deque<SingleSlot> &order_book, size_t num_packets_to_buy, const size_t latest_time_already_owned ) const
{
    priority_queue<pair<double, size_t>> costs_and_indices_to_buy;
    double total_cost = 0;

    priority_queue<pair<double, size_t>> best_indicies;
    double best_utility = std::numeric_limits<double>::lowest();

    for (size_t i = 0; i < order_book.size(); i++) {
        const SingleSlot &slot = order_book.at( i );

        if ( can_buy( slot ) ) {
            double slot_cost = slot.best_offer().cost;
            // add to the priority queue if it is not full size yet
            // or if the slot is cheaper than the most expensive slot on the priority queue
            if ( costs_and_indices_to_buy.size() < num_packets_to_buy or slot_cost < costs_and_indices_to_buy.top().first ) {
                costs_and_indices_to_buy.push( {slot_cost, i} );
                total_cost += slot_cost;

                // if we added a new cheaper slot and put the priority queue over the number of packets we needed
                // take the most expensive one off
                if ( costs_and_indices_to_buy.size() > num_packets_to_buy ) {
                    total_cost -= costs_and_indices_to_buy.top().first;
                    costs_and_indices_to_buy.pop();
                }
            }

            if ( costs_and_indices_to_buy.size() == num_packets_to_buy ) {
                size_t flow_completion_time = max( slot.time, latest_time_already_owned );
                double benefit = get_benefit( flow_completion_time );
                double utility = benefit - total_cost;

                if (utility > best_utility) {
                    if (DEBUG_PRINT)
                        cout << "that is better than current best, " << best_utility << " so swapping" << endl;
                    best_utility = utility;
                    best_indicies = costs_and_indices_to_buy;
                }
            }
        }
    }

    vector<size_t> toRet {};
    while ( not best_indicies.empty() ) {
        toRet.push_back( best_indicies.top().second );
        best_indicies.pop();
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
        size_t flow_completion_time = 0;

        if ( num_order_book_slots_owned > 0 ) {
            for ( auto rit = order_book.rbegin(); rit != order_book.rend(); rit++) {
                if ( rit->owner == name_ ) {
                    flow_completion_time = rit->time;
                    break;
                }
            }
        }
        if (DEBUG_PRINT)
            cout << "flow_completion_time " << flow_completion_time << endl;

        if (DEBUG_PRINT)
            cout << name_ << " is buying slots: ";

        for ( size_t idx : pick_n_slots_to_buy( order_book, num_packets_to_buy, flow_completion_time ) )
        {
            if (DEBUG_PRINT)
                cout << idx << ", ";

            const double slot_cost = order_book.at( idx ).best_offer().cost;
            mkt.add_bid_to_slot( idx, { slot_cost, name_ } );

            flow_completion_time = max( order_book.at( idx ).time, flow_completion_time );// might not be necessary
            money_spent_ += slot_cost; // assumes bid worked
            assert( order_book.at( idx ).owner == name_ ); // assert we succesfully got it
        }

        double new_expected_utility = - (double) (flow_completion_time - flow_start_time_) - money_spent_ + money_earned( mkt.money_exchanged(), name_ );
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

    if ( num_order_book_slots_owned > 0 or num_packets_to_buy > 0) {
        price_owned_slots( mkt );
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
