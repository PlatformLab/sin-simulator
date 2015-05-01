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
    /*const*/int64_t time;
    std::string owner; 
    std::deque<struct BidOffer> bids;
    std::deque<struct BidOffer> offers;

    bool add_bid(std::string &user_name, uint32_t price) { return true;}
    bool add_offer(std::string &user_name, uint32_t price){ return true;}

    bool delete_bids(std::string &user_name){ return true;}
    bool delete_offers(std::string &user_name){ return true;}

};

class Market {
    private:
    std::deque<struct Slot> order_book;

    public:
    Market(uint32_t num_future_slots);

    const std::deque<struct Slot> &get_order_book() { return order_book; }

    void advance_time();
    void match_bids_and_orders();

    void print_order_book();

    // owner only create slot
};

#endif /* MARKET */
