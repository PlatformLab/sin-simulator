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
    std::vector<std::unique_ptr<AbstractBCP>> users_;

    public:
    MarketSimulator( std::vector<std::unique_ptr<AbstractUser>> &&users );

    void run_to_completion();

    void print_outcome();
};

#endif /* MARKET_SIMULATOR_HH */
