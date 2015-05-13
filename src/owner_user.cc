/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "owner_user.hh"

using namespace std;

    OwnerUser::OwnerUser(const std::string &name, const uint32_t default_slot_offer, const size_t total_num_slots)
: AbstractUser( name ),
    default_slot_offer(default_slot_offer),
    total_num_slots(total_num_slots)
{
}

void OwnerUser::take_actions(Market& mkt) 
{
    auto &order_book = mkt.mutable_order_book();

    assert(not order_book.empty());
    while (order_book.size() < total_num_slots) {
        order_book.emplace_back(name, order_book.back().time);
        order_book.back().add_offer( { default_slot_offer, name } ); 
    }
}

void OwnerUser::print_stats(Market& ) const {}
