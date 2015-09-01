/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
#include "flow.hh"
#include "link.hh"
#include "market_simulator.hh"
#include "allocation_analysis.hh"
#include "srtf_simulator.hh"

using namespace std;

int main() //( int argc, char *argv[] )
{
    size_t uid = 0;
    const size_t slots_needed = 20;
    const size_t propagation_time = 0;
    const double default_interval_cost = 1;

    const vector<Link> links { { uid++, slots_needed, propagation_time, default_interval_cost } };
    const vector<Flow> flows { { uid++, 0, 5 }, { uid++, 0, 4 }, { uid++, 0, 3 }, { uid++, 0, 2 } };

    vector<Interval> market_allocation = simulate_market( links, flows );
    vector<Interval> srtf_allocation = simulate_srtf( links, flows );

    print_flows( flows );
    cout << "market mean flow duration " << mean_flow_duration( flows, market_allocation ) << " with allocation:" << endl;
    print_allocation( market_allocation );
    cout << "srtf mean flow duration " << mean_flow_duration( flows, srtf_allocation ) << " with allocation:" << endl;
    print_allocation( srtf_allocation );
}
