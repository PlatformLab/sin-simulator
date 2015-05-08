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

static size_t current_flow_completion_time(std::deque<struct Slot> &order_book, const std::string &name)
{
    size_t highest_owned_slot_idx = 0;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        if (order_book.at(i).owner == name) {
            highest_owned_slot_idx = i;
        }
    }
    return highest_owned_slot_idx;
}

static size_t flow_completion_time_if_sold(std::deque<struct Slot> &order_book, const std::string &name, size_t idx_to_sell)
{
    size_t fct_if_sold = 0;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        if (order_book.at(i).owner == name && i != idx_to_sell) {
            fct_if_sold = i;
        }
    }
    return fct_if_sold;
}

static uint32_t cost_to_get_slot(struct Slot& slot)
{
    if (not slot.bids.empty()) {
        return std::max(slot.lowest_offer().cost+1, slot.highest_bid().cost+1);
    } else {
        return slot.lowest_offer().cost+1;
    }
}

class BasicUser : public AbstractUser
{
    size_t flow_size;

    public:
        BasicUser(const std::string &name, struct Market& mkt, size_t flow_size) : AbstractUser(name, mkt), flow_size(flow_size) {}

        void take_actions()
        {
            if (flow_size) {
                std::deque<struct Slot> &order_book = mkt.get_order_book();
                for (size_t i = 0; i < mkt.get_order_book().size(); i++)
                {
                    if (mkt.get_order_book().at(i).owner == name) {
                        bid_won(i);
                    }
                }

                size_t slots_owned = num_slots_owned(mkt.get_order_book(), name);
                std::cout << "I'm a basic user named " << name;
                std::cout << " with a flow of size " << flow_size <<
                    " and I currently have " << slots_owned << " slots" << std::endl;
                if (flow_size > slots_owned)
                    get_best_slots(mkt.get_order_book(), flow_size - slots_owned);
            }
        }

        void bid_won(size_t at_idx)
        {
            // price and make offer
            std::vector<size_t> idxs_to_buy;
            size_t fct_if_sold = flow_completion_time_if_sold(mkt.get_order_book(), name, at_idx);
            int utility_delta = recursive_pick_best_slots(mkt.get_order_book(), 0, 1, idxs_to_buy, fct_if_sold);
            std::cout << "in bid won for " << name << " at idx " << at_idx << " and fct_if_sold " << fct_if_sold
            << " got utility delta " << utility_delta 
            << " and idx to buy instead " << idxs_to_buy.front() << std::endl;
        }

        void offer_taken(size_t at_idx)
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
        size_t cur_fct = current_flow_completion_time(order_book, name);
        recursive_pick_best_slots(order_book, 0, flow_size, idxs_to_buy, cur_fct);
       
        for (auto i : idxs_to_buy)
        {
            auto &slot = order_book.at(i);
            struct BidOffer toAdd = { cost_to_get_slot(slot), name };
            toAdd.if_packet_sent = [&] () {packet_sent();};
            //toAdd.if_bid_wins = [&] (size_t i) {bid_won(i);};
            slot.add_bid( toAdd );
            std::cout << name << " making bid of $" << toAdd.cost << " to idx " << i << std::endl;
        }
        std::cout << std::endl;
    }

    private:
    int recursive_pick_best_slots(std::deque<struct Slot> &order_book, size_t start, size_t n,
    std::vector<size_t> &idxs, size_t cur_fct)
    {
        int best_utility = -11111;
        std::vector<size_t> best_idxs;
        for (size_t i = start; i < order_book.size()-n; i++)
        {
            struct Slot &cur_slot = order_book.at(i);
            if (cur_slot.owner != name and not cur_slot.offers.empty()) {
                std::vector<size_t> recursive_idxs = idxs; // hopefully copy
                recursive_idxs.emplace_back(i);

                int utility;
                // base case
                if (n == 1) {
                    utility = -cost_to_get_slot(cur_slot) - std::max(i, cur_fct);
                } else {
                    utility = -cost_to_get_slot(cur_slot) 
                        + recursive_pick_best_slots(order_book, i+1, n-1, recursive_idxs, cur_fct);
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
