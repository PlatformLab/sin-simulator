#include <iostream>
#include "abstract_user.hh"

class BasicUser : public AbstractUser
{
    const std::string name;
    const uint32_t flow_size;

    public:
        BasicUser(std::string name, uint32_t flow_size) : name(name), flow_size(flow_size) {}

        void take_actions(struct Market& mkt)
        {
            auto &order_book = mkt.get_order_book();

            std::cout << "I'm a basic user named " << name;
            std::cout << " with a flow of size " << flow_size << std::endl;
        }


    /*
    private:
    int get_best_slots(std::deque<struct Slot> &order_book, size_t start, size_t finish, size_t n,
    std::vector<size_t> idxs)
    {
        int best_utility = -11111;
        for (size_t i = start; i < finish-n; i++)
        {
            int utility = -order_book.at(i).

            

        }


*/
};
