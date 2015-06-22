/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "flow_completion_time_user.hh" 

using namespace std;

template <typename T>
static double money_earned( const T &collection, const string &name )
{
    double toRet = 0;
    for ( auto &item : collection )
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
    for ( auto &item : collection )
    {
        if ( item.owner == name ) {
            toRet++;
        }
    }
    return toRet;
}

/* Returns 0 if name owns no slots in collection */
template <typename T>
static size_t last_packet_time( const T &collection, const string &name )
{
    size_t last_packet_time = 0;

    for ( auto rit = collection.rbegin(); rit != collection.rend(); rit++ ) {
        if ( rit->owner == name ) {
            last_packet_time = rit->time;
            break;
        }
    }
    return last_packet_time;
}

FlowCompletionTimeUser::FlowCompletionTimeUser( const std::string &name, const size_t flow_start_time, const size_t num_packets )
: AbstractUser( name ),
    flow_start_time_( flow_start_time ),
    flow_num_packets_( num_packets )
{
}

/* Returns the benefit score for a given flow completion time */
double FlowCompletionTimeUser::get_benefit(size_t flow_completion_time) const
{
    assert( flow_completion_time >= flow_start_time_ );
    return - (double) ( flow_completion_time + 1 - flow_start_time_ );
}

bool FlowCompletionTimeUser::can_buy(const SingleSlot &slot) const
{
    return slot.owner != name_ and slot.has_offers();
}

/* Slots are priced to increase the overall utility by .01 if they were sold and the best
   replacement was bought. */
void FlowCompletionTimeUser::price_owned_slots( Market &mkt )
{/* split up so if there is only 1 owned slots we dont calculate non-back replacement */
    auto &order_book = mkt.order_book();
    assert ( num_owned( order_book, name_ ) + num_owned( mkt.packets_sent(), name_ ) == flow_num_packets_ );
    //bool price_non_back_differently = num_owned( order_book, name_ ) > 1;

    size_t last_packet_time = 0;
    size_t last_packet_time_if_sold_back = 0;

    /* first we find the flow completion time and the flow completion time not including the last
       owned slot, which is used for pricing that slot */
    for ( auto rit = order_book.rbegin(); rit != order_book.rend(); rit++ ) {
        if ( rit->owner == name_ ) {
            if ( last_packet_time == 0 ) {
                last_packet_time = rit->time;
            } else {
                last_packet_time_if_sold_back = rit->time;
                break;
            }
        }
    }

    /* there are two prices, one for the last slot, for which selling can reduce the overall flow
       completion time, and another for every other slot, for which selling cannot reduce the
       overall flow completion time. For both we find index of best slot to buy instead if that slot was sold */
       /* explain better */
    size_t back_replacement_idx  = pick_n_slots_to_buy( order_book, 1, last_packet_time_if_sold_back ).front();
    size_t non_back_replacement_idx = pick_n_slots_to_buy( order_book, 1, last_packet_time ).front();

    double current_benefit = get_benefit( last_packet_time );
    double benefit_with_back_replacement = get_benefit( max( last_packet_time_if_sold_back, order_book.at( back_replacement_idx ).time ) );
    double benefit_with_non_back_replacement = get_benefit( max( last_packet_time, order_book.at( non_back_replacement_idx ).time ) );

    double back_benefit_delta = benefit_with_back_replacement - current_benefit;
    double non_back_benefit_delta = benefit_with_non_back_replacement - current_benefit;

    double back_utility_delta = back_benefit_delta - order_book.at( back_replacement_idx ).best_offer().cost;
    double non_back_utility_delta = non_back_benefit_delta - order_book.at( non_back_replacement_idx ).best_offer().cost;

    double back_sell_price = .01 - back_utility_delta;
    double non_back_sell_price = .01 - non_back_utility_delta;


    for ( size_t idx = 0; idx < order_book.size(); idx++ ) {
        const SingleSlot &slot = order_book.at( idx );
        if ( slot.owner == name_ ) {
            double slot_sell_price = slot.time == last_packet_time ? back_sell_price : non_back_sell_price;

            /* only add offer to slot if it differs from existing best offer */
            if ( not slot.has_offers() or slot.best_offer().cost != slot_sell_price ) {
                mkt.clear_offers_from_slot( idx, name_ );
                mkt.add_offer_to_slot( idx, { slot_sell_price, name_ } );
            }
        }
        if ( slot.time > last_packet_time ) {
            /* nothing left to price */
            break;
        }
    }
}

/* fills and then keeps a priority queue of the cheapest n (=num_packets_to_buy) slots then keeps a
   copy of the set of cheapest slots with the most utility and returns that */
vector<size_t> FlowCompletionTimeUser::pick_n_slots_to_buy( const deque<SingleSlot> &order_book, /* const */ size_t num_packets_to_buy, const size_t latest_time_already_owned ) const
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
                    assert( costs_and_indices_to_buy.size() == num_packets_to_buy );
                }
            }

            /* this means we have an complete possible set of slot indices to buy */
            if ( costs_and_indices_to_buy.size() == num_packets_to_buy ) {
                size_t last_packet_time = max( slot.time, latest_time_already_owned );
                double benefit = get_benefit( last_packet_time );
                double utility = benefit - total_cost;

                if (utility > best_utility) {
                    best_utility = utility;
                    best_indices = costs_and_indices_to_buy;
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
    if ( order_book.empty() or order_book.front().time < flow_start_time_)
    {
        return;
    }

    const size_t num_packets_sent = num_owned( mkt.packets_sent(), name_ );
    const size_t num_order_book_slots_owned = num_owned( order_book, name_ );

    /* makes sure num_packets_to_buy is non-negative */
    assert ( flow_num_packets_ >= ( num_packets_sent + num_order_book_slots_owned ) );

    size_t num_packets_to_buy = flow_num_packets_ - num_packets_sent - num_order_book_slots_owned; 

    /* if we have something to buy, buy things */
    if ( num_packets_to_buy > 0 ) {
        size_t completion_time = 0;

        if ( num_order_book_slots_owned > 0 ) {
            completion_time = last_packet_time( order_book, name_ );// TIME of last slot owned function
        }

        for ( size_t idx : pick_n_slots_to_buy( order_book, num_packets_to_buy, completion_time ) )
        {
            const double slot_cost = order_book.at( idx ).best_offer().cost;
            mkt.add_bid_to_slot( idx, { slot_cost, name_ } );

            /* have flow completion time reflect if we bought new slots after previous flow completion time */
            completion_time = max( completion_time, order_book.at( idx ).time );
            /* assert we succesfully got it */
            assert( order_book.at( idx ).owner == name_ );
            money_spent_ += slot_cost;
        }

        expected_utility_ = get_benefit( completion_time ) - money_spent_ + money_earned( mkt.money_exchanged(), name_ );
        best_expected_utility_ = max( best_expected_utility_, expected_utility_ );
    }

    /* if we own anything from before or bought anything than price all slots again */
    if ( num_order_book_slots_owned > 0 or num_packets_to_buy > 0) {
        price_owned_slots( mkt );//make offers for owned slots
    }
}

bool FlowCompletionTimeUser::done( const Market& mkt )
{
    if ( not done_ ) {
        done_ = flow_num_packets_ == num_owned( mkt.packets_sent(), name_ );
    }
    return done_;
}

void FlowCompletionTimeUser::print_stats( const Market& mkt ) const
{
    double benefit = get_benefit( last_packet_time( mkt.packets_sent(), name_ ) );
    double cost = - money_spent_ + money_earned( mkt.money_exchanged(), name_ );
    double utility = benefit + cost;

    cout << name_ << " has benefit " << benefit << ", cost " << cost << ", utility " << utility;
    cout << ", and best expected utility " << best_expected_utility_ << endl;
}
