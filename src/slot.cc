/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "slot.hh"
#include <algorithm>

using namespace std;

bool Slot::market_crossed() const
{
    return not bids.empty() and not offers.empty() and
            best_bid().cost >= best_offer().cost;
}

// TODO double check
static double median(const double a, const double b)
{
    if (a > b) {
        return b + ((a-b)/2);
    } else {
        return a + ((a-b)/2);
    }
}

void Slot::settle_slot( std::deque<MoneyExchanged> &log )
{
    while ( market_crossed() ) {
        double sale_price = median(best_offer().cost, best_bid().cost);

        log.push_back( { best_bid().owner, best_offer().owner, sale_price, time } );

        owner = best_bid().owner;
        offers.clear();
        bids.clear();
    }
}

void Slot::add_bid( BidOffer bid, std::deque<MoneyExchanged> &log )
{
    bids.emplace_back( bid );
    settle_slot( log );
}

void Slot::add_offer( BidOffer offer, std::deque<MoneyExchanged> &log )
{
    offers.emplace_back( offer );
    settle_slot( log );
}

bool Slot::has_offers() const { return not offers.empty(); }
bool Slot::has_bids() const { return not bids.empty(); }

static bool compare_two_bidoffers(const BidOffer &a, const BidOffer &b)
{
    return a.cost < b.cost;
}

const BidOffer &Slot::best_bid () const
{
    assert(not bids.empty());
    if ( bids.size() == 1 ) {
        return bids.front();
    } else {
        return *max_element(bids.begin(), bids.end(), compare_two_bidoffers);
    }
}

const BidOffer &Slot::best_offer() const
{
    assert(not offers.empty());
    if ( offers.size() == 1 ) {
        return offers.front();
    } else {
        return *min_element(offers.begin(), offers.end(), compare_two_bidoffers);
    }
}

void Slot::clear_all_bids(const size_t &uid)
{
    bids.remove_if( [uid](BidOffer x){ return x.owner == uid; } );
}

void Slot::clear_all_offers(const size_t &uid)
{
    offers.remove_if( [uid](BidOffer x){ return x.owner == uid; } );
}
