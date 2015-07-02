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

const deque<PacketSent> sim_users(list<flow> usr_args, const bool print_stats, const bool run_verbose, const bool old_style_user, double &worst_let_down )
{
    vector<unique_ptr<AbstractUser>> usersToSimulate;

    if ( print_stats ) {
        cout << "trial shorthand= ";
        cout << usr_args.front().flow_start_time << ":" << usr_args.front().num_packets;
        bool first = true;
        for ( auto & u : usr_args )
        {
            if ( print_stats and not first ) {
                cout << ", " << u.flow_start_time << ":" << u.num_packets;
            }
            first = false;
        }
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

    double sum_user_utilities = simulated_market.sum_user_utilities();
    double sum_user_best_expected_utilities = simulated_market.sum_user_best_expected_utilities();

    if ( abs( sum_user_best_expected_utilities - sum_user_utilities ) > 1e-9 ) {
        worst_let_down = max( worst_let_down, sum_user_best_expected_utilities - sum_user_utilities );
    }

    if ( print_stats ) {
        simulated_market.print_user_stats();
        cout << "Sum user utility was " << sum_user_utilities << " while sum of best expected utilties was ";
        cout << sum_user_best_expected_utilities << " (let down of ";
        cout << sum_user_best_expected_utilities - sum_user_utilities << ")" << endl << endl;
    }

    // now clean up results by getting rid of hard coded owner
    deque<PacketSent> toRet = {};
    for (auto &ps : simulated_market.packets_sent()) {
        if (ps.owner != 0) {
            toRet.emplace_back(ps);
        }
    }

    assert( simulated_market.sum_user_benefits() == - (double) schedule_sum_flow_completion_times( usr_args, toRet ) );
    return toRet;
}

size_t dice_roll( const size_t die_size )
{
    return ( rand() % die_size ) + 1;
}

list<flow> make_random_users( const size_t num_users, const size_t die_size )
{
    list<flow> toRet { };
    for ( size_t i = 0; i < num_users; i++ )
    {
        toRet.push_back( { i+1, dice_roll( die_size ), dice_roll( die_size ) } );
    }

    // normalize them to 0 start time
    size_t min_start_time = 133337;
    for ( auto &flow : toRet ) {
        min_start_time = min( min_start_time, flow.flow_start_time );
    }
    for ( auto &flow : toRet ) {
        flow.flow_start_time = flow.flow_start_time - min_start_time;
    }
    return toRet;
}

void run_random_trials( const size_t num_users, const size_t num_trials, const size_t die_size, const bool print_stats, const bool run_verbose, const bool old_style_user )
{
    size_t market_matched_srtf = 0;
    size_t market_didnt_match_srtf = 0;
    size_t total_market_delay = 0;
    size_t total_srtf_delay = 0;
    double worst_let_down = numeric_limits<double>::lowest();

    for (size_t i = 0; i < num_trials; i++)
    {
        list<flow> usr_args =  make_random_users( num_users, die_size );

        auto market = sim_users(usr_args, print_stats, run_verbose, old_style_user, worst_let_down );
        auto srtf = simulate_shortest_remaining_time_first(usr_args);

        size_t market_sum_fcts = schedule_sum_flow_completion_times( usr_args, market );
        size_t srtf_sum_fcts = schedule_sum_flow_completion_times( usr_args, srtf );
        assert( market_sum_fcts >= srtf_sum_fcts );
        total_market_delay += market_sum_fcts;
        total_srtf_delay += srtf_sum_fcts;

        if ( market_sum_fcts == srtf_sum_fcts ) {
            market_matched_srtf++;
            if ( print_stats ) {
                cout << "market matched srtf, market was:"<< endl;
                printSlots(market);
            }
        } else {
            market_didnt_match_srtf++;

            if ( print_stats ) {
                cout << "market didnt match srtf! Market was:"<< endl;
                printSlots(market);
                cout << "and srtf was:" << endl;
                printSlots(srtf);

                cout << "market had " << market_sum_fcts - srtf_sum_fcts << " less benefit than srtf" << endl;
            }
        }

        if ( print_stats ) {
            cout << "finished trial " << i+1 << " of " << num_trials << endl << endl;
        }
    }
    cout << market_matched_srtf << " of " << market_matched_srtf + market_didnt_match_srtf  << " scenarios matched the srtf result" << endl;
    cout << "average delay ratio " << ( (double) total_market_delay / (double) total_srtf_delay ) << endl;
    if ( worst_let_down == numeric_limits<double>::lowest() ) {
        cout << "all users in all trials got best utility they thought they could get" << endl;
    } else {
        cout << "largest difference from best expected utilities to final utilities (utility let down) was " << worst_let_down << endl;
    }
}

void usage_error( const string & program_name )
{
    cerr << "Usage: " << program_name << " --num-users=NUMBER --num-trials=NUMBER --die-size=NUMBER | --simulate=TRIAL_STRING" << endl;
    cerr << endl;
    cerr << "       TRIAL_STRING = \"START:DURATION[, START2:DURATION2, ...]\"" << endl;
    cerr << endl;
    cerr << "Options = --v --vv --old-style-user" << endl;
    cerr << endl;
    throw runtime_error( "invalid arguments" );
}

int main( int argc, char *argv[] )
{
    try {
        if ( argc < 2 ) {
            usage_error( argv[ 0 ] );
        }
        string sim_string = "";
        size_t die_size = 0, num_trials = 0, num_users = 0;
        bool print_stats = false, run_verbose = false, old_style_user = false;

        const option command_line_options[] = {
            { "simulate",           required_argument, nullptr, 's' },
            { "v",                        no_argument, nullptr, 'v' },
            { "vv",                       no_argument, nullptr, 'w' },
            { "old-style-user",           no_argument, nullptr, 'o' },
            { "die-size",           required_argument, nullptr, 'd' },
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
                cout << "got: " << sim_string << endl;
                break;
            case 'v':
                print_stats = true;
                break;
            case 'w':
                print_stats = true;
                run_verbose = true;
                break;
            case 'o':
                old_style_user = true;
                break;
            case 'd':
                die_size = stoul( optarg );
                assert( dice_roll );
                break;
            case 'u':
                num_users = stoul( optarg );
                assert( num_users );
                break;
            case 't':
                num_trials = stoul( optarg );
                assert( num_trials );
                break;
            case '?':
                usage_error( argv[ 0 ] );
                break;
            default:
                throw runtime_error( "getopt_long: unexpected return value " + to_string( opt ) );
            }
        }

        if ( sim_string == "" ) {
            if ( num_users and num_trials and die_size ) {
                run_random_trials( num_users, num_trials, die_size, print_stats, run_verbose, old_style_user );
            } else  {
                usage_error( argv[ 0 ] );
            }
        } else {
            cout << "got here" << endl;
            /* user inputted string */
        }

    } catch ( const exception & e ) {
        cerr << "got expection: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
