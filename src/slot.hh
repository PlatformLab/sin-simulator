/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SLOT_HH
#define SLOT_HH

#include <iostream>
#include <deque>
#include <queue>
#include <algorithm>
#include <cassert>
#include <memory>
#include <functional>

#include "market_event.hh"

struct BidOffer {
    uint32_t cost;
    std::string owner;

    inline bool operator==(const BidOffer& rhs) const {
        return cost == rhs.cost and owner == rhs.owner;
    }
};

// template <size_t size> XXX later
class Slot {
    public: // temp
    std::deque<BidOffer> bids = {};
    std::deque<BidOffer> offers = {};

    public:
    //std::array<std::string, size> owners; TODO
    std::string owner;
    uint64_t time; //const

    Slot(std::string owner, uint64_t time) : owner(owner), time(time) {}

    private:
    bool market_crossed();
    std::deque<MoneyExchanged> settle_slot();

    public:
    std::deque<MoneyExchanged> add_bid(struct BidOffer bid);
    std::deque<MoneyExchanged> add_offer(struct BidOffer offer);

    bool has_offers() const;
    bool has_bids() const;

    const struct BidOffer &best_bid() const;
    const struct BidOffer &best_offer() const;

    void clear_all_bids(const std::string &user_name);
    void clear_all_offers(const std::string &user_name);

    inline bool operator==(const Slot& rhs) const {
        return owner == rhs.owner and time == rhs.time and 
            bids == rhs.bids and offers == rhs.offers;
    }
};

typedef Slot SingleSlot;

#endif /* SLOT_HH */
