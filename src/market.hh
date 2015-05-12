/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef MARKET_HH
#define MARKET_HH

#include <iostream>
#include <deque>
#include <queue>
#include <vector>
#include <algorithm>
#include <cassert>
#include <memory>
#include <functional>

#include "abstract_user.hh"
#include "slot.hh"

class Market {
    private:
        std::deque<SingleSlot> _order_book = {}; //ordered my most recent time first
        std::deque<Events> _sent_slots = {};

    public:
        const std::deque<SingleSlot> &sent_slots() const { return _sent_slots; }
        std::deque<SingleSlot> &mutable_order_book() { return _order_book; }

        void advance_time();

        static void print_slots(const std::deque<Slot> &slots);

        // owner only create slot
        void owner_add_slot(std::string owner, uint64_t slot_time)
        { _order_book.emplace_back(owner, slot_time); };
};

#endif /* MARKET_HH */
