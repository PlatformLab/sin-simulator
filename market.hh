#ifndef MARKET
#define MARKET

#include <iostream>
#include <deque>
#include <vector>
#include <cassert>

#include "common.hh"

struct Bid {
    uint32_t cost;
    struct Buyer* owner;
};

struct Slot {
    public:
    const uint64_t time;
    struct Buyer* owner;
    uint32_t current_offer;

    Slot(uint64_t time, uint32_t base_price) 
        : time(time),
        owner(nullptr),
        current_offer(base_price),
        packet("")
        {}

    struct Slot_view to_slot_view(struct Buyer &recipient)
    {
        return {time, current_offer, (owner && owner->name == recipient.name)};
    };

    //private:
    std::deque<struct Bid> bids;
    std::string packet;
};

class Market {
    private:
    std::deque<struct Slot> order_book;
    const uint32_t order_book_size;
    uint64_t cur_time;
    const uint32_t default_slot_price;

    public:
    Market(uint32_t num_future_slots, uint32_t default_slot_price);

    bool add_bid(struct Buyer* buyer, uint64_t slot_time, uint32_t price);

    // delete all bids made by a buyer for time slot_time, returns true if one or more bids deleted
    bool delete_bids(struct Buyer* buyer, uint64_t slot_time);

    bool add_offer(struct Buyer* buyer, uint64_t slot_time, uint32_t price);

    bool add_packet(struct Buyer* buyer, uint64_t slot_time, std::string packet);

    void advance_time();
    void match_bids_and_orders();

    bool empty();
    void print_order_book();
    std::vector<struct Slot_view> give_order_book(struct Buyer &recipient);
};

#endif /* MARKET */
