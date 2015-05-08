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
#include "slot.hh"

class Market {
    private:
        std::deque<struct Slot> order_book = {}; //ordered my most recent time first

    public:
        std::deque<struct Slot> &get_order_book() { return order_book; }

        void advance_time();

        void print_order_book();

        // owner only create slot
        void owner_add_slot(std::string owner, uint64_t slot_time)
        { order_book.emplace_back(owner, slot_time); };
};

#endif /* MARKET */
