/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"
#include <algorithm>

using namespace std;

//void Market::advance_until_time(const size_t ts)
void Market::advance_time()
{
    if ( not order_book_.empty() ) {

        const SingleSlot &front = order_book().front();

        cout << "slot at time " << front.time << " awarded to " << front.owner << endl;

        // add slot to sent_slots and delete from order book
        packets_sent_.push_back({front.owner, front.time});
        order_book_.pop_front();
    }
}

void Market::add_offer_to_slot(const size_t slot_idx, BidOffer offer)
{
    /*auto &transactions =*/ order_book_.at(slot_idx).add_offer(offer);
            //money_exchanged_.insert
}

void Market::add_bid_to_slot(const size_t slot_idx, BidOffer bid)
{
    order_book_.at(slot_idx).add_bid(bid);
}
