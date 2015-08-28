/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include "abstract_user.hh"
#include "owner_user.hh"
#include "flow_completion_time_user.hh"
#include "market_simulator.hh"
#include "market.hh"

using namespace std;

int main( int argc, char *argv[] )
{
    vector<unique_ptr<AbstractUser>> usersToSimulate;

    size_t uid = 0;
    usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( uid++, 0, 5 ));
    usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( uid++, 2, 3 ));

    usersToSimulate.emplace_back(make_unique<OwnerUser>( 0, 1, slots_needed, true ));

    MarketSimulator simulated_market( move(usersToSimulate) );

    simulated_market.run_to_completion();
}
