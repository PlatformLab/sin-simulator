/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef MARKET_HH
#define MARKET_HH

#include <iostream>
#include <deque>
#include <list>
#include <vector>
#include <cassert>
#include <memory>
#include <functional>
#include <algorithm>
#include <set>
#include <unordered_set>

#include "interval.hh"
#include "offer.hh"
#include "transaction.hh"
#include "pending_message.hh"

class Market {
    private:
        size_t version_ = 0;
        bool verbose_ = false;
        std::unordered_set<Offer> offers_ { };
        std::vector<Transaction> transactions_ { };
        std::unordered_map<size_t, std::vector<PendingMessage>> user_messages_ { }; // maps from user id to list of messages to deliver to that user

    public:
        Market()
        { }

        bool add_offer( const Offer &offer ); /* returns true if successful */
        const std::vector<Offer> offers_in_interval( const size_t start, const size_t end ) const;
        bool buy_offer( size_t uid, const Offer &o ); /* returns true if successful */

        const std::vector<Transaction> transactions() const { return transactions_; }

        const std::vector<PendingMessage> get_messages( const size_t & uid )
        { auto search = user_messages_.find( uid );
            if ( search != user_messages_.end() ) {
                // dump message vector and return it to user
                std::vector<PendingMessage> toRet { };
                std::swap( search->second, toRet );
                return toRet;
            } else {
                return { };
            }
        }

        size_t version() const { return version_; }
        void verbose() { verbose_ = true; }
        void increment_version() { version_++; }
};

#endif /* MARKET_HH */
