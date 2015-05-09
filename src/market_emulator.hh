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
    Market mkt;
    std::vector<std::pair< size_t, std::unique_ptr<AbstractUser>>> users_to_add; // users and time they join

std::vector<std::unique_ptr<AbstractUser>> active_users = {};

    MarketEmulator(Market &mkt, std::vector<std::pair< size_t, std::unique_ptr<AbstractUser>>> &&users) : mkt(mkt), users_to_add(std::move(users)) {};

    void run_to_completion();
};

#endif /* MARKET_EMULATOR_HH */
