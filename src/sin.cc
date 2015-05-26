/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include <cassert>
#include <map>
#include "abstract_user.hh"
#include "noop_user.hh"
#include "owner_user.hh"
#include "brute_force_user.hh"
#include "market_emulator.hh"
#include "market.hh"

#include "shortest_remaining_time_first.hh"

//using namespace std;

int utility_func(std::list<size_t>& lst, size_t flow_start_time)
{
    // take in size of flow and int min if they dont have full flow
    auto max_it = std::max_element(lst.begin(), lst.end());
    if (max_it == lst.end()) {
        return std::numeric_limits<int>::min();
    } else {
        return - (*max_it - flow_start_time);
    }
}

std::function<int(std::list<size_t>&, size_t)> get_utility_func(size_t num_packets)
{
    return [num_packets] ( std::list<size_t>& lst, size_t flow_start_time )
    {
        if (lst.size() != num_packets) {
            return std::numeric_limits<int>::min();
        } else {
            return - (int) (*std::max_element(lst.begin(), lst.end()) - flow_start_time);
        }
    };
}

void sim_brute_force_users(std::list<flow> user_args)
{
    std::vector<std::unique_ptr<AbstractUser>> usersToEmulate;
    for (auto & u : user_args)
    {
        std::cout << "User: " << u.name << " flow start time: " << u.flow_start_time << " num packets: " << u.num_packets << std::endl;
        usersToEmulate.emplace_back(std::make_unique<BruteForceUser>( u.name, u.flow_start_time, u.num_packets, get_utility_func( u.num_packets )));
    }

    usersToEmulate.emplace_back(std::make_unique<OwnerUser>( "~", 1, 10, true ));

    MarketEmulator emulated_market(move(usersToEmulate));

    emulated_market.run_to_completion();
    emulated_market.print_packets_sent();
    emulated_market.print_money_exchanged();
}

std::list<flow> random_users(size_t number)
{
    std::list<flow> toRet { };
    for (size_t i = 0; i < number; i ++)
    {
        toRet.push_back( { std::string(1,'A'+i), 0, (size_t) (rand() % 4) + 1 } );
    }
    return toRet;
}

int main(){
    std::cout << "hello world" << std::endl;

    //std::list<flow> usrs = { { "A", 0, 3 }, { "B", 0, 2 }, { "C", 0, 2 } };
    std::list<flow> usrs = random_users(3);
    sim_brute_force_users(usrs);
    std::cout << "shortest remaining time first was:" << std::endl;
    simulate_shortest_remaining_time_first(usrs);

    return 1;
}
