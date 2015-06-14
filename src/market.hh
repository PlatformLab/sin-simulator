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
        std::deque<SingleSlot> order_book_ = {};
        std::deque<PacketSent> packets_sent_ = {};
        std::deque<MoneyExchanged> money_exchanged_ = {};
        size_t version_ = 0;

    public:
        const std::deque<SingleSlot> &order_book() const { return order_book_; }
        const std::deque<PacketSent> &packets_sent() const { return packets_sent_; };
        const std::deque<MoneyExchanged> &money_exchanged() const { return money_exchanged_; };
        const size_t &version() const { return version_; };

        void advance_time();
        void owner_add_to_order_book(const std::string &name, uint64_t next_time);

        void add_offer_to_slot(size_t slot_idx, BidOffer &&offer);
        void add_bid_to_slot(size_t slot_idx, BidOffer &&bid);

        void clear_offers_from_slot(size_t slot_idx, const std::string &name);
        void clear_bids_from_slot(size_t slot_idx, const std::string &name);
};

#endif /* MARKET_HH */
