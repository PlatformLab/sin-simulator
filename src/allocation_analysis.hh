/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ALLOCATION_ANALYSIS_HH
#define ALLOCATION_ANALYSIS_HH

#include <iostream>
#include <unordered_map>
#include <vector>
#include <cassert>
#include "interval.hh"

/* checks that all flows have had their packets allocated */
static bool allocation_valid( const std::vector<Flow> &flows, const std::vector<Interval> &allocation )
{
    std::unordered_map<size_t, size_t> uid_to_num_sent;
    for ( auto &i : allocation ) {
        auto search = uid_to_num_sent.find( i.owner );

        if ( search == uid_to_num_sent.end() ) {
            uid_to_num_sent.emplace( i.owner, i.num_packets );
        } else {
            search->second += i.num_packets;
        }
    }

    for (auto &flow : flows ) {
        auto search = uid_to_num_sent.find( flow.uid );
        if ( search == uid_to_num_sent.end() ) {
            std::cout << "failed because uid not found" << std::endl;
            return false;
        } else if ( search->second != flow.num_packets ) {
            std::cout << "num packets recorded was " << search->second << " for flow of size " << flow.num_packets << std::endl;
        }
    }

    return true;
}

/* returns a map of uid to flow num packets and duration */
std::unordered_map<size_t, std::pair<size_t, size_t>> get_flow_lengths_and_durations(
        const std::vector<Flow> &flows, const std::vector<Interval> &allocation )
{
    std::unordered_map<size_t, std::pair<size_t, size_t>> toRet;
    for ( auto &i : allocation ) {
        toRet[ i.owner ] = { 0, 0 };
    }

    // find flow completion time
    for ( auto &i : allocation ) {
        toRet[ i.owner ].second = std::max( i.end, toRet[ i.owner ].second );
    }

    // record flow duration and calculate flow duration from flow completion time we found above
    for (auto &flow : flows ) {
        if ( toRet.count( flow.uid ) > 0 ) { // only count flow if it shows up in intevals
            toRet[ flow.uid ].first = flow.num_packets;
            toRet[ flow.uid ].second += 1 - flow.start;
        }
    }

    // filter out intevals not allocated to flows
    for ( auto it = toRet.begin(); it != toRet.end(); ) {
        if ( it->second.first == 0 ) {
            it = toRet.erase( it );
        } else {
            ++it;
        }
    }

    return toRet;
}

double mean_flow_duration( const std::vector<Flow> &flows, const std::vector<Interval> &allocation )
{
    assert( allocation_valid( flows, allocation ) );
    size_t sum_durations = 0;
    for ( const auto &flow_stats : get_flow_lengths_and_durations( flows, allocation ) ) {
        sum_durations += flow_stats.second.second;
    }
    return (double) sum_durations / (double) flows.size();
}

#endif /* ALLOCATION_ANALYSIS_HH */
