/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SCHEDULE
#define SCHEDULE

#include <unordered_map>

#include "market_event.hh"

struct flow {
    std::string name;
    size_t flow_start_time;
    size_t num_packets;
};

std::unordered_map<std::string, size_t> schedule_flow_completion_times( std::list<flow> flows, std::deque<PacketSent> schedule )
{
    std::unordered_map<std::string, size_t> toRet;
    for (auto & packet_sent : schedule) {
        toRet[packet_sent.owner] = packet_sent.time;
    }
    for (auto &flow : flows ) {
        toRet[flow.name] += 1 - flow.flow_start_time;
    }
    return toRet;
}

size_t schedule_sum_flow_completion_times( std::list<flow> flows, std::deque<PacketSent> schedule )
{
    size_t shedule_queuing_delay = 0;
    for (auto &fct_pair : schedule_flow_completion_times( flows, schedule ) ) {
        shedule_queuing_delay += fct_pair.second;
    }
    return shedule_queuing_delay;
}

#endif /* SCHEDULE */
