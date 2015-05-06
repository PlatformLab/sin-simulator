#include <iostream>
#include "abstract_user.hh"

class BasicUser : public AbstractUser
{
    const std::string name;
    const size_t flow_size;

    public:
        BasicUser(std::string name, size_t flow_size) : name(name), flow_size(flow_size) {}

        void take_actions(struct Market& mkt)
        {
            std::cout << "I'm a basic user named " << name;
            std::cout << " with a flow of size " << flow_size << std::endl;
            get_best_slots(mkt.get_order_book(), flow_size);
        }


    void get_best_slots(std::deque<struct Slot> &order_book, size_t flow_size)
    {
        std::vector<size_t> idxs_to_buy;
        recursive_slot_checker(order_book, 0, flow_size, idxs_to_buy);
       
        std::cout << "got idxs to buy ";
        for (auto i : idxs_to_buy)
        {
            auto &slot = order_book.at(i);
            struct BidOffer toAdd = { slot.lowest_offer().cost, name };
            slot.add_bid( toAdd );
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    private:
    int recursive_slot_checker(std::deque<struct Slot> &order_book, size_t start, size_t n,
    std::vector<size_t> &idxs)
    {
        // WE CANT OWN SLOT
        // base case
        if (n == 1)
        {
            idxs.emplace_back(start);
            return -order_book.at(start).lowest_offer().cost - start; // cost - flow completion time
        }

        int best_utility = -11111;
        std::vector<size_t> &best_idxs = idxs;
        for (size_t i = start; i < order_book.size()-n; i++)
        {
            std::vector<size_t> recursive_idxs(idxs);
            recursive_idxs.emplace_back(i);

            int utility;
            if (n == 1) {
                utility = -order_book.at(i).lowest_offer().cost - i;  // where i is flow completion time
            } else {
                utility = -order_book.at(i).lowest_offer().cost
                    + recursive_slot_checker(order_book, i+1, n-1, recursive_idxs);
            }
            if (utility > best_utility) {
                best_utility = utility;
                best_idxs = recursive_idxs;
            }
        }
        //assert(best_utility != -11111)
        return best_utility;
    }
};
