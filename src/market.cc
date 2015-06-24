/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"
#include <algorithm>

using namespace std;

void Market::advance_time()
{
    version_++;
    if ( not order_book_.empty() ) {

        const SingleSlot &front = order_book().front();

        // add slot to sent_slots and delete from order book
        packets_sent_.push_back({front.owner, front.time});
        packets_sent_num_owned_[ front.owner ]++;
        order_book_num_owned_[ front.owner ]--;
        order_book_.pop_front();
    }
}

void Market::owner_add_to_order_book(const size_t &uid, uint64_t next_time)
{
    version_++;
    order_book_.push_back( {uid, next_time} );
    order_book_num_owned_[ uid ]++;
}

void Market::add_offer_to_slot(size_t slot_idx, BidOffer offer)
{
    version_++;
    assert( order_book_.at(slot_idx).owner == offer.owner );
    const size_t before_owner_uid = order_book_.at(slot_idx).owner;
    order_book_.at(slot_idx).add_offer( offer, money_exchanged_ );
    const size_t after_owner_uid = order_book_.at(slot_idx).owner;
    if ( before_owner_uid != after_owner_uid ) {
        order_book_num_owned_[ before_owner_uid ] -= 1;
        order_book_num_owned_[ after_owner_uid ] += 1;
    }
}

void Market::add_bid_to_slot(size_t slot_idx, BidOffer bid)
{
    version_++;
    assert( order_book_.at(slot_idx).owner != bid.owner );
    const size_t before_owner_uid = order_book_.at(slot_idx).owner;
    order_book_.at(slot_idx).add_bid( bid, money_exchanged_ );
    const size_t after_owner_uid = order_book_.at(slot_idx).owner;
    if ( before_owner_uid != after_owner_uid ) {
        order_book_num_owned_[ before_owner_uid ] -= 1;
        order_book_num_owned_[ after_owner_uid ] += 1;
    }
}

size_t Market::num_owned_in_order_book( const size_t &uid ) const
{
    auto it = order_book_num_owned_.find( uid );
    if ( it == order_book_num_owned_.end() ) {
        return 0;
    } else {
        return it->second;
    }
}

size_t Market::num_owned_in_packets_sent( const size_t &uid ) const
{
    auto it = packets_sent_num_owned_.find( uid );
    if ( it == packets_sent_num_owned_.end() ) {
        return 0;
    } else {
        return it->second;
    }
}

void Market::clear_offers_from_slot(size_t slot_idx, const size_t &uid)
{
    version_++;
    order_book_.at(slot_idx).clear_all_offers( uid );
}

void Market::clear_bids_from_slot(size_t slot_idx, const size_t &uid)
{
    version_++;
    order_book_.at(slot_idx).clear_all_bids( uid );
}
