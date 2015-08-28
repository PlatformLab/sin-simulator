/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include <cassert>
#include <map>
#include <unordered_set>
#include "abstract_user.hh"
#include "owner_user.hh"
#include "flow_completion_time_user.hh"
#include "old_flow_completion_time_user.hh"
#include "random_evil_user.hh"
#include "market_simulator.hh"
#include "market.hh"

#include "schedule.hh"
#include "shortest_remaining_time_first.hh"
#include "round_robin.hh"

#include <getopt.h>

using namespace std;

const deque<Interval> sim_users(list<flow> usr_args, const size_t verbosity_level )
{
    vector<unique_ptr<AbstractUser>> usersToSimulate;

    if ( verbosity_level >= 2 ) {
        cout << "trial shorthand= ";
        cout << usr_args.front().flow_start_time << ":" << usr_args.front().num_packets;
        bool first = true;
        for ( auto & u : usr_args )
        {
            if ( verbosity_level >= 2 and not first ) {
                cout << ", " << u.flow_start_time << ":" << u.num_packets;
            }
            first = false;
        }
        cout << endl << endl;

        for ( auto & u : usr_args ) {
            cout << "User: " << uid_to_string( u.uid ) << " flow start time: " << u.flow_start_time;
            cout << " num packets: " << u.num_packets << endl;
        }
    }

    for ( auto & u : usr_args ) {
            usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( u.uid, u.flow_start_time, u.num_packets ));
    }

    // hack to size number of slots for market simulation based on time to complete srtf
    size_t slots_needed = simulate_shortest_remaining_time_first(usr_args).back().time + 2;
    if ( add_evil_user ) {
        slots_needed += 20;
    }

    usersToSimulate.emplace_back(make_unique<OwnerUser>( 0, 1, slots_needed, true ));

    MarketSimulator simulated_market( move(usersToSimulate), ( verbosity_level >= 3 ));

    simulated_market.run_to_completion();

    if ( verbosity_level >= 2 ) {
        cout << endl;
        simulated_market.print_user_stats();
    }

    return toRet;
}

/* returns sum flow completion time for market and SRTF simulations */
pair<size_t, size_t> run_single_trial( list<flow> usr_args, const size_t verbosity_level, const bool old_style_user, const bool round_robin_user, double &worst_let_down, size_t &worst_srtf_divergence, size_t &total_market_updates, const bool add_evil_user )
{
    auto market = sim_users(usr_args, verbosity_level, old_style_user, round_robin_user, worst_let_down, total_market_updates, add_evil_user );
    auto srtf = simulate_shortest_remaining_time_first(usr_args);
    return {1337, 1337};
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

void run_random_trials( const size_t num_users, const size_t num_trials, const size_t die_size, const size_t verbosity_level )
{
    for (size_t i = 0; i < num_trials; i++)
    {
        list<flow> user_args =  make_random_users( num_users, die_size );

        pair<size_t, size_t> sum_flow_completion_times = run_single_trial( user_args, verbosity_level );

        if ( verbosity_level >= 1 ) {
            cout << "finished trial " << i+1 << " of " << num_trials << endl << endl;
        }
    }
}

flow make_user_from_string( const size_t uid, const string &token )
{
    //cout << "making user from: " << token << endl;

    auto delimiter_idx = token.find_first_of( ":" );
    if ( delimiter_idx == std::string::npos ) {
        throw runtime_error( "invalid arguments" );
    }
    size_t flow_start_time = stoul( token.substr( 0, delimiter_idx ) );
    size_t flow_num_packets = stoul( token.substr( delimiter_idx + 1 ) );
    /* TODO extra chars after number ignored */
    
    //cout << " got start time " << flow_start_time << " and num packets "<<  flow_num_packets << endl;
    return { uid, flow_start_time , flow_num_packets };
}

list<flow> make_users_from_string( const string &arg )
{
    size_t uid = 1;
    list<flow> users;
    size_t last = 0;
    size_t next = 0;
    while ((next = arg.find(",", last)) != string::npos) {
        users.push_back( make_user_from_string( uid++, arg.substr( last, next - last ) ) );
        last = next + 1;
    }
    users.push_back( make_user_from_string( uid++, arg.substr( last ) ) );
    return users;
}

void usage_error( const string & program_name )
{
    cerr << "Usage: " << program_name << " --num-users=NUMBER --num-trials=NUMBER --die-size=NUMBER ";
    cerr << "| --simulate=TRIAL_STRING" << endl;
    cerr << endl;
    cerr << "       TRIAL_STRING = \"START:DURATION[, START2:DURATION2, ...]\"" << endl;
    cerr << endl;
    cerr << "Options = --v --vv" << endl;
    cerr << "          --random-seed=NUMBER" << endl;
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
        size_t die_size = 0;
        size_t num_trials = 1, num_users = 0;
        size_t verbosity_level = 0;

        const option command_line_options[] = {
            { "simulate",             required_argument, nullptr, 's' },
            { "v",                          no_argument, nullptr, 'v' },
            { "vv",                         no_argument, nullptr, 'w' },
            { "die-size",             required_argument, nullptr, 'd' },
            { "num-users",            required_argument, nullptr, 'u' },
            { "num-trials",           required_argument, nullptr, 't' },
            { "random-seed",          required_argument, nullptr, 'z' },
            { 0,                                        0, nullptr, 0 }
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
                verbosity_level = 1;
                break;
            case 'w':
                verbosity_level = 2;
                break;
            case 'd':
                die_size = stoul( optarg );
                assert( die_size );
                break;
            case 'u':
                num_users = stoul( optarg );
                assert( num_users );
                break;
            case 't':
                num_trials = stoul( optarg );
                assert( num_trials );
                break;
            case 'z':
                /* sets seed for rand */
                srand( 7 * stoul( optarg ) );
                break;
            case '?':
                usage_error( argv[ 0 ] );
                break;
            default:
                throw runtime_error( "getopt_long: unexpected return value " + to_string( opt ) );
            }
        }

        if ( sim_string == "" ) {
            if ( num_users and num_trials and flow_length_die_size > 0 and start_time_die_size > 0 ) {
                cout << "running " << num_trials << " trials with " << num_users;
                cout << " users and die size..." << die_size << endl;
                run_random_trials( num_users, num_trials, die_size, verbosity_level );
            } else  {
                usage_error( argv[ 0 ] );
            }
        } else {
            /* run scenario from user supplied string */
            verbosity_level = max( size_t(1), verbosity_level ); /* or else we print nothing and it looks weird */
            run_single_trial( make_users_from_string( sim_string ), verbosity_level );
        }

    } catch ( const exception & e ) {
        cerr << "got expection: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
