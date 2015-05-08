#ifndef MARKET
#define MARKET

#include <iostream>
#include <deque>
#include <queue>
#include <algorithm>
#include <cassert>
#include <memory>
#include <functional>

#include "abstract_user.hh"

struct BidOffer {
    uint32_t cost;
    std::string owner;
    std::function<void ()> if_packet_sent;
};

inline void filter_user_bidoffers(const std::string &user_name, std::deque<struct BidOffer> &from)
{
    auto name_matches = [&user_name](struct BidOffer x){return x.owner == user_name;}; 
    std::remove_if(from.begin(),from.end(), name_matches );
}

static bool compare_two_bidoffers(struct BidOffer &a, struct BidOffer &b)
{
    return (a.cost < b.cost);
}

struct Slot {
    std::string owner;
    std::function<void ()> if_packet_sent = [](){};
    const uint64_t time;
    private:
    std::deque<struct BidOffer> bids;
    std::deque<struct BidOffer> offers;

    public:
    Slot(std::string owner, uint64_t time) : owner(owner), time(time) {}

    private:
    void settle_slot()
    {
        if (not bids.empty() and not offers.empty() and
                highest_bid().cost > lowest_offer().cost) {
            owner = highest_bid().owner;
            if_packet_sent = highest_bid().if_packet_sent;
            offers.clear();
            bids.clear();
        }
    }

    public:
    void add_bid(struct BidOffer bid)
    {
        bids.emplace_back(bid);
        settle_slot();
    }

    void add_offer(struct BidOffer offer)
    {
        offers.emplace_back(offer);
        settle_slot();
    }

    bool has_offers() { return not offers.empty(); }
    bool has_bids() { return not bids.empty(); }

    const struct BidOffer &highest_bid() {
        assert(not bids.empty());
        return *std::max_element(bids.begin(), bids.end(), compare_two_bidoffers); }
    const struct BidOffer &lowest_offer() {
        assert(not offers.empty());
        return *std::min_element(offers.begin(), offers.end(), compare_two_bidoffers); }

    void delete_bids(const std::string &user_name) { filter_user_bidoffers(user_name, bids); }
    void delete_offers(const std::string &user_name) { filter_user_bidoffers(user_name, offers); }
};

class Market {
    private:
        std::deque<struct Slot> order_book; //ordered my most recent time first

    public:
        std::deque<struct Slot> &get_order_book() { return order_book; }

        void advance_time();

        void print_order_book();

        // owner only create slot
        void owner_add_slot(std::string owner, uint64_t slot_time)
        { order_book.emplace_back(owner, slot_time); };
};

#endif /* MARKET */
