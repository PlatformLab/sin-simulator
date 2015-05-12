/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include <cassert>
#include "abstract_user.hh"
#include "basic_user.hh"
#include "market_emulator.hh"
#include "market.hh"

using namespace std;

int main(){
    cout << "hello world" << endl;
    EmulatedUser g = {0, make_unique<BasicUser>( "gregs", 4 )};
    EmulatedUser k = {0, make_unique<BasicUser>( "keith", 2 )};

    vector<EmulatedUser> usersToEmulate;
    usersToEmulate.emplace_back(move(g));
    usersToEmulate.emplace_back(move(k));

    MarketEmulator emulated_market(move(usersToEmulate), "ccast", 1, 10);

    emulated_market.run_to_completion();
    return 1;
}
