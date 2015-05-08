#ifndef SLOT
#define SLOT

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
    std::function<void ()> if_packet_sent;// = [](){};
};

// make class, 
struct Slot {
    std::string owner;
    std::function<void ()> if_packet_sent = [](){};
    const uint64_t time;
    private:
    std::deque<struct BidOffer> bids = {};
    std::deque<struct BidOffer> offers = {};

    public:
    Slot(std::string owner, uint64_t time) : owner(owner), time(time) {}

    private:
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

#endif /* SLOT */
