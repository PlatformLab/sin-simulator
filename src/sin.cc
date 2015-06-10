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
#include "round_robin.hh"

using namespace std;

// makes a utility function given a flow size
function<int(const list<size_t>&, size_t, size_t)> get_utility_func(size_t num_packets)
{
    return [num_packets] ( const list<size_t>& times_could_own, size_t flow_start_time, size_t packets_already_sent )
    {
        if (packets_already_sent >= num_packets)
        {
            cout << "BIIG prob: packets_already_sent " << packets_already_sent  << " num_packets " << num_packets;
            cout << " flow start time " << flow_start_time << " times coud own size " <<  times_could_own.size() << endl;
        }
        assert(packets_already_sent < num_packets);
        if ( (times_could_own.size() + packets_already_sent) < num_packets ) {
            return numeric_limits<int>::min();
        } else {
            assert(not times_could_own.empty());
            assert(max_element(times_could_own.begin(), times_could_own.end()) != times_could_own.end());
            return - (int) (*max_element(times_could_own.begin(), times_could_own.end()) - flow_start_time);
        }
    };
}

const vector<PacketSent> sim_brute_force_users(list<flow> usr_args, const bool verbose)
{
    vector<unique_ptr<AbstractUser>> usersToEmulate;
    for (auto & u : usr_args)
    {
        if (verbose) {
            cout << "User: " << u.name << " flow start time: " << u.flow_start_time << " num packets: " << u.num_packets << endl;
        }
        usersToEmulate.emplace_back(make_unique<BruteForceUser>( u.name, u.flow_start_time, u.num_packets, get_utility_func( u.num_packets )));
    }

    size_t slots_needed = simulate_shortest_remaining_time_first(usr_args).back().time + 2; // also hack
    usersToEmulate.emplace_back(make_unique<OwnerUser>( "ccst", 1, slots_needed, true ));

    MarketEmulator emulated_market(move(usersToEmulate), verbose);

    emulated_market.run_to_completion();
    if (verbose) {
        emulated_market.print_money_exchanged();
        //emulated_market.print_packets_sent();
    }

    vector<PacketSent> toRet = {};
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

list<flow> random_users(size_t number)
{
    list<flow> toRet { };
    for (size_t i = 0; i < number; i ++)
    {
        toRet.push_back( { string(1,'A'+i), dice_roll(), dice_roll() } );
    }

    size_t min_start_time = 1337;
    for (auto &flow : toRet) {
        min_start_time = min(min_start_time, flow.flow_start_time);
    }
    for (auto &flow : toRet) {
        flow.flow_start_time = flow.flow_start_time - min_start_time;
    }
    return toRet;
}

int main(){
    size_t num_matched = 0;
    size_t num_didnt_match = 0;
    size_t total_market_delay = 0;
    size_t total_srtf_delay = 0;

    /*
    size_t round_robin_num_matched = 0;
    size_t round_robin_num_didnt_match = 0;
    size_t total_round_robin_delay = 0;
    */

    double worst_delay_ratio = 0;

    for (int i = 0; i < 10000; i++)
    {
        list<flow> usr_args = random_users( 3 );
        auto market = sim_brute_force_users(usr_args, false);

        auto delay_pair = queueing_delay_of_schedule_and_optimal( usr_args, market );
        total_market_delay += delay_pair.first;
        total_srtf_delay += delay_pair.second;
        size_t excess_delay = delay_pair.first - delay_pair.second;

        if (excess_delay == 0)
        {
            num_matched++;
        //    cout << "market matched srtf results!!!!!!!!!!!!!!!!!!!!!!" << endl;
        } else {
            num_didnt_match++;

            cout << "market didnt match srtf! Market:"<< endl;
            printPacketsSent(market);
            cout << "and srtf:" << endl;
            printPacketsSent(simulate_shortest_remaining_time_first(usr_args));
            cout << "market had " << excess_delay << " more queuing delay than srtf" << endl;

            double delay_ratio = (double) delay_pair.first / (double) delay_pair.second;
            worst_delay_ratio = max(delay_ratio, worst_delay_ratio);
            cout << "ratio of mean flow duration to optimal is: " << delay_ratio;
            if (delay_ratio == worst_delay_ratio) {
                cout << ". This is the worst seen so far in trials (market sum flow durations " << delay_pair.first << " / srtf sum flow durations " << delay_pair.second << ").";
            }

            cout << endl;

            cout << "in more detail, market results were:" << endl;
            auto market2 = sim_brute_force_users(usr_args, true);
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
    cout << num_matched << " of " << num_matched + num_didnt_match << " scenarios matched the srtf result" << endl;
    cout << "average delay ratio " << ((double) total_market_delay / (double) total_srtf_delay) << endl;
    cout << "worst delay ratio for market " << worst_delay_ratio << endl;
    /*
    cout << "for round robin: "<< round_robin_num_matched << " of " << round_robin_num_matched + round_robin_num_didnt_match << " scenarios matched the srtf result" << endl;
    cout << "average delay ratio " << ((double) total_round_robin_delay / (double) total_srtf_delay) << endl;
    */
    return 1;
}
