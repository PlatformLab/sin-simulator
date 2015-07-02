/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "flow_completion_time_user.hh" 
#include <cmath>

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

inline double get_benefit( const size_t flow_completion_time, const size_t flow_start_time ) {
    assert( flow_completion_time >= flow_start_time );
    return - (double) ( flow_completion_time - flow_start_time + 1 );
}

/* Returns the benefit score for a given flow completion time */
inline size_t latest_idx( const vector<pair<double, size_t>> &costs_and_indices_to_buy )
{
    /* assume priority queue is size of num packets we need to buy */
    assert( not costs_and_indices_to_buy.empty() );

    size_t latest_idx = 0;
    for ( auto &buy_pair : costs_and_indices_to_buy ) {
        latest_idx = max( latest_idx, buy_pair.second );
    }

    return latest_idx;
}

inline bool FlowCompletionTimeUser::can_use(const SingleSlot &slot) const
{
    return slot.owner == uid_ or slot.has_offers();
}

/* fills and then keeps a priority queue of the cheapest n (=num_packets_to_buy) slots then keeps a
   copy of the set of cheapest slots with the most utility and returns that */
   // returns priority queue of packets to buy and the utility of this set of packets
pair<vector<pair<double, size_t>>, double> FlowCompletionTimeUser::pick_n_slots_to_buy( const deque<SingleSlot> &order_book, const size_t num_packets_to_buy, const size_t time_to_ignore, const bool fill_vector ) const
{
    vector<pair<double, size_t>> costs_and_indices_to_buy;
    double total_cost = 0;

    vector<pair<double, size_t>> best_indices;
    double best_utility = std::numeric_limits<double>::lowest();
    double benefit = std::numeric_limits<double>::lowest();

    for ( size_t i = 0; i < order_book.size(); i++ ) {
        const SingleSlot &slot = order_book.at( i );

        if ( slot.time != time_to_ignore and can_use( slot ) ) {
            double slot_cost = slot.owner == uid_ ? 0 : slot.best_offer().cost;
            /* add to the priority queue if it is not full size yet or if the slot is cheaper than
               the most expensive slot on the priority queue */
            if ( costs_and_indices_to_buy.size() < num_packets_to_buy or slot_cost < costs_and_indices_to_buy.front().first ) {
                costs_and_indices_to_buy.push_back( {slot_cost, i} );
                push_heap( costs_and_indices_to_buy.begin(), costs_and_indices_to_buy.end() );
                //assert( is_heap( costs_and_indices_to_buy.begin(), costs_and_indices_to_buy.end() ) );
                total_cost += slot_cost;
                benefit = get_benefit( slot.time, flow_start_time_ );

                /* if we added a new cheaper slot and put the priority queue over the number of
                   packets we needed take the most expensive one off */
                if ( costs_and_indices_to_buy.size() > num_packets_to_buy ) {
                    total_cost -= costs_and_indices_to_buy.front().first;
                    pop_heap( costs_and_indices_to_buy.begin(), costs_and_indices_to_buy.end() );
                    costs_and_indices_to_buy.pop_back();
                    //assert( is_heap( costs_and_indices_to_buy.begin(), costs_and_indices_to_buy.end() ) );
                    assert( costs_and_indices_to_buy.size() == num_packets_to_buy );
                }
            }

            /* this means we have an complete possible set of slot indices to buy */
            if ( costs_and_indices_to_buy.size() == num_packets_to_buy ) {
                double utility = benefit - total_cost;

                if (utility > best_utility) {
                    best_utility = utility;
                    if ( fill_vector ) {
                        best_indices = costs_and_indices_to_buy;
                    }
                } else if ( best_utility > benefit ) {
                    /* The benefit only gets worse as we move later, so if benefit is less than best utility,
                       it would require the sum costs of the slots purchased to be negative to be better,
                       which we assume to be impossible, so we break */
                    break;
                }
            }
        }
    }

    assert( not fill_vector or best_indices.size() == num_packets_to_buy );

    return { best_indices, best_utility };
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
       auto got = pick_n_slots_to_buy( order_book, num_need_in_order_book, size_t( -1 ), true );
       vector<pair<double, size_t>> &to_buy = got.first;

       double benefit = get_benefit( order_book.at( latest_idx( to_buy ) ).time, flow_start_time_ );
       double greg_benefit = got.second;
       for ( auto &buy_pair : to_buy ) {
           const size_t idx = buy_pair.second;
           if ( order_book.at( idx ).owner != uid_ ) { // maybe make owns( slot ) function
               assert( buy_pair.first == order_book.at( idx ).best_offer().cost );
               const double slot_cost = buy_pair.first;
               mkt.add_bid_to_slot( idx, { slot_cost, uid_ } );
               assert( order_book.at( idx ).owner == uid_ ); // asssert succesfully got it
               money_spent_ += slot_cost;
               greg_benefit += slot_cost;
           }
       }

       assert(abs(benefit - greg_benefit) < .01);
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
               double utility_delta = pick_n_slots_to_buy( order_book, num_need_in_order_book, slot.time, false ).second - benefit;
               double sell_price = -utility_delta + .01;

                /* only add offer to slot if it differs from existing best offer */
                if ( not slot.has_offers() or abs( slot.best_offer().cost - sell_price ) > 1e-9 ) {
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
