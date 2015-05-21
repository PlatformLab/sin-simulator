/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"
#include <algorithm>

using namespace std;

//void Market::advance_until_time(const size_t ts)
void Market::advance_time()
{
    if ( not order_book_.empty() ) {

        const SingleSlot &front = order_book().front();

        //cout << "slot at time " << front.time << " awarded to " << front.owner << endl;

        // add slot to sent_slots and delete from order book
        packets_sent_.push_back({front.owner, front.time});
        order_book_.pop_front();
    }
}

void Market::owner_add_to_order_book(const std::string &name, uint64_t next_time)
{
    order_book_.push_back( {name, next_time} );
}

void Market::add_offer_to_slot(size_t slot_idx, BidOffer &&offer)
{
    auto transactions = order_book_.at(slot_idx).add_offer(offer);
    money_exchanged_.insert(money_exchanged_.end(), transactions.begin(), transactions.end());
}

void Market::add_bid_to_slot(size_t slot_idx, BidOffer &&bid)
{
    auto transactions = order_book_.at(slot_idx).add_bid(bid);
    money_exchanged_.insert(money_exchanged_.end(), transactions.begin(), transactions.end());
}

void Market::clear_offers_from_slot(size_t slot_idx, const std::string &name)
{
    order_book_.at(slot_idx).clear_all_offers(name);
}

void Market::clear_bids_from_slot(size_t slot_idx, const std::string &name)
{
    order_book_.at(slot_idx).clear_all_bids(name);
}
