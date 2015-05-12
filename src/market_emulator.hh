/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef MARKET_EMULATOR_HH
#define MARKET_EMULATOR_HH

#include <iostream>
#include <deque>
#include <queue>
#include <algorithm>
#include <cassert>
#include <memory>
#include <functional>

#include "abstract_user.hh"
#include "market.hh"

class MarketEmulator {
    Market mkt_;
    std::vector<std::unique_ptr<AbstractUser>> users_;

    bool all_users_finished();

    public:
    MarketEmulator( std::vector<std::unique_ptr<AbstractUser>> &&users, const std::string &default_user, uint32_t default_price, size_t total_num_slots );

    void run_to_completion();
};

#endif /* MARKET_EMULATOR_HH */
