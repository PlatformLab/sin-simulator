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

void users_take_actions_until_finished( Market &mkt, std::vector<std::unique_ptr<AbstractUser>> &users, bool verbose )
{
    bool all_done = true;
    do {
        all_done = true;
        for ( auto &u : users ) {
            if ( u->start_ <= mkt.time() ) {
                size_t before_version = mkt.version();
                u->take_actions( mkt );
                bool market_unchanged = before_version == mkt.version();
                if ( verbose ) {
                    // stupid w-unused-parameter
                }

                all_done &= market_unchanged;
            }
        }
    } while ( not all_done );
}

const std::vector<Interval> simulate_market( const std::vector<Link> &links, const std::vector<Flow> &flows, bool verbose )
{
    Market mkt = Market( verbose );
    std::vector<std::unique_ptr<AbstractUser>> users;

    for ( const Link &link : links ) {
        users.push_back( std::make_unique<OwnerUser>( link ) );
    }
    for ( const Flow &flow : flows ) {
        users.push_back( std::make_unique<FlowCompletionTimeUser>( flow ) );
    }

    do {
        users_take_actions_until_finished( mkt, users, verbose );
        mkt.advance_time();
    } while ( not mkt.empty() );

    if ( verbose ) {
        std::cout << "market transactions:" << std::endl;
        print_transactions( mkt.transactions() );
    }

    std::vector<Interval> toRet;
    for ( auto &u : users ) {
        std::vector<Interval> toAdd = u->intervals();
        toRet.insert( toRet.end(), toAdd.begin(), toAdd.end() );
    }
    std::sort(toRet.begin(), toRet.end(), []( const Interval &a, const Interval &b ) {
            return a.end < b.end;   
            });
    return toRet;
}

#endif /* MARKET_SIMULATOR_HH */
