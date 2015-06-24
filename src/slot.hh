/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SLOT_HH
#define SLOT_HH

#include <iostream>
#include <deque>
#include <list>
#include <queue>
#include <cassert>
#include <memory>
#include <functional>

#include "market_event.hh"

struct BidOffer {
    double cost;
    size_t owner;
};

// template <size_t size> XXX later
class Slot {
    private:
    std::list<BidOffer> bids = {};
    std::list<BidOffer> offers = {};

    bool market_crossed() const;
    void settle_slot( std::deque<MoneyExchanged> &log );

    public:
    //std::array<std::string, size> owners; TODO
    size_t owner;
    const uint64_t time;

    Slot( size_t uid, uint64_t time ) : owner( uid ), time( time ) {}

    void add_bid( struct BidOffer bid, std::deque<MoneyExchanged> &log );
    void add_offer( struct BidOffer offer, std::deque<MoneyExchanged> &log );

    void clear_all_bids(const size_t &uid);
    void clear_all_offers(const size_t &uid);

    bool has_offers() const;
    bool has_bids() const;

    const struct BidOffer &best_bid() const;
    const struct BidOffer &best_offer() const;
};

typedef Slot SingleSlot;

#endif /* SLOT_HH */
