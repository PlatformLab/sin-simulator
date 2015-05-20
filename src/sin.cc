/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include <cassert>
#include <climits>
#include "abstract_user.hh"
#include "noop_user.hh"
#include "owner_user.hh"
#include "brute_force_user.hh"
#include "market_emulator.hh"
#include "market.hh"

//using namespace std;

int utility_func(std::list<size_t>& lst, size_t flow_start_time)
{
    // take in size of flow and int min if they dont have full flow
    auto max_it = std::max_element(lst.begin(), lst.end());
    if (max_it == lst.end()) {
        return INT_MIN;
    } else {
        return - (*max_it - flow_start_time);
    }
}

int main(){
    std::cout << "hello world" << std::endl;

    std::vector<std::unique_ptr<AbstractUser>> usersToEmulate;
    usersToEmulate.emplace_back(std::make_unique<BruteForceUser>( "keith", 0, 5, utility_func ));
    usersToEmulate.emplace_back(std::make_unique<BruteForceUser>( "gregs", 1, 2, utility_func ));
    //usersToEmulate.emplace_back(std::make_unique<BruteForceUser>( "gregs", 1, 2, utility_func ));

    //usersToEmulate.emplace_back(std::make_unique<NoopUser>( "larry" ));
    usersToEmulate.emplace_back(std::make_unique<OwnerUser>( "ccast", 1, 10, true ));

    MarketEmulator emulated_market(move(usersToEmulate));

    emulated_market.run_to_completion();
    return 1;
}
