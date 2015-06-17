/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include <cassert>
#include <map>
#include "abstract_user.hh"
#include "noop_user.hh"
#include "owner_user.hh"
#include "flow_completion_time_user.hh"
#include "market_emulator.hh"
#include "market.hh"

#include "shortest_remaining_time_first.hh"
#include "round_robin.hh"

using namespace std;

const deque<PacketSent> sim_users(list<flow> usr_args, const bool verbose, const bool random_user_order)
{
    vector<unique_ptr<AbstractUser>> usersToEmulate;
    for (auto & u : usr_args)
    {
        if (verbose) {
            cout << "User: " << u.name << " flow start time: " << u.flow_start_time << " num packets: " << u.num_packets << endl;
        }
        usersToEmulate.emplace_back(make_unique<FlowCompletionTimeUser>( u.name, u.flow_start_time, u.num_packets ));
    }

    // hack to size number of slots for market simulation based on time to complete srtf
    size_t slots_needed = simulate_shortest_remaining_time_first(usr_args).back().time + 2;

    usersToEmulate.emplace_back(make_unique<OwnerUser>( "ccst", 1, slots_needed, true ));

    MarketEmulator emulated_market( move(usersToEmulate), verbose, random_user_order);

    emulated_market.run_to_completion();
    //cout << "market required " << emulated_market.total_roundtrips() << " round trips" << endl;

    if (verbose) {
        unordered_map<string, double> net_balances = emulated_market.print_money_exchanged();
        emulated_market.print_packets_sent();

        std::unordered_map<std::string, size_t> flow_completion_times = schedule_flow_completion_times( usr_args, emulated_market.packets_sent() );
        for (auto &fct_pair : flow_completion_times) {
cout << "utility for " << fct_pair.first << " was " << -(double) fct_pair.second + net_balances[fct_pair.first] << endl;
        }
    }

    // now clean up results by getting rid of hard coded owner
    deque<PacketSent> toRet = {};
    for (auto &ps : emulated_market.packets_sent()) {
        if (ps.owner != "ccst") {
            toRet.emplace_back(ps);
        }
    }
    return toRet;
}
size_t dice_roll() {
    return (rand() % 6) + 1;
}

list<flow> make_random_users(size_t number)
{
    list<flow> toRet { };
    for (size_t i = 0; i < number; i ++)
    {
        toRet.push_back( { string(1,'A'+i), dice_roll(), dice_roll() } );
    }

    // normalize them to 0 start time
    size_t min_start_time = 133337;
    for (auto &flow : toRet) {
        min_start_time = min(min_start_time, flow.flow_start_time);
    }
    for (auto &flow : toRet) {
        flow.flow_start_time = flow.flow_start_time - min_start_time;
    }
    return toRet;
}

int main(){
    size_t market_matched_srtf = 0;
    size_t market_didnt_match_srtf = 0;
    size_t total_market_delay = 0;
    size_t total_srtf_delay = 0;

    /*
    size_t round_robin_num_matched = 0;
    size_t round_robin_num_didnt_match = 0;
    size_t total_round_robin_delay = 0;
    */

    double worst_delay_ratio = 0;

    for (int i = 0; i < 1000; i++) // number of trails
    {
        list<flow> usr_args = make_random_users( 3 ); // makes this number of random users for market
        auto market = sim_users(usr_args, false, false);
        auto srtf = simulate_shortest_remaining_time_first(usr_args);

        size_t market_delay = queueing_delay_of_schedule( usr_args, market );
        size_t srtf_delay = queueing_delay_of_schedule( usr_args, srtf );
        assert(market_delay >= srtf_delay);
        total_market_delay += market_delay;
        total_srtf_delay += srtf_delay;

        if (market_delay == srtf_delay)
        {
            market_matched_srtf++;
        } else {
            market_didnt_match_srtf++;

            cout << "market didnt match srtf! Market was:"<< endl;
            printSlots(market);
            cout << "and srtf was:" << endl;
            printSlots(srtf);
            cout << "market had " << market_delay - srtf_delay << " more queuing delay than srtf" << endl;

            double delay_ratio = (double) market_delay / (double) srtf_delay;
            worst_delay_ratio = max(delay_ratio, worst_delay_ratio);
            cout << "ratio of mean flow duration to optimal is: " << delay_ratio;
            if (delay_ratio == worst_delay_ratio) {
                cout << ". This is the worst seen so far in trials (market sum flow durations " << market_delay << " / srtf sum flow durations " << srtf_delay << ").";
            }

            cout << endl;

            cout << "in more detail, market results were:" << endl;
            auto market2 = sim_users(usr_args, true, false);
            assert(market2 == market);
            cout << "DONE" << endl << endl;
        }

        /*
        auto round_robin = simulate_round_robin(usr_args);
        auto round_robin_delay_pair = queueing_delay_of_schedule_and_optimal( usr_args, round_robin );
        total_round_robin_delay += round_robin_delay_pair.first;
        size_t round_robin_excess_delay = round_robin_delay_pair.first - round_robin_delay_pair.second;
        if (round_robin_excess_delay == 0) {
            round_robin_num_matched++;
            cout << "round robin matched srtf" << endl;
        } else {
            round_robin_num_didnt_match++;
            cout << "round robin didn't match" << endl;
        }
        */
    }
    cout << market_matched_srtf << " of " << market_matched_srtf + market_didnt_match_srtf  << " scenarios matched the srtf result" << endl;
    cout << "average delay ratio " << ((double) total_market_delay / (double) total_srtf_delay) << endl;
    cout << "worst delay ratio for market " << worst_delay_ratio << endl;
    /*
    cout << "for round robin: "<< round_robin_num_matched << " of " << round_robin_num_matched + round_robin_num_didnt_match << " scenarios matched the srtf result" << endl;
    cout << "average delay ratio " << ((double) total_round_robin_delay / (double) total_srtf_delay) << endl;
    */
    return 1;
}
