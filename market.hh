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

    //private:
    std::vector<struct Bid> bids;
    std::string* packet;
};


class Market {
    private:
    std::queue<struct Slot> order_book;
    uint64_t next_time_to_add;
    uint64_t cur_time;

    public:
    void add_bid(struct User* user, uint64_t slot_time, uint32_t price);

    bool delete_bid(struct User* user, uint64_t slot_time);

    void add_offer(struct User* user, uint64_t slot_time, uint32_t price);

    void add_packet(struct User* user, uint64_t slot_time, std::string* packet);

    void advance_time();

    std::queue<struct Slot> get_order_book();
};
