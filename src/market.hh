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
#include "market_event.hh"

class Market {
    private:
    public: // temp
        std::deque<SingleSlot> order_book_ = {}; //ordered my most recent time first
        std::deque<SingleSlot> sent_slots_ = {}; // XXX change to events

    public:
        std::deque<SingleSlot> &sent_slots() { return sent_slots_; }
        std::deque<SingleSlot> &mutable_order_book() { return order_book_; }

        void advance_time();
        inline bool operator==(const Market& rhs) const {
            return order_book_ == rhs.order_book_ and sent_slots_ == rhs.sent_slots_;
        }
        inline bool operator!=(const Market& rhs) const {
            return not operator==(rhs);
        }
};

#endif /* MARKET_HH */
