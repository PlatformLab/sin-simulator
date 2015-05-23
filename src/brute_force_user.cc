/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "brute_force_user.hh" 

using namespace std;

BruteForceUser::BruteForceUser( const std::string &name, const size_t flow_start_time, const size_t num_packets, std::function<int(std::list<size_t>&, size_t)> utility_func )
: AbstractUser( name ),
    flow_start_time_( flow_start_time ),
    num_packets_( num_packets ),
    utility_func_( utility_func )
{
}

static list<list<size_t>> potential_idxs( const deque<SingleSlot> &order_book, const string &name, size_t start, size_t num_packets )
{
    if ( num_packets == 0 )
        return {{}};

    list<list<size_t>> toRet = {};
    for ( size_t i = start; i < order_book.size(); i++ ) {
        if ( order_book.at( i ).owner != name and not order_book.at( i ).offers.empty()) {
            for ( list<size_t> &vec : potential_idxs( order_book, name, i+1, num_packets-1 ) ) {
                vec.emplace_front( i );
                toRet.emplace_back( vec );
            }
        }
    }
    return toRet;
}

static double cost_of_slots( const deque<SingleSlot> &order_book, const list<size_t> &idxs )
{
    double toRet = 0;
    for ( size_t i : idxs ) {
        toRet += order_book.at( i ).best_offer().cost;
    }
    return toRet;
}


// returns a list of best idxs and the utility - cost to get those slots
static pair<list<size_t>, double> best_slots( const deque<SingleSlot> &order_book, const string &name, const list<size_t> idxs_for_utility_func, size_t start, size_t num_packets, function<int(list<size_t>&, size_t)> utility_func )
{
    list<size_t> best_idxs = {};
    double max_net_utility = std::numeric_limits<double>::lowest();
    for ( list<size_t> &idxs : potential_idxs( order_book, name, start, num_packets ) )
    {
        list<size_t> combined_idxs_for_utility_func = idxs_for_utility_func;
        combined_idxs_for_utility_func.insert( combined_idxs_for_utility_func.end(), idxs.begin(), idxs.end() );
        double net_utility = utility_func( combined_idxs_for_utility_func, start ) - cost_of_slots( order_book, idxs );
        /*
        cout << "utility for [";
        for ( size_t idx : combined_idxs_for_utility_func ) {
            cout << " " << idx << ",";
        }
        cout << "]" << " is " << utility_func( combined_idxs_for_utility_func, start ) << " and cost of slots is " <<
        cost_of_slots( order_book, idxs )<<  endl;
        */

        if ( net_utility > max_net_utility ) {
            max_net_utility = net_utility;
            best_idxs = idxs;
        }
    }
    return make_pair( best_idxs, max_net_utility );
}

template <typename T>
static size_t num_owned_in_deque( const deque<T> deque, const string &name )
{
    size_t toRet = 0;
    for ( const T &t : deque ) {
        if ( t.owner == name ) {
            toRet++;
        }
    }
    return toRet;
}

static list<size_t> idxs_owned( deque<SingleSlot> order_book, const string &name )
{
    list<size_t> toRet {};
    for ( size_t i = 0; i < order_book.size(); i++ )
    {
        if ( order_book.at( i ).owner == name ) {
            toRet.emplace_back( i );
        }
    }
    return toRet;
}

void BruteForceUser::take_actions( Market& mkt )
{
    const deque<SingleSlot> &order_book = mkt.order_book();
    if ( order_book.empty() )
        return;

    size_t num_packets_to_buy = num_packets_ - num_owned_in_deque( mkt.packets_sent(), name_ ) - idxs_owned( order_book, name_ ).size(); 

    list<size_t> best_idxs = best_slots( order_book, name_, idxs_owned( order_book, name_ ), flow_start_time_, num_packets_to_buy, utility_func_ ).first;

    //cout << name_ << " buying slots ";
    for ( size_t i : best_idxs ) {
    //    cout << i << ", ";
        mkt.add_bid_to_slot( i, { order_book.at( i ).best_offer().cost, name_ } );
    }
   // cout << endl;

    //cout << name_ << " making offers for slots:" << endl;
    for ( size_t i : idxs_owned( order_book, name_) ) {
        if ( not order_book.at( i ).has_offers() ) //TODO this hack
        {
            auto cur_idxs_owned = idxs_owned( order_book, name_ );
            double cur_utility = utility_func_( cur_idxs_owned, flow_start_time_ );
            //cout << "old_utility " << old_utility  << endl;
            cur_idxs_owned.remove_if( [i] ( size_t elem ){ return elem == i; } );

            auto best_backup_slot = best_slots( order_book, name_, cur_idxs_owned, flow_start_time_, 1, utility_func_ );
            assert( best_backup_slot.first.size() == 1 );
            double utility_delta_to_move_slots = best_backup_slot.second - cur_utility;
            //cout << "best backup " << best_backup_slot.first.front() << " and util delta " << utility_delta_to_move_slots << endl;
            //assert( utility_delta_to_move_slots <= 0 );
   //         cout << "pricing slot " << i << " at " <<( -utility_delta_to_move_slots ) + 1 << endl; 
            mkt.add_offer_to_slot( i , { ( (double) -utility_delta_to_move_slots ) + .01, name_ } );
        } else {
  //          cout << "not adding offers to slot " << i << endl;
        }
    }
 //   cout << "done making offers for slots." << endl;
}

bool BruteForceUser::done( const Market& mkt ) const
{
//    cout << "num packets for " << name_ << " is " << num_packets_ << " and have sent " << num_owned_in_deque( mkt.packets_sent(), name_ ) << endl;
    return num_packets_ == num_owned_in_deque( mkt.packets_sent(), name_ );
}

void BruteForceUser::print_stats( const Market& ) const
{
        cout << "user " << name_ << " started at time " << flow_start_time_ << " and finished at time "
        <<  endl; // need to track money spent
}
