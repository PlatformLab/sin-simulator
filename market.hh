#ifndef MARKET
#define MARKET

#include <iostream>
#include <deque>
#include <vector>
#include <cassert>
#include <memory>

struct BidOffer {
    uint32_t cost;
    std::string owner;
};

struct Slot {
    public:
    const uint64_t time;
    std::string owner; 
    std::deque<struct BidOffer> bids;
    std::deque<struct BidOffer> offers;
};

class Market {
    private:
    std::deque<struct Slot> order_book;
    const uint32_t order_book_size;
    uint64_t cur_time;
    const uint32_t default_slot_price;

    public:
    Market(uint32_t num_future_slots, uint32_t default_slot_price);

    const std::deque<struct Slot> &get_order_book()
    {
        return order_book;
    }

    bool add_bid(std::string &user_name, uint64_t slot_time, uint32_t price);

    // delete all bids made by a user for time slot_time, returns true if one or more bids deleted
    bool delete_bids(std::string &user_name, uint64_t slot_time);

    bool add_offer(std::string &user_name, uint64_t slot_time, uint32_t price);

    bool add_packet(std::string &user_name, uint64_t slot_time, std::string packet);

    void advance_time();
    void match_bids_and_orders();

    bool empty();
    void print_order_book();

    // owner only create slot
};

#endif /* MARKET */
