/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include "flow.hh"
#include "link.hh"
#include "market_simulator.hh"
#include "allocation_analysis.hh"
#include "srtf_simulator.hh"
#include "round_robin_simulator.hh"

#include <getopt.h>

using namespace std;

tuple<double, double, double> run_single_trial( /*vector<Link> &links,*/ const vector<Flow> flows, const size_t verbosity_level )
{
    const size_t slots_needed = 1024;
    const size_t propagation_time = 0;
    const double default_interval_cost = 1;
    const vector<Link> links { { 0, 0, slots_needed, propagation_time, default_interval_cost } };

    const vector<Interval> market_allocation = simulate_market( links, flows, verbosity_level > 0 );
    const vector<Interval> srtf_allocation = simulate_srtf( links, flows );
    const vector<Interval> round_robin_allocation = simulate_round_robin( links, flows );

    double market_mean_flow_duration = mean_flow_duration( flows, market_allocation );
    double srtf_mean_flow_duration = mean_flow_duration( flows, srtf_allocation );
    double round_robin_mean_flow_duration = mean_flow_duration( flows, round_robin_allocation );

    if ( verbosity_level > 0 ) {
        print_flows( flows );
        cout << "market mean flow duration " << market_mean_flow_duration  << " with allocation:" << endl;
        print_intervals( market_allocation );

        cout << "srtf mean flow duration " << srtf_mean_flow_duration << " with allocation:" << endl;
        print_intervals( srtf_allocation );

/*
        cout << "round robin mean flow duration " << round_robin_mean_flow_duration << " with allocation:" << endl;
        print_intervals( round_robin_allocation );
        */
    }
    
    // TODO improve this
    return tie<double, double, double>( market_mean_flow_duration, srtf_mean_flow_duration, round_robin_mean_flow_duration );
}

size_t dice_roll( const size_t die_size )
{
    return ( rand() % die_size ) + 1;
}

const vector<Flow> make_random_flows( const size_t num_flows, const size_t die_size )
{
    vector<Flow> toRet { };
    for ( size_t i = 0; i < num_flows; i++ ) {
        toRet.push_back( { i+1, dice_roll( die_size ), dice_roll( die_size ) } );
    }

    // normalize them to 0 start time
    size_t min_start_time = 133337;
    for ( auto &flow : toRet ) {
        min_start_time = min( min_start_time, flow.start );
    }
    for ( auto &flow : toRet ) {
        flow.start = flow.start - min_start_time;
    }
    return toRet;
}

Flow make_flow_from_string( const size_t uid, const string &token )
{
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

vector<Flow> make_flows_from_string( const string &arg )
{
    size_t uid = 1;
    vector<Flow> flows;
    size_t last = 0;
    size_t next = 0;
    while ((next = arg.find(",", last)) != string::npos) {
        flows.push_back( make_flow_from_string( uid++, arg.substr( last, next - last ) ) );
        last = next + 1;
    }
    flows.push_back( make_flow_from_string( uid++, arg.substr( last ) ) );
    return flows;
}

void run_random_trials( const size_t num_flows, const size_t num_trials, const size_t die_size, const size_t verbosity_level )
{
    double market_mean_flow_duration = 0;
    double srtf_mean_flow_duration = 0;
    double round_robin_mean_flow_duration = 0;
    for (size_t i = 0; i < num_trials; i++) {
        vector<Flow> flows = make_random_flows( num_flows, die_size );

        double market, srtf, round_robin;
        tie<double,double,double>( market, srtf, round_robin ) = run_single_trial( flows, verbosity_level );
        market_mean_flow_duration += market;
        srtf_mean_flow_duration += srtf;
        round_robin_mean_flow_duration += round_robin;

        if ( verbosity_level >= 1 ) {
            cout << "finished trial " << i+1 << " of " << num_trials << endl << endl;
        }
    }
    market_mean_flow_duration /= (double) num_trials;
    srtf_mean_flow_duration /= (double) num_trials;
    round_robin_mean_flow_duration /= (double) num_trials;
    cout << market_mean_flow_duration << " " << srtf_mean_flow_duration << " " << round_robin_mean_flow_duration << endl;
}

void usage_error( const string & program_name )
{
    cerr << "Usage: " << program_name << " --num-flows=NUMBER --num-trials=NUMBER --die-size=NUMBER ";
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
        size_t num_trials = 1, num_flows = 0;
        size_t verbosity_level = 0;

        const option command_line_options[] = {
            { "simulate",             required_argument, nullptr, 's' },
            { "v",                          no_argument, nullptr, 'v' },
            { "vv",                         no_argument, nullptr, 'w' },
            { "die-size",             required_argument, nullptr, 'd' },
            { "num-flows",            required_argument, nullptr, 'u' },
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
                num_flows = stoul( optarg );
                assert( num_flows );
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
            if ( num_flows and num_trials and die_size > 0 ) {
                if ( verbosity_level > 0 ) {
                    cout << "running " << num_trials << " trials with " << num_flows;
                    cout << " flows and die size..." << die_size << endl;
                }
                run_random_trials( num_flows, num_trials, die_size, verbosity_level );
            } else  {
                usage_error( argv[ 0 ] );
            }
        } else {
            /* run scenario from user supplied string */
            verbosity_level = max( size_t(1), verbosity_level ); /* or else we print nothing and it looks weird */
            run_single_trial( make_flows_from_string( sim_string ), verbosity_level );
        }

    } catch ( const exception & e ) {
        cerr << "got expection: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
