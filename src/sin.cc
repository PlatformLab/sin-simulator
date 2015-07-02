/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include <cassert>
#include <map>
#include <unordered_set>
#include "abstract_user.hh"
#include "owner_user.hh"
#include "flow_completion_time_user.hh"
#include "old_flow_completion_time_user.hh"
#include "market_simulator.hh"
#include "market.hh"

#include "schedule.hh"
#include "shortest_remaining_time_first.hh"
#include "round_robin.hh"

#include <getopt.h>

using namespace std;

void usage_error( const string & program_name )
{
    cerr << "Usage: " << program_name << " UPLINK-TRACE DOWNLINK-TRACE [OPTION]... [COMMAND]" << endl;
    cerr << endl;
    cerr << "Options = --once" << endl;
    cerr << "          --uplink-log=FILENAME --downlink-log=FILENAME" << endl;
    cerr << "          --meter-uplink --meter-uplink-delay" << endl;
    cerr << "          --meter-downlink --meter-downlink-delay" << endl;
    cerr << "          --meter-all" << endl;
    cerr << "          --uplink-queue=QUEUE_TYPE --downlink-queue=QUEUE_TYPE" << endl;
    cerr << "          --uplink-queue-args=QUEUE_ARGS --downlink-queue-args=QUEUE_ARGS" << endl;
    cerr << endl;
    cerr << "          QUEUE_TYPE = infinite | droptail | drophead" << endl;
    cerr << "          QUEUE_ARGS = \"NAME=NUMBER[, NAME2=NUMBER2, ...]\"" << endl;
    cerr << "              (with NAME = bytes | packets)" << endl << endl;

    throw runtime_error( "invalid arguments" );
}

double worst_let_down = std::numeric_limits<double>::lowest();
size_t dice_roll_num_sides = 1000;

const deque<PacketSent> sim_users(list<flow> usr_args, const bool print_stats, const bool run_verbose, const bool old_style_user )
{
    vector<unique_ptr<AbstractUser>> usersToSimulate;

    if ( print_stats ) {
        cout << "trial shorthand: ";
    }
    for ( auto & u : usr_args )
    {
    if ( print_stats ) {
        cout <<u.flow_start_time << u.num_packets;
    }
    }
    if ( print_stats ) {
        cout << endl;
    }

    for ( auto & u : usr_args )
    {
        if ( print_stats ) {
            cout << "User: " << uid_to_string( u.uid ) << " flow start time: " << u.flow_start_time << " num packets: " << u.num_packets << endl;
        }

        if ( old_style_user ) {
            usersToSimulate.emplace_back(make_unique<OldFlowCompletionTimeUser>( u.uid, u.flow_start_time, u.num_packets ));
        } else {
            usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( u.uid, u.flow_start_time, u.num_packets ));
        }
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
    if ( print_stats ) {
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
        auto market = sim_users(usr_args, false, false, false );
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
    cout << "market_matched_srtf " << market_matched_srtf << " total_srtf_delay " << total_srtf_delay
    << " total_market_delay " << total_market_delay << endl;
    assert( market_matched_srtf == 984 );
    assert( total_srtf_delay == 15666 );
    assert( total_market_delay == 15690 );

    dice_roll_num_sides = old_num_sides;
}

int main( int argc, char *argv[] )
{
    try {
        if ( argc < 3 ) {
            usage_error( argv[ 0 ] );
        }
        string sim_string;
        size_t dice_roll, num_trials, num_users;
        bool print_stats, run_verbose = false;

        const option command_line_options[] = {
            { "simulate",           required_argument, nullptr, 's' },
            { "v",                        no_argument, nullptr, 'v' },
            { "vv",                       no_argument, nullptr, 'w' },
            { "dice-roll",          required_argument, nullptr, 'd' },
            { "num-users",          required_argument, nullptr, 'u' },
            { "num-trials",         required_argument, nullptr, 't' },
            { 0,                                    0, nullptr, 0 }
        };

        while ( true ) {
            const int opt = getopt_long( argc, argv, "u:d:", command_line_options, nullptr );
            if ( opt == -1 ) { 
                break;
            }

            switch ( opt ) {
            case 's':
                sim_string = optarg;
                break;
            case 'v':
                print_stats = true;
                print_stats &= false;
                break;
            case 'w':
                run_verbose = true;
                run_verbose  &= false;
                break;
            case 'd':
                dice_roll = stoul( optarg );
                dice_roll++;
                break;
            case 'u':
                num_users = stoul( optarg );
                num_users++;
                break;
            case 't':
                num_trials = stoul( optarg );
                num_trials++;
                break;
            case '?':
                usage_error( argv[ 0 ] );
                break;
            default:
                throw runtime_error( "getopt_long: unexpected return value " + to_string( opt ) );
            }
        }

        if ( optind + 1 >= argc ) {
            usage_error( argv[ 0 ] );
        }
    } catch ( const exception & e ) {
        cerr << "got expection: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
