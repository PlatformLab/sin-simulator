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

#include "interval.hh"
#include "opportunity.hh"
#include "offer.hh"
#include "transaction.hh"

class Market {
    private:
        size_t time_ = 0;
        size_t version_ = 0;
        std::vector<Offer> offers_ { };
        std::vector<Transaction> transactions_ { };
        const bool verbose_;

    public:
        Market( const bool verbose )
            : verbose_( verbose )
        { }

        void add_offer( Offer &offer );
        std::vector<Offer> offers_in_interval( Interval &i ) const;
        bool buy_offer( size_t uid, Offer &o );

        const std::vector<Opportunity> backing_opportunities( ) const;
        const std::vector<Transaction> transactions( ) const { return transactions_; };

        bool empty() const;

        size_t version() const { return version_; };
        size_t verbose() const { return verbose_; };
        size_t time() const { return time_; };
        void advance_time() { time_++; };
        void increment_version() { version_++; };
};

#endif /* MARKET_HH */
