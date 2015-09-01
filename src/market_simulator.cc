/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market_simulator.hh"

using namespace std;

MarketSimulator::MarketSimulator( std::vector<Link> &links, std::vector<Flow> &flows )
: mkt_()
{
    for ( auto &link : links ) {
        users_.push_back( make_unique<OwnerUser>( link ) );
    }
    for ( Flow &flow : flows ) {
        users_.push_back( make_unique<FlowCompletionTimeUser>( flow ) );
    }
}

static bool all_users_done(const vector<unique_ptr<AbstractUser>> &users, const Market &mkt)
{
    for ( const auto & u : users ) {
        if (not u->done( mkt )) {
            return false;
        }
    }
    return true;
}

void MarketSimulator::users_take_actions_until_finished()
{
    bool all_done = true;
    do
    {
        all_done = true;
        for ( auto &u : users_ ) {
            size_t before_version = mkt_.version();
            u->take_actions( mkt_ );
            bool market_unchanged = before_version == mkt_.version();

            all_done &= market_unchanged;
        }
    } while ( not all_done );
}

void MarketSimulator::run_to_completion()
{
    do
    {
        users_take_actions_until_finished();
        mkt_.advance_time();
    }
    while ( not mkt_.empty() );
}

void MarketSimulator::print_outcome()
{
    mkt_.print_intervals();
}
