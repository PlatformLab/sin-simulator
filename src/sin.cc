/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include <cassert>
#include "abstract_user.hh"
#include "basic_user.hh"
#include "noop_user.hh"
#include "owner_user.hh"
#include "brute_force_user.hh"
#include "market_emulator.hh"
#include "market.hh"

//using namespace std;

int main(){
    std::cout << "hello world" << std::endl;

    std::vector<std::unique_ptr<AbstractUser>> usersToEmulate;
    usersToEmulate.emplace_back(std::make_unique<BruteForceUser>( "gregs", 5, 3 ));
    //usersToEmulate.emplace_back(std::make_unique<NoopUser>( "larry" ));
    //usersToEmulate.emplace_back(std::make_unique<BasicUser>( "keith", 5, 2 ));
    usersToEmulate.emplace_back(std::make_unique<OwnerUser>( "ccast", 1, 10, true ));

    MarketEmulator emulated_market(move(usersToEmulate));

    emulated_market.run_to_completion();
    return 1;
}
