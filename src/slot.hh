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

struct BidOffer {
    uint32_t cost;
    std::string owner;
};

template <size_t size>
class Slot {
    std::deque<struct BidOffer> bids = {};
    std::deque<struct BidOffer> offers = {};

    public:
    std::array<std::string, size> owners;
    const uint64_t time;

    Slot(std::string owner, uint64_t time) : owner(owner), time(time) {}

    private:
    bool market_crossed();
    void settle_slot();

    public:
    void add_bid(struct BidOffer bid);
    void add_offer(struct BidOffer offer);

    bool has_offers();
    bool has_bids();

    const struct BidOffer &highest_bid();
    const struct BidOffer &lowest_offer();

    void delete_bids(const std::string &user_name);
    void delete_offers(const std::string &user_name);
};

typedef Slot<1> SingleSlot;

#endif /* SLOT_HH */
