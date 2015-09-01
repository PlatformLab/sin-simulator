/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include "abstract_user.hh"
#include "owner_user.hh"
#include "flow_completion_time_user.hh"
#include "market_simulator.hh"

using namespace std;

int main() //( int argc, char *argv[] )
{
    vector<unique_ptr<AbstractUser>> usersToSimulate;

    size_t uid = 0;
    size_t slots_needed = 10;
    size_t propagation_time_ms = 5;
    usersToSimulate.emplace_back(make_unique<OwnerUser>( uid++, 1, slots_needed, propagation_time_ms ));

    usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( uid++, 0, 2 ));
    usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( uid++, 2, 3 ));
    usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( uid++, 2, 3 ));
    usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( uid++, 2, 3 ));


    MarketSimulator simulated_market( move(usersToSimulate) );

    simulated_market.run_to_completion();
    simulated_market.print_outcome();
}
