/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include <cassert>
#include <climits>
#include <map>
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

struct brute_force_args {
    std::string name;
    size_t flow_start_time;
    size_t num_packets;
};

void sim_brute_force_users(std::list<brute_force_args> user_args)
{
    std::vector<std::unique_ptr<AbstractUser>> usersToEmulate;
    for (auto & u : user_args)
    {
        std::cout << "User: " << u.name << " flow start time: " << u.flow_start_time << " num packets: " << u.num_packets << std::endl;
        usersToEmulate.emplace_back(std::make_unique<BruteForceUser>( u.name, u.flow_start_time, u.num_packets, utility_func ));
    }

    usersToEmulate.emplace_back(std::make_unique<OwnerUser>( "ccast", 1, 10, true ));

    MarketEmulator emulated_market(move(usersToEmulate));

    emulated_market.run_to_completion();
    emulated_market.print_packets_sent();
    emulated_market.print_money_exchanged();
}

int main(){
    std::cout << "hello world" << std::endl;

    std::list<brute_force_args> usrs;
    usrs.push_back({ "A", 0, 2 });
    usrs.push_back({ "B", 0, 2 });
    usrs.push_back({ "C", 2, 4 });

    sim_brute_force_users(usrs);
    usrs.reverse();
    sim_brute_force_users(usrs);
    return 1;
}
