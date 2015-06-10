/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ROUND_ROBIN_HH
#define ROUND_ROBIN_HH

#include <iostream>
#include <unordered_map>

#include "market_event.hh"
#include "pretty_print.hh"
#include "shortest_remaining_time_first.hh"

const std::vector<PacketSent> simulate_round_robin( std::list<flow> flows ) // copy of flows so we can decrement
{
    std::vector<PacketSent> final_schedule;
    uint64_t cur_time = 0;

    while ( true )
    {
        bool someone_went = false;
        bool everyone_finished = true;

        for (auto &flow : flows) {
            bool flow_finished = flow.num_packets == 0;
            everyone_finished = everyone_finished and flow_finished;

            bool flow_started = cur_time >= flow.flow_start_time;

            if (flow_started and not flow_finished) { // then schedule it
                final_schedule.push_back( {flow.name, cur_time} );
                flow.num_packets--;
                cur_time++;
                someone_went = true;
            }
        }

        if (everyone_finished) {
            return final_schedule;
        }
        if (not someone_went) { // if nobody could go then advance time with no packet sent
            cur_time++;
        }
    }
}

#endif /* ROUND_ROBIN_HH */
