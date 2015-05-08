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

class BasicUser : public AbstractUser
{
    size_t flow_size;

    public:
        BasicUser(const std::string &name, struct Market& mkt, size_t flow_size) : AbstractUser(name, mkt), flow_size(flow_size) {}

        void add_offer_to_slot(size_t at_idx)
        {
            std::deque<struct Slot> &order_book = mkt.get_order_book();
            struct Slot &slot = order_book.at(at_idx);
            assert(slot.owner == name);

            // price and make offer
            std::vector<size_t> idxs_to_buy;
            size_t fct_if_sold = flow_completion_time_if_sold(order_book, name, at_idx);
            int utility_delta = recursive_pick_best_slots(order_book, 0, 1, idxs_to_buy, fct_if_sold);
            std::cout << name << " adding offer to idx " << at_idx << " and fct_if_sold " << fct_if_sold
            << " got utility delta " << utility_delta 
            << " and idx to buy instead " << idxs_to_buy.front() << std::endl;
            assert(utility_delta < 0);
            struct BidOffer toAdd = { (uint32_t) (-utility_delta) + 1, name };
            slot.add_offer( toAdd );
        }

        void take_actions()
        {
            std::deque<struct Slot> &order_book = mkt.get_order_book();
            for (size_t i = 0; i < order_book.size(); i++)
            {
                struct Slot &cur_slot = order_book.at(i);
                if (cur_slot.owner == name and not cur_slot.has_offers())
                {
                    add_offer_to_slot(i);
                }
            }

            if (flow_size > 0) {
                size_t slots_owned = num_slots_owned(mkt.get_order_book(), name);
                std::cout << "I'm a basic user named " << name;
                std::cout << " with a flow of size " << flow_size <<
                    " and I currently have " << slots_owned << " slots" << std::endl;
                if (flow_size > slots_owned)
                    get_best_slots(mkt.get_order_book(), flow_size - slots_owned);
            }
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
            struct BidOffer toAdd = { slot.lowest_offer().cost, name };
            toAdd.if_packet_sent = [&] () {packet_sent();};
            slot.add_bid( toAdd );
            std::cout << name << " making bid of $" << toAdd.cost << " to idx " << i;
            if (slot.owner == name)
            {
                std::cout << "... got slot!" << std::endl;
//                add_offer_to_slot(i); 
            }
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
            if (cur_slot.owner != name and cur_slot.has_offers()) {
                std::vector<size_t> recursive_idxs = idxs; // hopefully copy
                recursive_idxs.emplace_back(i);

                int utility;
                // base case
                if (n == 1) {
                    utility = -cur_slot.lowest_offer().cost - std::max(i, cur_fct);
                } else {
                    utility = -cur_slot.lowest_offer().cost 
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
