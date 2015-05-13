/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "owner_user.hh"

using namespace std;

    OwnerUser::OwnerUser(const std::string &name, const uint32_t default_slot_offer, const size_t total_num_slots, const bool add_once)
: AbstractUser( name ),
    default_slot_offer(default_slot_offer),
    total_num_slots(total_num_slots),
    add_once(add_once),
    added_before(false)
{
}

void OwnerUser::take_actions(Market& mkt) 
{
    auto &order_book = mkt.mutable_order_book();

    if (add_once and not added_before) {
        while (order_book.size() < total_num_slots) {
            uint64_t next_time = order_book.empty() ? 0 : order_book.back().time + 1;

            order_book.emplace_back(name, next_time);
            order_book.back().add_offer( { default_slot_offer, name } ); 
        }
        added_before = true;
    }
}

void OwnerUser::print_stats(Market& ) const {}
