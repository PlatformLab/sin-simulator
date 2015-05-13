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
        std::deque<SingleSlot> order_book_ = {}; //ordered my most recent time first
        std::deque<SingleSlot> sent_slots_ = {}; // XXX change to events

    public:
        std::deque<SingleSlot> &sent_slots() { return sent_slots_; }
        std::deque<SingleSlot> &mutable_order_book() { return order_book_; }

        void advance_time();

        // owner only create slot
        void owner_add_slot(std::string owner, uint64_t slot_time)
        { order_book_.emplace_back(owner, slot_time); };
};

#endif /* MARKET_HH */
