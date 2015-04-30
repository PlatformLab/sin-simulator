#include <iostream>
#include <queue>
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
    uint64_t time;
    struct User* owner;
    uint32_t current_offer;

    Slot(uint64_t time, uint32_t base_price) 
        : time(time),
        current_offer(base_price)
        {}

    //private:
    std::vector<struct Bid> bids;
    std::string* packet;
};


class Market {
    private:
    std::queue<struct Slot> order_book;
    const uint32_t order_book_size;
    uint64_t cur_time;
    const uint32_t default_slot_price;

    public:
    Market(uint32_t num_future_slots, uint32_t default_slot_price);

    void add_bid(struct User* user, uint64_t slot_time, uint32_t price);

    bool delete_bid(struct User* user, uint64_t slot_time);

    void add_offer(struct User* user, uint64_t slot_time, uint32_t price);

    void add_packet(struct User* user, uint64_t slot_time, std::string* packet);

    void advance_time();

    std::queue<struct Slot> get_order_book();
};
