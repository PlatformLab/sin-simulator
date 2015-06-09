/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SRTF_HH
#define SRTF_HH

#include <iostream>
#include <unordered_map>

#include "market_event.hh"
#include "pretty_print.hh"

struct flow {
    std::string name;
    size_t flow_start_time;
    size_t num_packets;
};

const std::vector<PacketSent> simulate_shortest_remaining_time_first( std::list<flow> flows )
{
    std::vector<PacketSent> final_schedule;
    uint64_t cur_time = 0;

    while ( not flows.empty() )
    {
        auto shortest_remaining_time_flow = flows.end();
        for (auto it = flows.begin(); it != flows.end();  ++it )
        {
            // we can schedule this flow at this time
            if (it->flow_start_time <= cur_time)
            {
                if (shortest_remaining_time_flow == flows.end()
                        || it->num_packets < shortest_remaining_time_flow->num_packets)
                {
                    shortest_remaining_time_flow = it;
                }
            }
        }

        // we can schedule a flow now (otherwise they all have too high of start times
        if (shortest_remaining_time_flow != flows.end())
        {
            final_schedule.push_back( {shortest_remaining_time_flow->name, cur_time} );

            // delete flow from list if flow completed
            shortest_remaining_time_flow->num_packets--;
            if (shortest_remaining_time_flow->num_packets == 0)
            {
                flows.erase(shortest_remaining_time_flow);
            }
        }
        cur_time++;
    }

    // now print results
//    printPacketsSent(final_schedule);
    return final_schedule;
}

// figures out how much extra queuing time a schedule has over optimal
std::pair<size_t, size_t> queueing_delay_of_schedule_and_optimal( std::list<flow> flows, std::vector<PacketSent> schedule )
{
    std::unordered_map<std::string, size_t> schedule_flow_completion_times;
    for (auto & packet_sent : schedule) {
        schedule_flow_completion_times[packet_sent.owner] = packet_sent.time;
    }

    std::unordered_map<std::string, size_t> srtf_flow_completion_times;
    for (const auto & packet_sent : simulate_shortest_remaining_time_first(flows)) {
        srtf_flow_completion_times[packet_sent.owner] = packet_sent.time;
    }

    size_t shedule_queuing_delay = 0;
    size_t srtf_queuing_delay = 0;
    for (auto & flow : flows) {
        shedule_queuing_delay += schedule_flow_completion_times[flow.name] - flow.flow_start_time;
        srtf_queuing_delay += srtf_flow_completion_times[flow.name] - flow.flow_start_time;
    }
    assert (shedule_queuing_delay >= srtf_queuing_delay);
    return std::make_pair(shedule_queuing_delay, srtf_queuing_delay);
}

#endif /* SRTF_HH */
