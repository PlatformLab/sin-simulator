#include <iostream>
#include <vector>
#include <queue>
#include <cassert>

using namespace std;

struct User {
    string name;
};

struct Bid {
    uint32_t cost;
    struct User* owner;
};

struct Slot {
    uint32_t time;
    struct User* owner;
    uint32_t current_offer;
    vector<struct Bid> bids;
    string* packet;
};

class Market {
    void add_bid(struct User* user, uint32_t slot_time, uint32_t price);
    bool delete_bid(struct User* user, uint32_t slot_time);

    void add_offer(struct User* user, uint32_t slot_time, uint32_t price);

    void add_packet(struct User* user, uint32_t slot_time, string* packet);
};
