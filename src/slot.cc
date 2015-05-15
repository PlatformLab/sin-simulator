/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "slot.hh"

using namespace std;

bool Slot::market_crossed()
{
    return not bids.empty() and not offers.empty() and
            best_bid().cost >= best_offer().cost;
}

deque<MoneyExchanged> Slot::settle_slot()
{
    // only single loop for now because we clear bids
    while ( market_crossed() ) {
        owner = best_bid().owner;
        offers.clear();
        bids.clear();
        // median of best bid and best offer transferred from new owner to old owner

        // print to tape and total up later
    }
    return {};
}

deque<MoneyExchanged> Slot::add_bid(BidOffer bid)
{
    bids.emplace_back(bid);
    return settle_slot();
}

deque<MoneyExchanged> Slot::add_offer(BidOffer offer)
{
    offers.emplace_back(offer);
    return settle_slot();
}

bool Slot::has_offers() const { return not offers.empty(); }
bool Slot::has_bids() const { return not bids.empty(); }

static bool compare_two_bidoffers(const BidOffer &a, const BidOffer &b)
{
    return (a.cost < b.cost);
}

const BidOffer &Slot::best_bid () const
{
    assert(not bids.empty());
    return *max_element(bids.begin(), bids.end(), compare_two_bidoffers);
}

const BidOffer &Slot::best_offer() const
{
    assert(not offers.empty());
    return *min_element(offers.begin(), offers.end(), compare_two_bidoffers);
}

void filter_user_bidoffers(const string &user_name, deque<BidOffer> &from)
{
    remove_if( from.begin(), from.end(), [&](BidOffer x){return x.owner == user_name;} );
}

void Slot::clear_all_bids(const string &user_name)
{
    filter_user_bidoffers(user_name, bids);
}

void Slot::clear_all_offers(const string &user_name)
{
    filter_user_bidoffers(user_name, offers);
}
