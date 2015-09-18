/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef MARKET_SIMULATOR_HH
#define MARKET_SIMULATOR_HH

#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

#include "flow.hh"
#include "link.hh"
#include "pretty_print.hh"
#include "market.hh"
#include "abstract_user.hh"
#include "owner_user.hh"
#include "flow_completion_time_user.hh"

/* returns true if all flows are beyond their start time, this could infa loop XXX */
bool users_take_actions_until_finished( Market &mkt, const size_t time, std::vector<std::unique_ptr<AbstractUser>> &users, bool verbose )
{
    bool all_done = true;
    bool all_flows_started = true;
    do {
        all_done = true;
        for ( auto &u : users ) {
            if ( u->start_ <= time ) {
                size_t before_version = mkt.version();
                u->take_actions( mkt, time );
                bool market_unchanged = before_version == mkt.version();
                if ( verbose ) {
                    // stupid w-unused-parameter
                }

                all_done &= market_unchanged;
            } else {
                all_flows_started = false;
            }
        }
    } while ( not all_done );

    return all_flows_started;
}

const std::unordered_map<Flow, std::vector<Opportunity>> simulate_market( const std::vector<Link> &links, const std::vector<Flow> &flows, bool verbose )
{
    Market mkt = Market( verbose );
    std::vector<std::unique_ptr<AbstractUser>> users;
    std::unordered_map<size_t, Flow> uid_to_flow;
    size_t time = 0;

    for ( const Link &link : links ) {
        users.push_back( std::make_unique<OwnerUser>( link ) );
    }
    for ( const Flow &flow : flows ) {
        users.push_back( std::make_unique<FlowCompletionTimeUser>( flow ) );
        uid_to_flow[ flow.uid ] = flow;
    }

    bool all_flows_started = false;
    do {
        all_flows_started = users_take_actions_until_finished( mkt, time, users, verbose );
        time++;
    } while ( not all_flows_started );

    if ( verbose ) {
        std::cout << "market transactions:" << std::endl;
        print_transactions( mkt.transactions() );
    }

    std::unordered_map<Flow, std::vector<Opportunity>> allocation {};
    for ( auto &u : users ) {
        auto search = uid_to_flow.find( u->uid_ );
        if ( search != uid_to_flow.end() ) {
            allocation[ search->second ] = u->opportunities();
        }
    }
    return allocation;
}

#endif /* MARKET_SIMULATOR_HH */
