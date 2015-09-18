/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ALLOCATION_ANALYSIS_HH
#define ALLOCATION_ANALYSIS_HH

#include <iostream>
#include <unordered_map>
#include <vector>
#include <cassert>
#include "interval.hh"

/* checks that all flows got to send all their packets */
static bool allocation_valid( const std::unordered_map<Flow, std::vector<Opportunity>> &allocation )
{
    for ( auto &allocation_pair : allocation ) {
        size_t num_packets_sent = 0;
        for ( const Opportunity &o : allocation_pair.second ) {
            num_packets_sent += o.num_packets;
        }
        if ( num_packets_sent != allocation_pair.first.num_packets ) {
            std::cout << "got num_packets_sent " << num_packets_sent << " while flow " << uid_to_string( allocation_pair.first.uid ) << " was supposed to have " <<  allocation_pair.first.num_packets << std::endl;
            return false;
        }
    }
    return true;
}

double mean_flow_duration( const std::unordered_map<Flow, std::vector<Opportunity>> &allocation )
{
    assert( allocation_valid( allocation ) );

    size_t sum_durations = 0;
    for ( auto &allocation_pair : allocation ) {
        size_t flow_start = allocation_pair.first.start;
        if ( not allocation_pair.second.empty() ) {
            size_t flow_end = allocation_pair.second.at(0).interval.end;
            for ( const Opportunity &o : allocation_pair.second ) {
                flow_end = std::max( flow_end, o.interval.end );
            }
            size_t flow_duration = flow_end - flow_start + 1; // TODO should it really be + 1?
            sum_durations += flow_duration;
        }
    }

    size_t num_flows = allocation.size();
    return (double) sum_durations / (double) num_flows;
}

#endif /* ALLOCATION_ANALYSIS_HH */
