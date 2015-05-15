/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market_emulator.hh"

using namespace std;

MarketEmulator::MarketEmulator( vector<unique_ptr<AbstractUser>> &&users)
: mkt_(), users_(move(users))
{
}

void MarketEmulator::run_to_completion()
{ 
    cout << "run to completion" << endl;

    Market oldMkt;

    do {
        do {
            oldMkt = mkt_;
            for ( auto & u : users_ ) {
                u->take_actions(mkt_);
            }
        } while (oldMkt != mkt_);

        mkt_.advance_time();
    } while (oldMkt != mkt_);

    // print stats
    for ( auto & u : users_ ) {
        u->print_stats(mkt_);
    }
}
