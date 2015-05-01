#ifndef MARKET
#define MARKET

#include <iostream>
#include <deque>
#include <vector>
#include <cassert>
#include <memory>

#include "common.hh"

struct Bid {
    uint32_t cost;
    std::string owner;
};

struct Slot {
    public:
    const uint64_t time;
    std::shared_ptr<std::string> owner;
    uint32_t current_offer;

    Slot(uint64_t time, uint32_t base_price) 
        : time(time),
        owner(nullptr),
        current_offer(base_price),
        packet("")
        {}

    struct Slot_view to_slot_view(std::string &recipient)
    {
        return {time, current_offer, (owner && *owner == recipient)};
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

    bool add_bid(std::string &user_name, uint64_t slot_time, uint32_t price);

    // delete all bids made by a user for time slot_time, returns true if one or more bids deleted
    bool delete_bids(std::string &user_name, uint64_t slot_time);

    bool add_offer(std::string &user_name, uint64_t slot_time, uint32_t price);

    bool add_packet(std::string &user_name, uint64_t slot_time, std::string packet);

    void advance_time();
    void match_bids_and_orders();

    bool empty();
    void print_order_book();
    std::vector<struct Slot_view> give_order_book(std::string &recipient);
};

#endif /* MARKET */
