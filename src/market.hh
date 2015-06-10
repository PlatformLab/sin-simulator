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
        std::vector<PacketSent> packets_sent_ = {};
        std::vector<MoneyExchanged> money_exchanged_ = {};

    public:
        const std::deque<SingleSlot> &order_book() const { return order_book_; }
        const std::vector<PacketSent> &packets_sent() const { return packets_sent_; };
        const std::vector<MoneyExchanged> &money_exchanged() const { return money_exchanged_; };

        void advance_time();
        void owner_add_to_order_book(const std::string &name, uint64_t next_time);

        void add_offer_to_slot(const SingleSlot &slot, BidOffer &&offer);
        void add_bid_to_slot(const SingleSlot &slot, BidOffer &&bid);

        void add_offer_to_slot_idx(size_t slot_idx, BidOffer &&offer);
        void add_bid_to_slot_idx(size_t slot_idx, BidOffer &&bid);

        void clear_offers_from_slot(size_t slot_idx, const std::string &name);
        void clear_bids_from_slot(size_t slot_idx, const std::string &name);

        inline bool operator==(const Market& rhs) const {
            return order_book_ == rhs.order_book_ and packets_sent_ == rhs.packets_sent_
                and money_exchanged_ == rhs.money_exchanged_;
        }

        inline bool operator!=(const Market& rhs) const {
            return not operator==(rhs);
        }
};

#endif /* MARKET_HH */
