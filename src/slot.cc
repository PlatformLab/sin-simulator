/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "slot.hh"

bool Slot::market_crossed()
{
    return not bids.empty() and not offers.empty() and
            highest_bid().cost >= lowest_offer().cost;
}

void Slot::settle_slot()
{
    // only single loop for now because we clear bids
    while ( market_crossed() ) {
        owner = highest_bid().owner;
        offers.clear();
        bids.clear();
    }
}

void Slot::add_bid(struct BidOffer bid)
{
    bids.emplace_back(bid);
    settle_slot();
}

void Slot::add_offer(struct BidOffer offer)
{
    offers.emplace_back(offer);
    settle_slot();
}

bool Slot::has_offers() { return not offers.empty(); }
bool Slot::has_bids() { return not bids.empty(); }

static bool compare_two_bidoffers(struct BidOffer &a, struct BidOffer &b)
{
    return (a.cost < b.cost);
}

const struct BidOffer &Slot::highest_bid()
{
    assert(not bids.empty());
    return *std::max_element(bids.begin(), bids.end(), compare_two_bidoffers);
}

const struct BidOffer &Slot::lowest_offer()
{
    assert(not offers.empty());
    return *std::min_element(offers.begin(), offers.end(), compare_two_bidoffers);
}

inline void filter_user_bidoffers(const std::string &user_name, std::deque<struct BidOffer> &from)
{
    auto name_matches = [&user_name](struct BidOffer x){return x.owner == user_name;}; 
    std::remove_if(from.begin(),from.end(), name_matches );
}

void Slot::delete_bids(const std::string &user_name)
{
    filter_user_bidoffers(user_name, bids);
}

void Slot::delete_offers(const std::string &user_name)
{
    filter_user_bidoffers(user_name, offers);
}
