/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "random_evil_user.hh"
#include "pretty_print.hh"
#include <unordered_set>

using namespace std;

template <typename T>
static double net_money( const T &collection, const size_t &uid )
{
    double toRet = 0;
    for ( auto &item : collection )
    {
        if ( item.to == uid ) {
            toRet += item.amount;
        }
        if ( item.from == uid ) {
            toRet -= item.amount;
        }
    }
    return toRet;
}

bool all_other_users_gone( const Market& mkt, const size_t num_other_users )
{
    unordered_set<size_t> uids_seen;
    for ( const auto &ps : mkt.packets_sent() ) {
        uids_seen.insert( ps.owner );
    }

    for ( const auto &slot : mkt.order_book() ) {
        uids_seen.insert( slot.owner );
    }

    return uids_seen.size() == num_other_users;
}

    RandomEvilUser::RandomEvilUser( const size_t &uid )
: AbstractUser( uid )
{
}

void RandomEvilUser::take_actions( Market& mkt ) 
{
    if ( done_ or not all_other_users_gone( mkt, uid_ ) ) { // based on how we choosing uids, the uid of a single evil user will be the same as the number of other users in the market
        return;
    }

    auto &order_book = mkt.order_book();
    vector<size_t> potential_idxs;

    for ( size_t idx = 0; idx < order_book.size(); idx++ ) {
        const auto & slot = order_book.at( idx );
        if ( slot.owner != 0 and slot.has_offers() ) {
            potential_idxs.push_back( idx );
        }
    }

    if ( not potential_idxs.empty() ) {
        size_t idx_to_buy = potential_idxs.at( rand() % potential_idxs.size() );
        assert( order_book.at( idx_to_buy ).has_offers() );

        double slot_cost = order_book.at( idx_to_buy ).best_offer().cost;

        mkt.add_bid_to_slot( idx_to_buy, { order_book.at( idx_to_buy ).best_offer().cost , uid_ } );
        assert( order_book.at( idx_to_buy ).owner == uid_ );
        mkt.add_offer_to_slot( idx_to_buy, { slot_cost + .85 , uid_ } );

        done_ = true;
    }
}

bool RandomEvilUser::done( const Market& )
{
    return done_;
}

void RandomEvilUser::print_stats( const Market &mkt ) const
{
    double money = net_money( mkt.money_exchanged(), uid_ );
    cout << uid_to_string( uid_ ) << " made net money " << money << endl;
    if ( money > 0 ) {
        cout << "EVIL USER MADE MONEY!!" << endl;
    }
}
