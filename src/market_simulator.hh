/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef MARKET_SIMULATOR_HH
#define MARKET_SIMULATOR_HH

#include <unordered_map>
#include <iostream>
#include <deque>
#include <queue>
#include <list>
#include <algorithm>
#include <cassert>
#include <memory>
#include <functional>

#include "abstract_user.hh"
#include "market.hh"

class MarketSimulator {
    Market mkt_;
    std::vector<std::unique_ptr<AbstractUser>> users_;
    const bool verbose_;
    const bool random_user_order_;
    size_t total_roundtrips_ = 0;

    size_t next_idx( size_t last_idx );
    void users_take_actions_until_finished();

    public:
    MarketSimulator( std::vector<std::unique_ptr<AbstractUser>> &&users, bool verbose, bool random_user_order );

    void run_to_completion();

    void print_slots();

    const std::deque<PacketSent> &packets_sent() const { return mkt_.packets_sent(); };
    size_t total_roundtrips() const { return total_roundtrips_; };

    void print_packets_sent();

    std::unordered_map<size_t, double> print_money_exchanged();

    void print_user_stats();
    double sum_user_utilities();
    double sum_user_best_expected_utilities();
    double sum_user_benefits();
};

#endif /* MARKET_SIMULATOR_HH */
