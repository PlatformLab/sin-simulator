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


MarketEmulator::MarketEmulator( vector<unique_ptr<AbstractUser>> &&users, const string &default_user, uint32_t default_slot_offer, size_t total_num_slots )
: mkt_(), users_(move(users))
{
    for (size_t slot_time = 0; slot_time < total_num_slots; slot_time++) {
        // have user do this, not in emulator
        mkt_.owner_add_slot(default_user, slot_time);
        mkt_.mutable_order_book().back().add_offer({ default_slot_offer, default_user });
    }
}

void MarketEmulator::run_to_completion()
{ 
    cout << "run to completion" << endl;
    size_t cur_time = 0;

    while (not mkt_.mutable_order_book().empty() ) { //and not allusers__done()) {
        // instead loop until no user takes an action

        // differentiate flows and users, users exist at all time but flows given to users

        for (int i = 0; i < 1; i++)
        {
            for ( auto & u : users_ ) {
                u->take_actions(mkt_);
                print_slots(mkt_.mutable_order_book());
            }
        }
        cout << endl;

        cur_time++;
        mkt_.advance_time();
    }

    // print stats
    for ( auto & u : users_ ) {
        u->print_stats(mkt_);
    }

    cout << "sent packets are ";
    print_slots(mkt_.sent_slots());

}
