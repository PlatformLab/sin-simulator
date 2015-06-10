/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "flow_completion_time_user.hh" 

using namespace std;

FlowCompletionTimeUser::FlowCompletionTimeUser( const std::string &name, const size_t flow_start_time, const size_t num_packets )
: AbstractUser( name ),
    flow_start_time_( flow_start_time ),
    num_packets_( num_packets )
{
}

static pair<priority_queue<const SingleSlot &>, double> slots_to_buy( const deque<SingleSlot> &order_book, const string &name, size_t start_time, size_t num_packets_to_send, size_t num_packets_already_sent, const size_t latest_time_already_owned )
{
    priority_queue<const SingleSlot &> slots_to_buy;
    double slots_to_buy_cost = 0;

    size_t idx = start_time < order_book.front().time ? 0 : order_book.front().time - start_time;
    while  (slots_to_buy.size() == num_packets_to_send) {
        const SingleSlot &potential_slot = order_book.at( idx );
        bool can_buy = potential_slot.owner != name and not potential_slot.offers.empty();
        if ( can_buy ) {
            slots_to_buy.insert(potential_slot);
            slots_to_buy += potential_slot.best_offer().cost;
        }
        idx++;
        assert(idx < order_book.size() && "can't buy slots we need");
    }

    idx--; // we incremented 1 too many times in while loop
    size_t min_flow_completion_time = max(order_book.at( idx ).time, latest_time_already_owned);

    priority_queue<const SingleSlot &> best_slots = slots_to_buy;
    double flow_benefit = -(min_flow_completion_time - start_time);
    double best_utility = flow_benefit - slots_to_buy_cost;

    for (int i = idx + 1; i < order_book.size(); i++) {
        const SingleSlot &potential_slot = order_book.at( i );
        bool can_buy = potential_slot.owner != name and not potential_slot.offers.empty();
        if ( can_buy ) {
            slots_to_buy.insert(potential_slot);
            slots_to_buy_cost += potential_slot.best_offer().cost;

            slots_to_buy_cost -= slots_to_buy.top().best_offer().cost;
            slots_to_buy.top().pop();
            assert(slots_to_buy.size() == num_packets_to_send);

            double current_benefit = -max( i, latest_time_already_owned ) - start_time;
            double current_utility = current_benefit - slots_to_buy_cost;

            if (current_utility > best_utility) {
                best_slots = slots_to_buy;
                best_utility = current_utility;
            }
        }
    }
    return std::make_pair(best_slots, best_utility);
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

template <typename T>
static size_t idx_of_time(size_t time, const T &order_book) {
    for (size_t i = 0; i < order_book.size(); i++) {
        if (order_book.at(i).time == time) {
            return i;
        }
    }
    assert(false && "time not in order book");
    return -1;
}

void BruteForceUser::makeOffersForOwnedSlots( Market& mkt )
{
    auto &order_book = mkt.order_book();
    if (times_owned( order_book, name_ ).empty()) {
        return;
    }

    size_t num_packets_already_sent = times_owned( mkt.packets_sent(), name_ ).size();
    /*
    cout << name_ << " got num packets sent " << num_packets_already_sent  << endl;
    printPacketsSent(mkt.packets_sent());
    cout << "for these packets sent " << endl;
    cout << "and order book is " << endl;
    printOrderBook(order_book);
    cout << "()()()()() " << endl;
    */
    const auto cur_times_owned = times_owned( order_book, name_ );
    const double cur_utility = utility_func_( cur_times_owned, flow_start_time_, num_packets_already_sent );
    for ( size_t time : times_owned( order_book, name_) ) {
        mkt.clear_offers_from_slot(idx_of_time(time, order_book), name_);

        auto times_owned_without_a_slot(cur_times_owned);
        size_t before_size = times_owned_without_a_slot.size();
        times_owned_without_a_slot.remove_if( [time] ( size_t elem ){ return elem == time; } );
        assert(times_owned_without_a_slot.size() == before_size - 1);

        auto best_backup_slot = best_slots( order_book, name_, times_owned_without_a_slot, flow_start_time_, 1, num_packets_already_sent, utility_func_ );
        if (best_backup_slot.first.size() != 1)
        {
            cout << name_ << " failed to make offers for slot at time " << time << " got size = " << best_backup_slot.first.size() << endl;
            cout << "times_owned_without_a_slot was [ ";
            for ( auto i : times_owned_without_a_slot )
                cout << i << ", ";
            cout <<  "]" << endl;
            cout << " num packets already sent " << num_packets_already_sent << " flow start time " << flow_start_time_ << endl;

            assert( best_backup_slot.first.size() == 1 && "no other slots to buy" );
        }
        //cout << "cur utility is " << cur_utility << " and utility to buy next best slot (idx " << best_backup_slot.first.front() << ") if sold " << time << " is " << best_backup_slot.second  << endl;
        double utility_delta_to_move_slots = best_backup_slot.second - cur_utility;
        /*
        if (utility_delta_to_move_slots > 0)
        {
            size_t idx_to_buy = best_backup_slot.first.front();
            mkt.add_bid_to_slot( idx_to_buy, { order_book.at( idx_to_buy ).best_offer().cost, name_ } );
            cout << name_ << " should just move slots to " <<  idx_to_buy << endl;
            makeOffersForOwnedSlots( mkt ); // start over
            return;
        }
        */
        utility_delta_to_move_slots = min ( 0., utility_delta_to_move_slots );
        //cout << " adding offer of $" << ((double) -utility_delta_to_move_slots ) + .01 << " to slot at time " << time << endl;
        mkt.add_offer_to_slot( idx_of_time(time, order_book) , { ( (double) -utility_delta_to_move_slots ) + .01, name_ } );
    }
    //cout << " DONE adding offers" << endl;
}

void BruteForceUser::take_actions( Market& mkt )
{
    auto &order_book = mkt.order_book();
    if ( order_book.empty() or order_book.front().time < flow_start_time_)
    {
        return;
    }

    assert( not times_owned( order_book, "ccst" ).empty() && " HACK: no owner slots left");
    size_t num_packets_sent = times_owned( mkt.packets_sent(), name_ ).size();
    size_t num_order_book_slots_owned = times_owned( order_book, name_ ).size();
    assert (num_packets_ >= ( num_packets_sent + num_order_book_slots_owned ));
    size_t num_packets_to_buy = num_packets_ - num_packets_sent - num_order_book_slots_owned; 
    if (num_packets_to_buy > 0) {
        pair<priority_queue<const SingleSlot &>, double> slots_to_buy = slots_to_buy( const deque<SingleSlot> &order_book, const string &name, size_t start_time, size_t num_packets_to_send, size_t num_packets_already_sent, const size_t latest_time_already_owned );
        for (auto slot : slots_to_buy.first) {
            mkt.add_bid_to_slot( slot, { slot.best_offer().cost, name_ } );
            //assert got it
        }

        double current_utility = slots_to_buy.second();
        for (auto slot : slots_to_buy.first) {
            double utility_delta = current_utility - slots_to_buy( ).second;
            utility_delta + .01;
            make offer for it based on slots_to_buy( 1 slot, latest time );
        }
    }
    makeOffersForOwnedSlots( mkt );
}

bool BruteForceUser::done( const Market& mkt ) const
{
    //    cout << "num packets for " << name_ << " is " << num_packets_ << " and have sent " << num_owned_in_deque( mkt.packets_sent(), name_ ) << endl;
    return num_packets_ == times_owned( mkt.packets_sent(), name_ ).size();
}

void BruteForceUser::print_stats( const Market& ) const
{
        cout << "user " << name_ << " started at time " << flow_start_time_ << " and finished at time "
        <<  endl; // need to track money spent
}
