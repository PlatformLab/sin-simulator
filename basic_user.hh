#include <iostream>
#include <deque>
#include "abstract_user.hh"
#include "market.hh"

static size_t num_slots_owned(std::deque<struct Slot> &order_book, const std::string &name)
{
    size_t slots_owned = 0;
    for (auto &slot : order_book)
    {
        if (slot.owner == name)
            slots_owned++;
    }
    return slots_owned;
}

class BasicUser : public AbstractUser
{
    size_t flow_size;

    public:
        BasicUser(const std::string &name, struct Market& mkt, size_t flow_size) : AbstractUser(name, mkt), flow_size(flow_size) {}

        void take_actions()
        {
            if (flow_size) {
                size_t slots_owned = num_slots_owned(mkt.get_order_book(), name);
                std::cout << "I'm a basic user named " << name;
                std::cout << " with a flow of size " << flow_size <<
                    " and I currently have " << slots_owned << " slots" << std::endl;
                if (flow_size > slots_owned)
                    get_best_slots(mkt.get_order_book(), flow_size - slots_owned);
            }
        }

        void bid_won()
        {
            // price and make offer, do we do this once per packet bought or like in rounds or what

            //size_t slots_owned = num_slots_owned(mkt.get_order_book(), name);
            //get_best_slots(mkt.get_order_book(), flow_size - slots_owned);
            std::vector<size_t> idxs_to_buy;
            int utility_delta = recursive_pick_best_slots(mkt.get_order_book(), 0, 1, idxs_to_buy);
            std::cout << "in bid won for " << name << " got utility delta " << utility_delta 
            << " and idx to buy instead " << idxs_to_buy.front() << std::endl;
        }

        void offer_taken()
        {
            std::cout << "in offer taken for " << name << std::endl;
        }

        void packet_sent()
        {
            std::cout << "in packet sent for " << name << std::endl;
            flow_size--;
            if (flow_size == 0)
                std::cout << "flow for user " << name << " finished" << std::endl;
        }


    void get_best_slots(std::deque<struct Slot> &order_book, size_t flow_size)
    {
        std::vector<size_t> idxs_to_buy;
        recursive_pick_best_slots(order_book, 0, flow_size, idxs_to_buy);
       
        std::cout << name << " got idxs to buy ";
        for (auto i : idxs_to_buy)
        {
            auto &slot = order_book.at(i);
            struct BidOffer toAdd = { slot.lowest_offer().cost + 1, name };
            toAdd.if_packet_sent = [&] () {packet_sent();};
            toAdd.if_bid_wins = [&] () {bid_won();};
            slot.add_bid( toAdd );
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    private:
    int recursive_pick_best_slots(std::deque<struct Slot> &order_book, size_t start, size_t n,
    std::vector<size_t> &idxs)
    {
        int best_utility = -11111;
        std::vector<size_t> best_idxs = idxs;
        for (size_t i = start; i < order_book.size()-n; i++)
        {
            struct Slot &cur_slot = order_book.at(i);
            if (cur_slot.owner != name and not cur_slot.offers.empty()) {
                std::vector<size_t> recursive_idxs = idxs; // hopefully copy
                recursive_idxs.emplace_back(i);

                int utility;
                // base case
                if (n == 1) {
                    utility = -(cur_slot.lowest_offer().cost+1) - i;  // where i is flow completion time
                } else {
                    utility = -(cur_slot.lowest_offer().cost+1)
                        + recursive_pick_best_slots(order_book, i+1, n-1, recursive_idxs);
                }
                if (utility > best_utility) {
                    best_utility = utility;
                    best_idxs = recursive_idxs; //hopefully copy as well
                }
            }
        }
        idxs = best_idxs;
        //assert(best_utility != -11111)
        return best_utility;
    }
};
