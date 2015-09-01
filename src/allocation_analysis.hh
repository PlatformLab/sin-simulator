/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ALLOCATION_ANALYSIS_HH
#define ALLOCATION_ANALYSIS_HH

#include <iostream>
#include <unordered_map>
#include <vector>
#include <cassert>
#include "interval.hh"

/* returns a map of uid to flow num packets and duration */
std::unordered_map<size_t, std::pair<size_t, size_t>> get_flow_lengths_and_durations( const std::vector<Flow> &flows, const std::vector<Interval> &allocation )
{
    std::unordered_map<size_t, std::pair<size_t, size_t>> toRet;
    for ( auto & interval : allocation ) {
        toRet[interval.owner].first = 0; // used to find intervals not associated with flows later
        toRet[interval.owner].second = interval.end;
    }

    for (auto &flow : flows ) {
        toRet[flow.uid].first = flow.num_packets;
        toRet[flow.uid].second += 1 - flow.start;
    }

    // filter out intervals not allocated to flows
    for ( auto &map_entry : toRet )
    {
        if ( map_entry.second.first == 0 ) {
            toRet.erase( map_entry.first );
        }
    }

    return toRet;
}

double mean_flow_duration( const std::vector<Flow> &flows, const std::vector<Interval> &allocation )
{
    size_t sum_durations = 0;
    for ( const auto &flow_stats : get_flow_lengths_and_durations( flows, allocation ) )
    {
        sum_durations += flow_stats.second.second;
    }
    return (double) sum_durations / (double) flows.size();
}

#endif /* ALLOCATION_ANALYSIS_HH */
