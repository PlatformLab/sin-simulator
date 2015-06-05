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

std::function<int(const std::list<size_t>&, size_t, size_t)> get_utility_func(size_t num_packets)
{
    return [num_packets] ( const std::list<size_t>& times_could_own, size_t flow_start_time, size_t packets_already_sent )
    {
        if (packets_already_sent >= num_packets)
        {
        std::cout << "BIIG prob: packets_already_sent " << packets_already_sent  << " num_packets " << num_packets;
        std::cout << " flow start time " << flow_start_time << " times coud own size " <<  times_could_own.size() << std::endl;
        }
        assert(packets_already_sent < num_packets);
        if ( (times_could_own.size() + packets_already_sent) < num_packets ) {
            return std::numeric_limits<int>::min();
        } else {
            assert(not times_could_own.empty());
            assert(std::max_element(times_could_own.begin(), times_could_own.end()) != times_could_own.end());
            return - (int) (*std::max_element(times_could_own.begin(), times_could_own.end()) - flow_start_time);
        }
    };
}

const std::list<PacketSent> sim_brute_force_users(std::list<flow> user_args, const bool print_order_book_when_changed)
{
    std::vector<std::unique_ptr<AbstractUser>> usersToEmulate;
    for (auto & u : user_args)
    {
        std::cout << "User: " << u.name << " flow start time: " << u.flow_start_time << " num packets: " << u.num_packets << std::endl;
        usersToEmulate.emplace_back(std::make_unique<BruteForceUser>( u.name, u.flow_start_time, u.num_packets, get_utility_func( u.num_packets )));
        //usersToEmulate.emplace_back(std::make_unique<BruteForceUser>( u.name, u.flow_start_time, u.num_packets, utility_func));
    }

    usersToEmulate.emplace_back(std::make_unique<OwnerUser>( "ccst", 1, 21, true ));
   // usersToEmulate.emplace_back(std::make_unique<OwnerUser>( "ccst", 1, 20, true ));

    MarketEmulator emulated_market(move(usersToEmulate), print_order_book_when_changed);

    emulated_market.run_to_completion();
    emulated_market.print_money_exchanged();
    //emulated_market.print_packets_sent();
    std::list<PacketSent> packets_sent_copy = emulated_market.packets_sent();
    packets_sent_copy.remove_if([](PacketSent ps) { return ps.owner == "ccst"; } );
    return packets_sent_copy;
}
size_t dice_roll() {
    return (rand() % 6) + 1;
}

std::list<flow> random_users(size_t number)
{
    std::list<flow> toRet { };
    for (size_t i = 0; i < number; i ++)
    {
        toRet.push_back( { std::string(1,'A'+i), dice_roll(), dice_roll() } );
    }
    return toRet;
}

int main(){
    size_t num_matched = 0;
    size_t num_didnt_match = 0;
    for (int i = 0; i < 100; i++)
    {
        std::list<flow> usrs = random_users( 3 );
        auto market = sim_brute_force_users(usrs, false);
        auto srtf = simulate_shortest_remaining_time_first(usrs);
        usrs.reverse();
        assert(has_minimum_queueing_time( usrs, srtf ));
        usrs.reverse();
        if (has_minimum_queueing_time( usrs, market ))
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
