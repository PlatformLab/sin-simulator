#ifndef MARKET
#define MARKET

#include <iostream>
#include <deque>
#include <queue>
#include <algorithm>
#include <cassert>
#include <memory>

struct BidOffer {
    uint32_t cost;
    std::string owner;
};

void filter_user_bidoffers(std::string &user_name, std::deque<struct BidOffer> &from)
{
    auto name_matches = [&user_name](struct BidOffer x){return x.owner == user_name;}; 
    std::remove_if(from.begin(),from.end(), name_matches );
}

struct Slot {
    public:
    /*const*/uint64_t time;
    std::string owner; 
    std::deque<struct BidOffer> bids;
    std::deque<struct BidOffer> offers;

    void add_bid(struct BidOffer bid) { bids.emplace_back(bid); }
    void add_offer(struct BidOffer offer) { offers.emplace_back(offer); }

    void delete_bids(std::string &user_name) { filter_user_bidoffers(user_name, bids); }

    void delete_offers(std::string &user_name) { filter_user_bidoffers(user_name, offers); }
};

class Market {
    private:
    std::deque<struct Slot> order_book; //ordered my most recent time first

    public:
    Market(uint32_t num_future_slots);

    const std::deque<struct Slot> &get_order_book() { return order_book; }

    void advance_time();
    void match_bids_and_orders();

    void print_order_book();

    // owner only create slot
};

#endif /* MARKET */
