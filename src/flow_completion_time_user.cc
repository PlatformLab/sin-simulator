/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "flow_completion_time_user.hh" 

using namespace std;

template <typename T>
static double money_earned( const T &collection, const size_t &uid )
{
    double toRet = 0;
    for ( auto &item : collection )
    {
        if ( item.to == uid ) {
            toRet += item.amount;
        }
    }
    return toRet;
}

/* Returns 0 if name owns no slots in collection */
template <typename T>
static size_t last_slot_time( const T &collection, const size_t &uid )
{
    size_t last_slot_time = 0;

    for ( auto rit = collection.rbegin(); rit != collection.rend(); rit++ ) {
        if ( rit->owner == uid ) {
            last_slot_time = rit->time;
            break;
        }
    }
    return last_slot_time;
}

FlowCompletionTimeUser::FlowCompletionTimeUser( const size_t &uid, const size_t flow_start_time, const size_t num_packets )
: AbstractUser( uid ),
    flow_start_time_( flow_start_time ),
    flow_num_packets_( num_packets )
{
}

/* Returns the benefit score for a given flow completion time */
double FlowCompletionTimeUser::get_benefit(size_t flow_completion_time) const
{
    //assert( flow_completion_time >= flow_start_time_ );
    return - (double) ( flow_completion_time + 1 - flow_start_time_ );
}

inline bool FlowCompletionTimeUser::can_buy(const SingleSlot &slot) const
{
    return slot.owner != uid_ and slot.has_offers();
}

/* Price slot to increase the overall utility by .01 if it was sold and the best replacement was bought. */
double FlowCompletionTimeUser::get_sell_price( const deque<SingleSlot> &order_book, const size_t last_slot_time, const double current_benefit ) const
{
    size_t replacement_idx = pick_replacement_slot( order_book, last_slot_time );

    double benefit_with_replacement = get_benefit( max( last_slot_time, order_book.at( replacement_idx ).time ) );

    double benefit_delta = benefit_with_replacement - current_benefit;

    double utility_delta = benefit_delta - order_book.at( replacement_idx ).best_offer().cost;

    return .01 - utility_delta;
}

void FlowCompletionTimeUser::price_owned_slots( Market &mkt )
{
    auto &order_book = mkt.order_book();
    assert ( mkt.num_owned_in_order_book(uid_) + mkt.num_owned_in_packets_sent(uid_) == flow_num_packets_ );

    size_t last_owned_slot_time = last_slot_time( order_book, uid_ );
    double current_benefit = get_benefit( last_owned_slot_time );

    double sell_price = -1;

    size_t last_owned_slot_idx = last_owned_slot_time - order_book.front().time;
    assert( order_book.at( last_owned_slot_idx ).time == last_owned_slot_time );
    for ( size_t idx = 0; idx <= last_owned_slot_idx; idx++ ) {
        const SingleSlot &slot = order_book.at( idx );
        if ( slot.owner == uid_ ) {
            if ( slot.time == last_owned_slot_time ) {
                /* price last packet differently, as the last_owned_slot_time if we sold this slot would
                   be the time of the second to last slot */
                   last_owned_slot_time = order_book.front().time;
                   for ( int i = idx - 1; i >= 0; i-- ) {
                       if ( order_book.at(i).owner == uid_ ) {
                           last_owned_slot_time = order_book.at(i).time;
                           break;
                       }
                   }
                   sell_price = get_sell_price( order_book, last_owned_slot_time, current_benefit );
            } else {
                /* haven't set sell price yet */
                if ( sell_price < 0 ) {
                    sell_price = get_sell_price( order_book, last_owned_slot_time, current_benefit );
                }
            }

            /* only add offer to slot if it differs from existing best offer */
            if ( not slot.has_offers() or slot.best_offer().cost != sell_price ) {
                mkt.clear_offers_from_slot( idx, uid_ );
                mkt.add_offer_to_slot( idx, { sell_price, uid_ } );
            }
        }
    }
}

size_t FlowCompletionTimeUser::pick_replacement_slot( const deque<SingleSlot> &order_book, const size_t latest_time_already_owned ) const
{
    size_t best_idx = -1;
    double best_utility = std::numeric_limits<double>::lowest();

    for ( size_t idx = 0; idx < order_book.size(); idx++ ) {
        const SingleSlot &slot = order_book.at( idx );

        if ( can_buy( slot ) ) {
            size_t last_slot_time = max( slot.time, latest_time_already_owned );
            double benefit = get_benefit( last_slot_time );
            double utility = benefit - slot.best_offer().cost;

            if (utility > best_utility) {
                best_utility = utility;
                best_idx = idx;
            } else if ( best_utility > benefit ) {
                /* The benefit only gets worse as we move later, so if benefit is less than best utility,
                   it would require the sum costs of the slots purchased to be negative to be better,
                   which we assume to be impossible, so we break */
                break;
            }
        }
    }
    assert( best_idx != size_t(-1) );
    return best_idx;
}

/* fills and then keeps a priority queue of the cheapest n (=num_packets_to_buy) slots then keeps a
   copy of the set of cheapest slots with the most utility and returns that */
vector<size_t> FlowCompletionTimeUser::pick_n_slots_to_buy( const deque<SingleSlot> &order_book, const size_t num_packets_to_buy, const size_t latest_time_already_owned ) const
{
    priority_queue<pair<double, size_t>> costs_and_indices_to_buy;
    double total_cost = 0;

    priority_queue<pair<double, size_t>> best_indices;
    double best_utility = std::numeric_limits<double>::lowest();

    for ( size_t i = 0; i < order_book.size(); i++ ) {
        const SingleSlot &slot = order_book.at( i );

        if ( can_buy( slot ) ) {
            double slot_cost = slot.best_offer().cost;
            /* add to the priority queue if it is not full size yet or if the slot is cheaper than
               the most expensive slot on the priority queue */
            if ( costs_and_indices_to_buy.size() < num_packets_to_buy or slot_cost < costs_and_indices_to_buy.top().first ) {
                costs_and_indices_to_buy.push( {slot_cost, i} );
                total_cost += slot_cost;

                /* if we added a new cheaper slot and put the priority queue over the number of
                   packets we needed take the most expensive one off */
                if ( costs_and_indices_to_buy.size() > num_packets_to_buy ) {
                    total_cost -= costs_and_indices_to_buy.top().first;
                    costs_and_indices_to_buy.pop();
                    //assert( costs_and_indices_to_buy.size() == num_packets_to_buy );
                }
            }

            /* this means we have an complete possible set of slot indices to buy */
            if ( costs_and_indices_to_buy.size() == num_packets_to_buy ) {
                size_t last_slot_time = max( slot.time, latest_time_already_owned );
                double benefit = get_benefit( last_slot_time );
                double utility = benefit - total_cost;

                if (utility > best_utility) {
                    best_utility = utility;
                    best_indices = costs_and_indices_to_buy;
                } else if ( best_utility > benefit ) {
                    /* The benefit only gets worse as we move later, so if benefit is less than best utility,
                       it would require the sum costs of the slots purchased to be negative to be better,
                       which we assume to be impossible, so we break */
                    break;
                }
            }
        }
    }

    vector<size_t> toRet;
    while ( not best_indices.empty() ) {
        toRet.push_back( best_indices.top().second );
        best_indices.pop();
    }

    assert( toRet.size() == num_packets_to_buy );

    return toRet;
}

void FlowCompletionTimeUser::take_actions( Market& mkt )
{
    auto &order_book = mkt.order_book();
    /* order book empty or flow hasn't started yet */
    if ( order_book.empty() or order_book.front().time < flow_start_time_ or mkt.version() == previous_market_version_ )
    {
        return;
    }

    const size_t num_packets_sent = mkt.num_owned_in_packets_sent(uid_);
    const size_t num_order_book_slots_owned = mkt.num_owned_in_order_book(uid_);

    /* makes sure num_packets_to_buy is non-negative */
    assert ( flow_num_packets_ >= ( num_packets_sent + num_order_book_slots_owned ) );

    size_t num_packets_to_buy = flow_num_packets_ - num_packets_sent - num_order_book_slots_owned; 

    /* if we have something to buy, buy things */
    if ( num_packets_to_buy > 0 ) {
        size_t completion_time = 0;

        if ( num_order_book_slots_owned > 0 ) {
            completion_time = last_slot_time( order_book, uid_ );// TIME of last slot owned function
        }

        for ( size_t idx : pick_n_slots_to_buy( order_book, num_packets_to_buy, completion_time ) )
        {
            const double slot_cost = order_book.at( idx ).best_offer().cost;
            mkt.add_bid_to_slot( idx, { slot_cost, uid_ } );

            /* have flow completion time reflect if we bought new slots after previous flow completion time */
            completion_time = max( completion_time, order_book.at( idx ).time );
            /* assert we succesfully got it */
            assert( order_book.at( idx ).owner == uid_ );
            money_spent_ += slot_cost;
        }

        expected_utility_ = get_benefit( completion_time ) - money_spent_ + money_earned( mkt.money_exchanged(), uid_ );
        if ( expected_utility_ < best_expected_utility_ ) {
            cout << uid_ << " got swindled!" << endl;
        }
        best_expected_utility_ = max( best_expected_utility_, expected_utility_ );
    }

    /* if we own anything from before or bought anything than price all slots again */
    if ( num_order_book_slots_owned > 0 or num_packets_to_buy > 0) {
        price_owned_slots( mkt );//make offers for owned slots
    }
    previous_market_version_ = mkt.version();
}

bool FlowCompletionTimeUser::done( const Market& mkt )
{
    if ( not done_ ) {
        done_ = flow_num_packets_ == mkt.num_owned_in_packets_sent(uid_);
    }
    return done_;
}

void FlowCompletionTimeUser::print_stats( const Market& mkt ) const
{
    cout << uid_ << " had benefit " << benefit( mkt ) << ", cost " << cost( mkt ) << ", utility " << utility( mkt );
    cout << ", and best expected utility " << best_expected_utility() << endl;
}

double FlowCompletionTimeUser::cost( const Market& mkt ) const
{
    return - money_spent_ + money_earned( mkt.money_exchanged(), uid_ );
}

double FlowCompletionTimeUser::utility( const Market& mkt ) const
{
    return benefit( mkt ) + cost( mkt );

}
double FlowCompletionTimeUser::best_expected_utility() const
{
    return best_expected_utility_;
}

double FlowCompletionTimeUser::benefit( const Market& mkt ) const
{
    return get_benefit( last_slot_time( mkt.packets_sent(), uid_ ) );
}
