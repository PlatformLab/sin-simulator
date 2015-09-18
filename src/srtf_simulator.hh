/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SRTF_SIMULATOR_HH
#define SRTF_SIMULATOR_HH

#include <iostream>
#include <vector>
#include <list>

#include "flow.hh"
#include "opportunity.hh"
#include "link.hh"

const std::unordered_map<Flow, std::vector<Opportunity>> simulate_srtf( const std::vector<Link> &links, const std::vector<Flow> flows )
{
    assert ( links.size() == 1 );
    Link link = links.at(0);
    const std::vector<Opportunity> opportunities_to_schedule = link.get_opportunities();
    std::unordered_map<Flow, std::vector<Opportunity>> allocation {};
    std::unordered_map<Flow, size_t> packets_sent {};

    for ( const Opportunity &opportunity : opportunities_to_schedule ) {
        // first find shortest remaining time flow we can schedule
        auto srtf_flow = flows.end();
        size_t srtf_flow_packets_remaining = std::numeric_limits<size_t>::max();
        for ( auto potential_flow = flows.begin(); potential_flow != flows.end(); ++potential_flow ) {

            size_t potential_flow_packets_remaining = potential_flow->num_packets - packets_sent[ *potential_flow ];
            bool can_schedule_flow = potential_flow->start <= opportunity.interval.start;
            if ( can_schedule_flow and potential_flow_packets_remaining < srtf_flow_packets_remaining ) {
                srtf_flow = potential_flow;
                srtf_flow_packets_remaining = potential_flow_packets_remaining;
            }
        }

        // we can schedule a flow now (otherwise they all have later start times and we advance time with allocation)
        if ( srtf_flow != flows.end() ) {
            allocation[ *srtf_flow ].push_back( opportunity );
            packets_sent[ *srtf_flow ]++; // TODO does this start at 0?
        }
    }

    return allocation;
}

#endif /* SRTF_SIMULATOR_HH */
