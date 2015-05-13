/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include <cassert>
#include "abstract_user.hh"
#include "basic_user.hh"
#include "owner_user.hh"
#include "market_emulator.hh"
#include "market.hh"

using namespace std;

int main(){
    cout << "hello world" << endl;

    vector<unique_ptr<AbstractUser>> usersToEmulate;
    usersToEmulate.emplace_back(make_unique<BasicUser>( "gregs", 0, 4 ));
    usersToEmulate.emplace_back(make_unique<BasicUser>( "keith", 1, 2 ));
    usersToEmulate.emplace_back(make_unique<OwnerUser>( "ccast", 1, 10 ));

    MarketEmulator emulated_market(move(usersToEmulate));

    emulated_market.run_to_completion();
    return 1;
}
