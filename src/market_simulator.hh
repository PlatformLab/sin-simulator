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
#include "owner_user.hh"
#include "flow_completion_time_user.hh"
#include "flow.hh"
#include "link.hh"

class MarketSimulator {
    Market mkt_;
    std::vector<std::unique_ptr<AbstractUser>> users_;

    void users_take_actions_until_finished();

    public:
    MarketSimulator( std::vector<Link> &links, std::vector<Flow> &flows );

    void run_to_completion();

    void print_outcome();
};

#endif /* MARKET_SIMULATOR_HH */
