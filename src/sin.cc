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

const deque<PacketSent> sim_users(list<flow> usr_args, const size_t verbosity_level, const bool old_style_user, const bool round_robin_user, double &worst_let_down, size_t &total_market_updates, const bool add_evil_user )
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

    if ( round_robin_user ) {
        return simulate_round_robin( usr_args );
    }

    for ( auto & u : usr_args ) {
        if ( old_style_user ) {
            usersToSimulate.emplace_back(make_unique<OldFlowCompletionTimeUser>( u.uid, u.flow_start_time, u.num_packets ));
        } else {
            usersToSimulate.emplace_back(make_unique<FlowCompletionTimeUser>( u.uid, u.flow_start_time, u.num_packets ));
        }
    }
    if ( add_evil_user )
    {
        usersToSimulate.emplace_back(make_unique<RandomEvilUser>( usersToSimulate.back()->uid_ + 1 ));
        if ( verbosity_level >= 2 ) {
            cout << "Evil user: " << uid_to_string( usersToSimulate.back()->uid_ ) << endl;
        }
    }

    // hack to size number of slots for market simulation based on time to complete srtf
    size_t slots_needed = simulate_shortest_remaining_time_first(usr_args).back().time + 2;
    if ( add_evil_user ) {
        slots_needed += 20;
    }

    usersToSimulate.emplace_back(make_unique<OwnerUser>( 0, 1, slots_needed, true ));

    MarketSimulator simulated_market( move(usersToSimulate), ( verbosity_level >= 3 ), false );

    simulated_market.run_to_completion();


    double sum_user_utilities = simulated_market.sum_user_utilities();
    double sum_user_best_expected_utilities = simulated_market.sum_user_best_expected_utilities();

    if ( abs( sum_user_best_expected_utilities - sum_user_utilities ) > 1e-9 ) {
        worst_let_down = max( worst_let_down, sum_user_best_expected_utilities - sum_user_utilities );
    }

    total_market_updates += simulated_market.total_market_updates();
    if ( verbosity_level >= 2 ) {
        cout << endl;
        simulated_market.print_user_stats();
        cout << endl;
        cout << "Sum user utility was " << sum_user_utilities << " while sum of best expected utilties was ";
        cout << sum_user_best_expected_utilities << " (let down of ";
        cout << sum_user_best_expected_utilities - sum_user_utilities << ")" << endl << endl;
        cout << "market required " << simulated_market.total_market_updates() << " updates" << endl;
    }

    // now clean up results by getting rid of hard coded owner
    deque<PacketSent> toRet = {};
    for (auto &ps : simulated_market.packets_sent()) {
        if (ps.owner != 0) {
            toRet.emplace_back(ps);
        }
    }

    if ( not add_evil_user ) {
        assert( simulated_market.sum_user_benefits() == - (double) schedule_sum_flow_durations( usr_args, toRet ) );
    }
    return toRet;
}

/* returns sum flow completion time for market and SRTF simulations */
pair<size_t, size_t> run_single_trial( list<flow> usr_args, const size_t verbosity_level, const bool old_style_user, const bool round_robin_user, double &worst_let_down, size_t &worst_srtf_divergence, size_t &total_market_updates, const bool add_evil_user )
{
    auto market = sim_users(usr_args, verbosity_level, old_style_user, round_robin_user, worst_let_down, total_market_updates, add_evil_user );
    auto srtf = simulate_shortest_remaining_time_first(usr_args);

    const bool print_individual_flow_durations = verbosity_level >= 2;
    if ( print_individual_flow_durations ) {
        for (auto &fct_pair : schedule_flow_durations( usr_args, market ) ) {
            cout << "User: " << uid_to_string( fct_pair.first ) << " had flow duration " <<
                fct_pair.second << " in simulated schedule" << endl;
        }
        for (auto &fct_pair : schedule_flow_durations( usr_args, srtf ) ) {
            cout << "User: " << uid_to_string( fct_pair.first ) << " had flow duration " <<
                fct_pair.second << " in srtf schedule" << endl;
        }
    }
    size_t market_sum_fcts = schedule_sum_flow_durations( usr_args, market );
    size_t srtf_sum_fcts = schedule_sum_flow_durations( usr_args, srtf );
    assert( market_sum_fcts >= srtf_sum_fcts );

    if ( verbosity_level >= 2 ) {
        if ( market_sum_fcts == srtf_sum_fcts ) {
            cout << "Trial matched srtf, trial was:"<< endl;
            printSlots(market);
            cout << "and srtf was:" << endl;
            printSlots(srtf);
        } else {
            cout << "trial didn't match srtf! Trial was:"<< endl;
            printSlots(market);
            cout << "and srtf was:" << endl;
            printSlots(srtf);

            cout << "Trial had " << market_sum_fcts - srtf_sum_fcts << " longer sum flow duration than srtf ( ";
            cout << market_sum_fcts << "-" << srtf_sum_fcts << " )" << endl;
        }
    }
    worst_srtf_divergence = max( worst_srtf_divergence, market_sum_fcts - srtf_sum_fcts );
    return { market_sum_fcts, srtf_sum_fcts };
}

size_t dice_roll( const size_t die_size )
{
    return ( rand() % die_size ) + 1;
}

list<flow> make_random_users( const size_t num_users, const size_t start_time_die_size, const size_t flow_length_die_size )
{
    list<flow> toRet { };
    for ( size_t i = 0; i < num_users; i++ )
    {
        toRet.push_back( { i+1, dice_roll( start_time_die_size ), dice_roll( flow_length_die_size ) } );
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

void run_random_trials( const size_t num_users, const size_t num_trials, const size_t start_time_die_size, const size_t flow_length_die_size, const size_t verbosity_level, const bool old_style_user, const bool round_robin_user, const bool add_evil_user )
{
    size_t market_matched_srtf = 0;
    size_t market_didnt_match_srtf = 0;
    size_t total_market_sum_fcts = 0;
    size_t total_srtf_sum_fcts = 0;
    double worst_let_down = numeric_limits<double>::lowest();
    size_t worst_srtf_divergence = 0;
    size_t total_market_updates = 0;
    size_t total_packets = 0;

    for (size_t i = 0; i < num_trials; i++)
    {
        list<flow> user_args =  make_random_users( num_users, start_time_die_size, flow_length_die_size );

        for ( auto& flow : user_args ) {
            total_packets += flow.num_packets;
        }

        pair<size_t, size_t> sum_flow_completion_times = run_single_trial( user_args, verbosity_level, old_style_user, round_robin_user, worst_let_down, worst_srtf_divergence, total_market_updates, add_evil_user );
        total_market_sum_fcts += sum_flow_completion_times.first;
        total_srtf_sum_fcts += sum_flow_completion_times.second;
        if ( sum_flow_completion_times.first == sum_flow_completion_times.second ) {
            market_matched_srtf++;
        } else {
            market_didnt_match_srtf++;
        }

        if ( verbosity_level >= 1 ) {
            cout << "finished trial " << i+1 << " of " << num_trials << endl << endl;
        }
    }
    cout << market_matched_srtf << " of " << market_matched_srtf + market_didnt_match_srtf  << " scenarios matched the srtf result ( ";
    cout << (double) market_matched_srtf / (double) ( market_matched_srtf + market_didnt_match_srtf ) << " )" << endl;
    cout << "average delay ratio " << ( (double) total_market_sum_fcts / (double) total_srtf_sum_fcts ) << endl;
    cout << "average flow duration for srtf " << (double) total_srtf_sum_fcts / (double) ( num_users * num_trials ) << endl;
    cout << "average flow duration for trials " << (double) total_market_sum_fcts / (double) ( num_users * num_trials ) << endl;
    cout << "average number of market updates per user " << (double) total_market_updates / (double) ( num_users * num_trials ) << endl;

    cout << "average number of market updates per packet " << (double) total_market_updates / (double) total_packets << endl;

    if ( not round_robin_user ) {
        if ( worst_let_down == numeric_limits<double>::lowest() ) {
            cout << "all users in all trials got best utility they thought they could get" << endl;
        } else {
            cout << "largest difference from best expected utilities to final utilities (utility let down) was " << worst_let_down << endl;
        }
    }
    cout << "worst srtf divergence was " << worst_srtf_divergence << endl;
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
    cerr << "Usage: " << program_name << " --num-users=NUMBER --num-trials=NUMBER --start-time-die-size=NUMBER ";
    cerr << "--flow-length-die-size=NUMBER | --simulate=TRIAL_STRING" << endl;
    cerr << endl;
    cerr << "       TRIAL_STRING = \"START:DURATION[, START2:DURATION2, ...]\"" << endl;
    cerr << endl;
    cerr << "Options = --v --vv --vvv" << endl;
    cerr << "          --round-robin-user" << endl;
    cerr << "          --old-style-user" << endl;
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
        size_t start_time_die_size = 0, flow_length_die_size = 0;
        size_t num_trials = 1, num_users = 0;
        size_t verbosity_level = 0;
        bool old_style_user = false, round_robin_user = false, add_evil_user = false;

        const option command_line_options[] = {
            { "simulate",             required_argument, nullptr, 's' },
            { "v",                          no_argument, nullptr, 'v' },
            { "vv",                         no_argument, nullptr, 'w' },
            { "vvv",                        no_argument, nullptr, 'x' },
            { "old-style-user",             no_argument, nullptr, 'o' },
            { "round-robin-user",           no_argument, nullptr, 'r' },
            { "start-time-die-size",  required_argument, nullptr, 'd' },
            { "flow-length-die-size", required_argument, nullptr, 'f' },
            { "num-users",            required_argument, nullptr, 'u' },
            { "num-trials",           required_argument, nullptr, 't' },
            { "random-seed",          required_argument, nullptr, 'z' },
            { "add-evil-user",              no_argument, nullptr, 'e' },
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
            case 'x':
                verbosity_level = 3;
                break;
            case 'o':
                old_style_user = true;
                break;
            case 'r':
                round_robin_user = true;
                break;
            case 'd':
                start_time_die_size = stoul( optarg );
                assert( start_time_die_size );
                break;
            case 'f':
                flow_length_die_size = stoul( optarg );
                assert( flow_length_die_size );
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
            case 'e':
                add_evil_user = true;
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
                cout << " users and start time die size " << start_time_die_size;
                cout << " and start time die size " << flow_length_die_size << "..." << endl;
                run_random_trials( num_users, num_trials, start_time_die_size, flow_length_die_size,
                        verbosity_level, old_style_user, round_robin_user, add_evil_user );
            } else  {
                usage_error( argv[ 0 ] );
            }
        } else {
            /* run scenario from user supplied string */
            verbosity_level = max( size_t(2), verbosity_level ); /* or else we print nothing and it looks weird */
            double worst_let_down = numeric_limits<double>::lowest();
            size_t worst_srtf_divergence = 0;
            size_t total_market_updates = 0;
            for (size_t i = 0; i < num_trials; i++) {
                run_single_trial( make_users_from_string( sim_string ), verbosity_level, old_style_user, round_robin_user, worst_let_down, worst_srtf_divergence, total_market_updates, add_evil_user );
            }
            if ( worst_let_down != numeric_limits<double>::lowest() ) {
                cout << "worst let down was " << worst_let_down << endl;
            }
            cout << "average number of market updates per user " << (double) total_market_updates / (double) ( num_users * num_trials ) << endl;
        }

    } catch ( const exception & e ) {
        cerr << "got expection: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
