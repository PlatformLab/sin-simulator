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
    public: // temp XXX easy to change back here because we have getters
        std::deque<SingleSlot> order_book_ = {};
        std::deque<MarketEvent> market_events_ = {};

    public:
        std::deque<MarketEvent> &market_events() { return market_events_; };
        std::deque<SingleSlot> &mutable_order_book() { return order_book_; }

        void advance_time();
        inline bool operator==(const Market& rhs) const {
            return order_book_ == rhs.order_book_ and market_events_ == rhs.market_events_;
        }
        inline bool operator!=(const Market& rhs) const {
            return not operator==(rhs);
        }
};

#endif /* MARKET_HH */
