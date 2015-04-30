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
    uint32_t time;
    struct User* owner;
    uint32_t current_offer;
    std::vector<struct Bid> bids;
    std::string* packet;
};


class Market {
    std::deque<Slot> order_book;

    void add_bid(struct User* user, uint32_t slot_time, uint32_t price);
    bool delete_bid(struct User* user, uint32_t slot_time);

    void add_offer(struct User* user, uint32_t slot_time, uint32_t price);

    void add_packet(struct User* user, uint32_t slot_time, std::string* packet);
};
