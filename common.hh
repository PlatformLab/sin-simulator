#ifndef COMMON
#define COMMON

#include <vector>

struct Buyer {
    std::string name;
};

// information about a slot that market gives to a buyer
struct Slot_view {
    uint64_t time;
    uint32_t current_offer;
    bool you_own;
};

struct User_action {
    uint64_t slot_time;
};

struct Buy_bid : public User_action {
    uint32_t price;
};

struct Delete_bid : public User_action { };

struct Sell_offer : public User_action {
    uint32_t price;
};

struct Packet_to_send : public User_action {
    std::string packet;
};

struct User_actions {
    std::vector<struct Buy_bid> buy_bids;
    std::vector<struct Delete_bid> delete_bids;
    std::vector<struct Sell_offer> sell_offers;
    std::vector<struct Packet_to_send> packets_to_send;
};

#endif /* COMMON */
