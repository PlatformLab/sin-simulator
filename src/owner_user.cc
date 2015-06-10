/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "owner_user.hh"

using namespace std;

    OwnerUser::OwnerUser( const std::string &name, const double default_slot_offer, const size_t total_num_slots, const bool only_add_once )
: AbstractUser( name ),
    default_slot_offer_( default_slot_offer ),
    total_num_slots_( total_num_slots ),
    only_add_once_( only_add_once ),
    added_before_( false )
{
}

void OwnerUser::take_actions( Market& mkt ) 
{
    auto &order_book = mkt.order_book();

    if ( only_add_once_ and not added_before_ ) {
        while ( order_book.size() < total_num_slots_ ) {
            uint64_t next_time = order_book.empty() ? 0 : order_book.back().time + 1;

            mkt.owner_add_to_order_book( name_, next_time );
            mkt.add_offer_to_slot_idx( order_book.size()-1, { default_slot_offer_, name_ } );
        }
        added_before_ = true;
    }
}

bool OwnerUser::done( const Market& mkt ) const 
{
    if ( only_add_once_ ) {
        return added_before_ && mkt.order_book().empty();
    } else {
        return false;
    }
}

void OwnerUser::print_stats( const Market& ) const { }
