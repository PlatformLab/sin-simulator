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

int utility_func(std::list<size_t>& lst, size_t flow_start_time, size_t)
{
    // take in size of flow and int min if they dont have full flow
    auto max_it = std::max_element(lst.begin(), lst.end());
    if (max_it == lst.end()) {
        return std::numeric_limits<int>::min();
    } else {
        return - (*max_it - flow_start_time);
    }
}

std::function<int(std::list<size_t>&, size_t, size_t)> get_utility_func(size_t num_packets)
{
    return [num_packets] ( std::list<size_t>& lst, size_t flow_start_time, size_t packets_already_sent )
    {
        if ( (lst.size() + packets_already_sent) < num_packets ) {
            return std::numeric_limits<int>::min();
        } else {
            return - (int) (*std::max_element(lst.begin(), lst.end()) - flow_start_time);
        }
    };
}

const std::list<PacketSent> sim_brute_force_users(std::list<flow> user_args)
{
    std::vector<std::unique_ptr<AbstractUser>> usersToEmulate;
    for (auto & u : user_args)
    {
        std::cout << "User: " << u.name << " flow start time: " << u.flow_start_time << " num packets: " << u.num_packets << std::endl;
        usersToEmulate.emplace_back(std::make_unique<BruteForceUser>( u.name, u.flow_start_time, u.num_packets, get_utility_func( u.num_packets )));// TODO fix to include packets already sent
        //usersToEmulate.emplace_back(std::make_unique<BruteForceUser>( u.name, u.flow_start_time, u.num_packets, utility_func));
    }

    usersToEmulate.emplace_back(std::make_unique<OwnerUser>( "ccst", 1, 25, true ));
 //   usersToEmulate.emplace_back(std::make_unique<OwnerUser>( "ccst", 1, 10, true ));

    MarketEmulator emulated_market(move(usersToEmulate));

    emulated_market.run_to_completion();
    //emulated_market.print_money_exchanged();
    //emulated_market.print_packets_sent();
    std::list<PacketSent> packets_sent_copy = emulated_market.packets_sent();
    packets_sent_copy.remove_if([](PacketSent ps) { return ps.owner == "ccst"; } );
    return packets_sent_copy;
}

std::list<flow> random_users(size_t number)
{
    std::list<flow> toRet { };
    for (size_t i = 0; i < number; i ++)
    {
        toRet.push_back( { std::string(1,'A'+i), (size_t) (rand() % 6), (size_t) (rand() % 5) + 1 } );
    }
    return toRet;
}

int main(){
    std::cout << "hello world" << std::endl;

    //std::list<flow> usrs = { { "A", 0, 3 }, { "B", 0, 2 }, { "C", 0, 2 } };
    //std::list<flow> usrs = random_users(3); // TODO this an interesting one
    size_t num_matched = 0;
    size_t num_didnt_match = 0;
    for (int i = 0; i < 1; i++)
    {
        std::list<flow> usrs = random_users(rand() % 4 + 1); // this an interesting one
        auto market = sim_brute_force_users(usrs);
        auto srtf = simulate_shortest_remaining_time_first(usrs);
        if (market == srtf)
        {
            num_matched++;
            std::cout << "market matched srtf results!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        } else {
            num_didnt_match++;
            std::cout << "market didnt match srtf! Market:"<< std::endl;
            printPacketsSent(market);
            std::cout << "and srtf:" << std::endl;
            printPacketsSent(srtf);
        }
    }
    std::cout << num_matched << " of " << num_matched + num_didnt_match << " scenarios matched the srtf result" << std::endl;
    return 1;
}
