/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include "flow.hh"
#include "link.hh"
#include "market_simulator.hh"

using namespace std;

int main() //( int argc, char *argv[] )
{
    vector<unique_ptr<AbstractUser>> usersToSimulate;

    size_t uid = 0;
    const size_t slots_needed = 10;
    const size_t propagation_time = 5;
    const double default_interval_cost = 1;

    vector<Link> links { { uid++, slots_needed, propagation_time, default_interval_cost } };
    vector<Flow> flows { { uid++, 0, 2 }, { uid++, 0, 2 }, { uid++, 0, 2 }, { uid++, 0, 2 } };

    MarketSimulator simulated_market( links, flows );

    simulated_market.run_to_completion();
    simulated_market.print_outcome();
}
