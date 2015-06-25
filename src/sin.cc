/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include <cassert>
#include <map>
#include <unordered_set>
#include "abstract_user.hh"
#include "owner_user.hh"
#include "flow_completion_time_user.hh"
#include "market_simulator.hh"
#include "market.hh"

#include "schedule.hh"
#include "shortest_remaining_time_first.hh"
#include "round_robin.hh"

using namespace std;

double worst_let_down = std::numeric_limits<double>::lowest();
size_t dice_roll_num_sides = 1000;

const deque<PacketSent> sim_users(list<flow> usr_args, const bool print_start_stats, const bool run_verbose, const bool print_end_stats )
{
    vector<unique_ptr<AbstractUser>> usersToSimulate;
    if ( print_start_stats ) {
        cout << "trial shorthand: ";
    }
    for (auto & u : usr_args)
    {
    if ( print_start_stats ) {
        cout <<u.flow_start_time << u.num_packets;
    }
    }
    if ( print_start_stats ) {
    cout << endl;
    }
    for (auto & u : usr_args)
    {
    if ( print_start_stats ) {
        cout << "User: " << u.uid << " flow start time: " << u.flow_start_time << " num packets: " << u.num_packets << endl;
    }
        usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( u.uid, u.flow_start_time, u.num_packets ));
    }

    // hack to size number of slots for market simulation based on time to complete srtf
    size_t slots_needed = simulate_shortest_remaining_time_first(usr_args).back().time + 2;

    usersToSimulate.emplace_back(make_unique<OwnerUser>( 0, 1, slots_needed, true ));

    MarketSimulator simulated_market( move(usersToSimulate), run_verbose, false );

    simulated_market.run_to_completion();
    //cout << "market required " << simulated_market.total_roundtrips() << " round trips" << endl;

    // now clean up results by getting rid of hard coded owner
    deque<PacketSent> toRet = {};
    for (auto &ps : simulated_market.packets_sent()) {
        if (ps.owner != 0) {
            toRet.emplace_back(ps);
        }
    }
    assert( simulated_market.sum_user_benefits() == - (double) schedule_sum_flow_completion_times( usr_args, toRet ) );
    if ( print_end_stats ) {
        simulated_market.print_user_stats();
        double sum_user_utilities = simulated_market.sum_user_utilities();
        double sum_user_best_expected_utilities = simulated_market.sum_user_best_expected_utilities();
        cout << "sum user utility was " << sum_user_utilities << " while sum of best expected utilties was " << sum_user_best_expected_utilities << " the difference between these is " << sum_user_best_expected_utilities -sum_user_utilities << endl;
        if ( abs( sum_user_best_expected_utilities - sum_user_utilities ) > 1e-9 ) {
            cout << "some users got let down " << endl;
            worst_let_down = max( worst_let_down, sum_user_best_expected_utilities - sum_user_utilities );
            cout << "running again in detail " << endl;
            for (auto & u : usr_args)
            {
                usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( u.uid, u.flow_start_time, u.num_packets ));
            }
            usersToSimulate.emplace_back(make_unique<OwnerUser>( 0, 1, slots_needed, true ));

            MarketSimulator simulated_market( move(usersToSimulate), true, false );

            simulated_market.run_to_completion();
        }
    }
    return toRet;
}

size_t dice_roll()
{
    return ( rand() % dice_roll_num_sides ) + 1;
}

list<flow> make_random_users(size_t number)
{
    list<flow> toRet { };
    for (size_t i = 0; i < number; i ++)
    {
        toRet.push_back( { i+1, dice_roll(), dice_roll() } );
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

void thousandThreeUserDSixTest() {
    size_t old_num_sides = dice_roll_num_sides;
    dice_roll_num_sides = 6;

    size_t market_matched_srtf = 0;
    size_t market_didnt_match_srtf = 0;
    size_t total_market_delay = 0;
    size_t total_srtf_delay = 0;

    for (int i = 0; i < 1000; i++)
    {
        list<flow> usr_args =  make_random_users( 3 ); // makes this number of random users for market
        auto market = sim_users(usr_args, false, false, false);
        auto srtf = simulate_shortest_remaining_time_first(usr_args);

        size_t market_sum_fcts = schedule_sum_flow_completion_times( usr_args, market );
        size_t srtf_sum_fcts = schedule_sum_flow_completion_times( usr_args, srtf );

        assert( market_sum_fcts >= srtf_sum_fcts );

        total_market_delay += market_sum_fcts;
        total_srtf_delay += srtf_sum_fcts;

        if ( market_sum_fcts == srtf_sum_fcts ) {
            market_matched_srtf++;
        } else {
            market_didnt_match_srtf++;
        }
    }
    assert( market_matched_srtf == 984 );
    assert( total_srtf_delay == 15666 );
    assert( total_market_delay == 15690 );

    dice_roll_num_sides = old_num_sides;
}

int main(){
    thousandThreeUserDSixTest();

    size_t market_matched_srtf = 0;
    size_t market_didnt_match_srtf = 0;
    size_t total_market_delay = 0;
    size_t total_srtf_delay = 0;

    /*
    size_t round_robin_num_matched = 0;
    size_t round_robin_num_didnt_match = 0;
    size_t total_round_robin_delay = 0;
    */

    const int num_trials = 1000;
    for (int i = 0; i < num_trials; i++)
    {
        list<flow> usr_args =  make_random_users( 10 ); // makes this number of random users for market
        if ( i <= 884 )
            continue;
        //list<flow> usr_args = {{1, 0, 1000}, {2, 1, 998}, {3, 1, 997}, {4, 1, 996}, {5, 1, 995}}; // make_random_users( 3 ); // makes this number of random users for market
        /*
        unordered_set<size_t> start_times;
        bool has_two_starting_at_same_time = false;
        for ( auto &arg : usr_args )
        {
            has_two_starting_at_same_time |= start_times.count(arg.flow_start_time);
            start_times.insert(arg.flow_start_time);
        }
        if ( has_two_starting_at_same_time ) {
            continue;
        }
        */
        auto market = sim_users(usr_args, true, true, false);
        auto srtf = simulate_shortest_remaining_time_first(usr_args);

        size_t market_sum_fcts = schedule_sum_flow_completion_times( usr_args, market );
        size_t srtf_sum_fcts = schedule_sum_flow_completion_times( usr_args, srtf );
        assert( market_sum_fcts >= srtf_sum_fcts );
        total_market_delay += market_sum_fcts;
        total_srtf_delay += srtf_sum_fcts;

        if ( market_sum_fcts == srtf_sum_fcts ) {
            market_matched_srtf++;
            cout << "market matched srtf, market was:"<< endl;
            printSlots(market);
        } else {
            market_didnt_match_srtf++;

            cout << "market didnt match srtf! Market was:"<< endl;
            printSlots(market);
            cout << "and srtf was:" << endl;
            printSlots(srtf);

            cout << "market had " << market_sum_fcts - srtf_sum_fcts << " less benefit than srtf" << endl;
        }
        auto market2 = sim_users(usr_args, false, false, true);
        assert(market2 == market);

        cout << "finished trial " << i << " of " << num_trials << endl << endl;
    }
    cout << market_matched_srtf << " of " << market_matched_srtf + market_didnt_match_srtf  << " scenarios matched the srtf result" << endl;
    cout << "average delay ratio " << ((double) total_market_delay / (double) total_srtf_delay) << endl;
    cout << "worst utility let down was " << worst_let_down << endl;
    return 1;
}
