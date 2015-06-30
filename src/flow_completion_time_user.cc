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
double FlowCompletionTimeUser::get_benefit( priority_queue<pair<double, size_t>> costs_and_indices_to_buy /* copy needed for now */ ) const
{
    /* assume priority queue is size of num packets we need to buy */
    assert( not costs_and_indices_to_buy.empty() );

    size_t latest_time = 0;
    while ( not costs_and_indices_to_buy.empty() )
    {
        latest_time = max( latest_time, costs_and_indices_to_buy.top().second );
        costs_and_indices_to_buy.pop();
    }

    return - (double) ( latest_time - flow_start_time_ + 1 );
}

inline bool FlowCompletionTimeUser::can_use(const SingleSlot &slot) const
{
    return slot.owner == uid_ or slot.has_offers();
}

/* fills and then keeps a priority queue of the cheapest n (=num_packets_to_buy) slots then keeps a
   copy of the set of cheapest slots with the most utility and returns that */
   // returns priority queue of packets to buy and the utility of this set of packets
priority_queue<pair<double, size_t>> FlowCompletionTimeUser::pick_n_slots_to_buy( const deque<SingleSlot> &order_book, const size_t num_packets_to_buy, const size_t time_to_ignore ) const
{
    priority_queue<pair<double, size_t>> costs_and_indices_to_buy;
    double total_cost = 0;

    priority_queue<pair<double, size_t>> best_indices;
    double best_utility = std::numeric_limits<double>::lowest();

    for ( size_t i = 0; i < order_book.size(); i++ ) {
        const SingleSlot &slot = order_book.at( i );

        if ( slot.time != time_to_ignore and can_use( slot ) ) {
            double slot_cost = slot.owner == uid_ ? 0 : slot.best_offer().cost;
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
                double benefit = get_benefit( costs_and_indices_to_buy );
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

    assert( best_indices.size() == num_packets_to_buy );

    return best_indices;
}

void FlowCompletionTimeUser::take_actions( Market& mkt )
{
    auto &order_book = mkt.order_book();
    /* order book empty or flow hasn't started yet */
    if ( order_book.empty() or order_book.front().time < flow_start_time_ or mkt.version() == previous_market_version_ )
    {
        return;
    }

    const size_t num_need_in_order_book = flow_num_packets_ - mkt.num_owned_in_packets_sent(uid_);
    if (num_need_in_order_book == 0) {
        done_ = true;
    } else {
       priority_queue<pair<double, size_t>> to_buy =  pick_n_slots_to_buy( order_book, num_need_in_order_book, size_t( -1 ) );

       double benefit = get_benefit( to_buy );
       while (not to_buy.empty() ) {
           const size_t idx = to_buy.top().second;
           if ( order_book.at( idx ).owner != uid_ ) { // maybe make owns( slot ) function
               assert( to_buy.top().first == order_book.at( idx ).best_offer().cost );
               const double slot_cost = to_buy.top().first;
               mkt.add_bid_to_slot( idx, { slot_cost, uid_ } );
               assert( order_book.at( idx ).owner == uid_ ); // asssert succesfully got it
               money_spent_ += slot_cost;
           }
           to_buy.pop();
       }

        expected_utility_ = benefit - money_spent_ + money_earned( mkt.money_exchanged(), uid_ );
        /*
        if ( expected_utility_ < best_expected_utility_ ) {
            cout << uid_ << " got swindled!" << endl;
        }
         */
        best_expected_utility_ = max( best_expected_utility_, expected_utility_ );

        /* if we own anything from before or bought anything than price all slots again */
        for ( size_t idx = 0; idx < order_book.size(); idx++ ) {
            const SingleSlot &slot = order_book.at( idx );
            if ( slot.owner == uid_ ) {
                priority_queue<pair<double, size_t>> slots_to_buy_if_slot_sold = pick_n_slots_to_buy( order_book, num_need_in_order_book, slot.time );
                double benefit_delta = get_benefit ( slots_to_buy_if_slot_sold ) - (double) benefit;
                double cost_delta = 0;
                while ( not slots_to_buy_if_slot_sold.empty() ) {
                    cost_delta -= slots_to_buy_if_slot_sold.top().first;
                    slots_to_buy_if_slot_sold.pop();
                }
                double sell_price = - benefit_delta - cost_delta + .01;
                cout << uid_to_string(uid_) << " benefit delta for slot at time " << order_book.at(idx).time << " is " << benefit_delta << " while cost delta is " << cost_delta << " so sell price will be " << sell_price << endl;

                /* only add offer to slot if it differs from existing best offer */
                if ( not slot.has_offers() or slot.best_offer().cost != sell_price ) {
                    mkt.clear_offers_from_slot( idx, uid_ );
                    mkt.add_offer_to_slot( idx, { sell_price, uid_ } );
                }
            }
        }
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
    assert( done_ ); // only call this at the end
    size_t num_sent = 0;
    for( auto &packet_sent : mkt.packets_sent() ) {
        if ( packet_sent.owner == uid_ ) {
            num_sent++;
            if ( num_sent == flow_num_packets_ ) {
                return -(double) ( packet_sent.time - flow_start_time_ + 1 );
            }
        }
    }
    return numeric_limits<double>::lowest();
}
