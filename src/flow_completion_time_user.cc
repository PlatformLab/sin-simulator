/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "flow_completion_time_user.hh" 

using namespace std;

FlowCompletionTimeUser::FlowCompletionTimeUser( const std::string &name, const size_t flow_start_time, const size_t num_packets )
: AbstractUser( name ),
    flow_start_time_( flow_start_time ),
    num_packets_( num_packets )
{
}

static vector<size_t> idxs_to_buy( const deque<SingleSlot> &order_book, const string &name, size_t start_time, size_t num_packets_to_buy, const size_t latest_time_already_owned )
{
    priority_queue<size_t> idxs_to_buy {};
    double idxs_to_buy_cost = 0;

    size_t idx = start_time < order_book.front().time ? 0 : order_book.front().time - start_time;
    while  (idxs_to_buy.size() == num_packets_to_buy) {
        const SingleSlot &potential_slot = order_book.at( idx );
        bool can_buy = potential_slot.owner != name and not potential_slot.offers.empty();
        if ( can_buy ) {
            idxs_to_buy.push( idx );
            idxs_to_buy_cost += potential_slot.best_offer().cost;
        }
        idx++;
        assert(idx < order_book.size() && "can't buy slots we need");
    }

    idx--; // we incremented 1 too many times in while loop
    size_t min_flow_completion_time = max(order_book.at( idx ).time, latest_time_already_owned);

    priority_queue<size_t> best_idxs = idxs_to_buy;
    double flow_benefit = -(min_flow_completion_time - start_time);
    double best_utility = flow_benefit - idxs_to_buy_cost;

    for (size_t i = idx + 1; i < order_book.size(); i++) {
        const SingleSlot &potential_slot = order_book.at( i );
        bool can_buy = potential_slot.owner != name and not potential_slot.offers.empty();
        if ( can_buy ) {
            idxs_to_buy.push( i );
            idxs_to_buy_cost += potential_slot.best_offer().cost;

            idxs_to_buy_cost -= order_book.at( idxs_to_buy.top() ).best_offer().cost;
            idxs_to_buy.pop();
            assert(idxs_to_buy.size() == num_packets_to_buy);

            double current_benefit = -max( i, latest_time_already_owned ) - start_time;
            double current_utility = current_benefit - idxs_to_buy_cost;

            if (current_utility > best_utility) {
                best_idxs = idxs_to_buy;
                best_utility = current_utility;
            }
        }
    }
    return {};//std::make_pair(best_idxs, //best_utility);
}

template <typename T>
static list<size_t> times_owned( const T &collection, const string &name )
{
    list<size_t> toRet {};
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

    list<size_t> order_book_slots_owned =  times_owned( order_book, name_ );
    size_t num_order_book_slots_owned = order_book_slots_owned.size();

    assert (num_packets_ >= ( num_packets_sent + num_order_book_slots_owned ));

    size_t num_packets_to_buy = num_packets_ - num_packets_sent - num_order_book_slots_owned; 

    if (num_packets_to_buy > 0) {
        size_t latest_time_already_owned = order_book_slots_owned.back();
        auto buying_slots = idxs_to_buy( order_book, name_, flow_start_time_, num_packets_to_buy, latest_time_already_owned );

        for (auto &idx : buying_slots ) {
            
            mkt.add_bid_to_slot( idx, { order_book.at(idx).best_offer().cost, name_ } );
            //assert got it
        }

        list<size_t> order_book_slots_owned =  times_owned( order_book, name_ ); // redo calc
        double current_utility = 0; //TODO figure it out pair.second;

        // TODO, all slots but last one will be priced same so only do twice
        for ( auto idx_to_price : buying_slots ) {
            // first get latest time owned that isnt this slot
            size_t latest_time_owned_for_pricing = order_book_slots_owned.front(); // TODO what if only own one slot
            for (const auto & slot : order_book) {
                //XXX temp if ( slot.owner == name_ and slot != slot_to_price ) {
                    latest_time_owned_for_pricing = slot.time;
                //}
            }

            num_packets_to_buy = 1;
            latest_time_owned_for_pricing --; // TODO temp
            double utility_delta = current_utility - 0;//TODO urrent_utility - idxs_to_buy( order_book, name_, flow_start_time_, num_packets_to_buy, num_packets_sent, latest_time_owned_for_pricing ).second;

            mkt.add_offer_to_slot( idx_to_price, { utility_delta + .01, name_ } );
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
