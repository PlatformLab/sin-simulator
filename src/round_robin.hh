/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ROUND_ROBIN_HH
#define ROUND_ROBIN_HH

#include <iostream>
#include <unordered_map>

#include "market_event.hh"
#include "pretty_print.hh"
#include "shortest_remaining_time_first.hh"

const std::vector<PacketSent> simulate_round_robin( std::list<flow> flows )
{
    std::vector<PacketSent> final_schedule;
    uint64_t cur_time = 0;

    while ( true )
    {
        bool someone_went = false;
        bool all_finished = true;

        for (auto &flow : flows) { // copy
            bool finished = flow.num_packets == 0;
            all_finished = all_finished and finished;

            bool started = cur_time >= flow.flow_start_time;

            if (started and not finished) { // schedule it
                final_schedule.push_back( {flow.name, cur_time} );
                flow.num_packets--;
                cur_time++;
                someone_went = true;
            }
        }

        if (all_finished) {
            return final_schedule;
        }
        if (not someone_went) {
            cur_time++;
        }
    }
    assert(false);
    return final_schedule;
}

#endif /* ROUND_ROBIN_HH */
