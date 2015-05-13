/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market_emulator.hh"

using namespace std;

static void print_slots(deque<SingleSlot> &slots)
{
    cout << "[ ";
    bool is_first = true;
    for (const SingleSlot &slot : slots)
    {
        if (is_first) {
            is_first = false;
        } else {
            cout << " | ";
        }
        cout << slot.time << ". ";

        if (slot.owner != "")
            cout << slot.owner;
        else
            cout << "null";

        cout << " $";
        if (slot.has_offers())
            cout << slot.best_offer().cost;
        else
            cout << "null";
    }

    cout << " ]" << endl;
}


MarketEmulator::MarketEmulator( vector<unique_ptr<AbstractUser>> &&users)
: mkt_(), users_(move(users))
{
}

void MarketEmulator::run_to_completion()
{ 
    cout << "run to completion" << endl;

    Market oldMkt;

    do {
        oldMkt = mkt_;
        do {
            oldMkt = mkt_;
            for ( auto & u : users_ ) {
                u->take_actions(mkt_);
                print_slots(mkt_.mutable_order_book());
            }
        } while (not (oldMkt == mkt_));

        if (not mkt_.mutable_order_book().empty()) {
            mkt_.advance_time();
        }
    } while (not (oldMkt == mkt_));

        // differentiate flows and users, users exist at all time but flows given to users

    // print stats
    for ( auto & u : users_ ) {
        u->print_stats(mkt_);
    }

    cout << "sent packets are ";
    print_slots(mkt_.sent_slots());
}
