/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "brute_force_user.hh" 

using namespace std;

BruteForceUser::BruteForceUser( const std::string &name, const size_t flow_start_time, const size_t num_packets, std::function<int(const std::list<size_t>&, size_t, size_t)> utility_func )
: AbstractUser( name ),
    flow_start_time_( flow_start_time ),
    num_packets_( num_packets ),
    utility_func_( utility_func )
{
}

static vector<vector<size_t>> potential_idxs( const deque<SingleSlot> &order_book, const string &name, const size_t start_idx, const size_t num_packets )
{
    if ( num_packets == 0 )
        return {{}};

    vector<vector<size_t>> toRet = {};
    for ( size_t i = start_idx; i < order_book.size(); i++ ) {
        bool can_buy = order_book.at( i ).owner != name and order_book.at( i ).has_offers();
        if ( can_buy ) {
            for ( vector<size_t> &vec : potential_idxs( order_book, name, i+1, num_packets-1 ) ) {
                vec.emplace_back( i );
                toRet.emplace_back( vec );
            }
        }
    }
    return toRet;
}


template <typename T>
static double cost_of_slots( const deque<SingleSlot> &order_book, const T &idxs )
{
    double toRet = 0;
    for ( size_t i : idxs ) {
        toRet += order_book.at( i ).best_offer().cost;
    }
    return toRet;
}


// returns a list of best idxs and the utility - cost to get those slots
static pair<vector<size_t>, double> best_slots( const deque<SingleSlot> &order_book, const string &name, const list<size_t> times_already_owned, size_t start_time, size_t num_packets, size_t num_packets_already_sent, function<int(const list<size_t>&, size_t, size_t)> utility_func )
{
    vector<size_t> best_idxs = {};
    double max_net_utility = std::numeric_limits<double>::lowest();
    size_t start_idx = start_time < order_book.front().time ? 0 : order_book.front().time - start_time;
    for ( vector<size_t> &idxs_could_buy : potential_idxs( order_book, name, start_idx, num_packets ) )
    {
        assert(not idxs_could_buy.empty());
        list<size_t> times_would_own_if_bought_idxs_could_buy = times_already_owned;
        for (auto idx : idxs_could_buy) {
            times_would_own_if_bought_idxs_could_buy.push_back(order_book.at(idx).time);
        }
        double net_utility = utility_func( times_would_own_if_bought_idxs_could_buy, start_time, num_packets_already_sent ) - cost_of_slots( order_book, idxs_could_buy );

        if ( net_utility > max_net_utility ) {
            max_net_utility = net_utility;
            best_idxs = idxs_could_buy;
        }
    }
    /*
    if (best_idxs.size() != num_packets)
        cout <<"trouble, best_idxs size " << best_idxs.size() << " while num packets " << num_packets << endl;
        */

    assert(best_idxs.size() == num_packets);
    return make_pair( best_idxs, max_net_utility );
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
        vector<size_t> best_idxs = best_slots( order_book, name_, times_owned( order_book, name_ ), flow_start_time_, num_packets_to_buy, num_packets_sent, utility_func_ ).first;

        //cout << name_ << " buying slots ";
        for ( size_t i : best_idxs ) {
            //cout << i << ", ";
            mkt.add_bid_to_slot( i, { order_book.at( i ).best_offer().cost, name_ } );
        }
        //cout << endl;
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
