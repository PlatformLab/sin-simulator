/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market_emulator.hh"

using namespace std;

MarketEmulator::MarketEmulator( std::vector<EmulatedUser> &&users, const std::string &default_user, uint32_t default_slot_offer, size_t total_num_slots )
: mkt(), users(std::move(users))
{
    for (size_t slot_time = 0; slot_time < total_num_slots; slot_time++) {
        mkt.owner_add_slot(default_user, slot_time);
        mkt.order_book().back().add_offer({ default_slot_offer, default_user });
    }
}

static size_t get_last_packet_sent_time(const std::deque<Slot> &sent_slots, const std::string &name)
{
    size_t highest_sent_time = 0;
    for (auto & slot : sent_slots) {
        if (slot.owner == name) {
            highest_sent_time = slot.time;
        }
    }
    return highest_sent_time;
}

void MarketEmulator::run_to_completion()
{ 
    cout << "run to completion" << endl;
    size_t cur_time = 0;

    while (not mkt.order_book().empty()) {
        cout << "in while" << endl;
        for (int i = 0; i < 1; i++)
        {
            for ( auto & u : users ) {
                if (u.time_to_start <= cur_time)
                {
                    u.user->take_actions(mkt);
                    mkt.print_slots(mkt.order_book());
                }
            }
        }
        cout << endl;

        cur_time++;
        mkt.advance_time();
    }

    // print stats
    for ( auto & u : users ) {
        cout << "user " << u.user->name << " started at time " << u.time_to_start << " and finished at time "
        << get_last_packet_sent_time(mkt.sent_slots(), u.user->name) << endl; // need to track money spent
    }
    cout << "sent packets are ";
    mkt.print_slots(mkt.sent_slots());

}
