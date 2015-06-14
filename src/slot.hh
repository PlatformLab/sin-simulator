/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SLOT_HH
#define SLOT_HH

#include <iostream>
#include <deque>
#include <list>
#include <queue>
#include <algorithm>
#include <cassert>
#include <memory>
#include <functional>

#include "market_event.hh"

struct BidOffer {
    double cost;
    std::string owner;
};

// template <size_t size> XXX later
class Slot {
    private:
    std::list<BidOffer> bids = {};
    std::list<BidOffer> offers = {};

    bool market_crossed() const;
    std::deque<MoneyExchanged> settle_slot();

    public:
    //std::array<std::string, size> owners; TODO
    std::string owner;
    const uint64_t time;

    Slot(std::string owner, uint64_t time) : owner(owner), time(time) {}

    std::deque<MoneyExchanged> add_bid(struct BidOffer bid);
    std::deque<MoneyExchanged> add_offer(struct BidOffer offer);

    void clear_all_bids(const std::string &user_name);
    void clear_all_offers(const std::string &user_name);

    bool has_offers() const;
    bool has_bids() const;

    const struct BidOffer &best_bid() const;
    const struct BidOffer &best_offer() const;
};

typedef Slot SingleSlot;

#endif /* SLOT_HH */
