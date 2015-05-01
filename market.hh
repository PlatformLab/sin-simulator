#include <iostream>
#include <deque>
#include <vector>
#include <cassert>

struct User {
    std::string name;
};

struct Bid {
    uint32_t cost;
    struct User* owner;
};

struct Slot {
    public:
    const uint64_t time;
    struct User* owner;
    uint32_t current_offer;

    Slot(uint64_t time, uint32_t base_price) 
        : time(time),
        current_offer(base_price),
        packet("")
        {}

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

    bool add_bid(struct User* user, uint64_t slot_time, uint32_t price);

    // delete all bids made by a user for time slot_time, returns true if one or more bids deleted
    bool delete_bids(struct User* user, uint64_t slot_time);

    bool add_offer(struct User* user, uint64_t slot_time, uint32_t price);

    bool add_packet(struct User* user, uint64_t slot_time, std::string packet);

    void advance_time();
    void match_bids_and_orders();

    std::deque<struct Slot> get_order_book();
};
