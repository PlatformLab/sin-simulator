/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ALLOCATION_ANALYSIS_HH
#define ALLOCATION_ANALYSIS_HH

#include <iostream>
#include <unordered_map>
#include <vector>
#include <cassert>
#include "opportunity.hh"

/* returns a map of uid to flow num packets and duration */
std::unordered_map<size_t, std::pair<size_t, size_t>> get_flow_lengths_and_durations(
        const std::vector<Flow> &flows, const std::vector<Opportunity> &allocation )
{
    std::unordered_map<size_t, std::pair<size_t, size_t>> toRet;
    for ( auto & opportunity : allocation ) {
        toRet[opportunity.owner] = { 0, 0 };
    }

    // find flow completion time
    for ( auto & opportunity : allocation ) {
        toRet[opportunity.owner].second = std::max( opportunity.interval.end, toRet[opportunity.owner].second );
    }

    // record flow duration and calculate flow duration from flow completion time we found above
    for (auto &flow : flows ) {
        if ( toRet.count( flow.uid ) > 0 ) { // only count flow if it shows up in opportunties
            toRet[flow.uid].first = flow.num_packets;
            toRet[flow.uid].second += 1 - flow.start;
        }
    }

    // filter out opportunities not allocated to flows
    for ( auto it = toRet.begin(); it != toRet.end(); ) {
        if ( it->second.first == 0 ) {
            it = toRet.erase( it );
        } else {
            ++it;
        }
    }

    return toRet;
}

double mean_flow_duration( const std::vector<Flow> &flows, const std::vector<Opportunity> &allocation )
{
    size_t sum_durations = 0;
    for ( const auto &flow_stats : get_flow_lengths_and_durations( flows, allocation ) ) {
        sum_durations += flow_stats.second.second;
    }
    return (double) sum_durations / (double) flows.size();
}

#endif /* ALLOCATION_ANALYSIS_HH */
