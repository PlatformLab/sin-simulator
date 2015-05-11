/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market_emulator.hh"

using namespace std;

MarketEmulator::MarketEmulator( std::vector<EmulatedUser> &&users, const std::string &default_user, uint32_t default_slot_offer, size_t total_num_slots )
: mkt(), users_to_add(std::move(users))

{
    for (size_t slot_time = 0; slot_time < total_num_slots; slot_time++) {
        mkt.owner_add_slot(default_user, slot_time);
        mkt.get_order_book().back().add_offer({default_slot_offer, default_user, [](){}});
    }
}

void MarketEmulator::run_to_completion()
{ 
    cout << "run to completion" << endl;
    size_t cur_time = 0;

    while (not mkt.get_order_book().empty()) {
        for (auto &u : users_to_add)
        {
            if (u.time_to_start == cur_time) 
            {
                active_users.emplace_back(std::move(u.user));
            }

        }

        for (int i = 0; i < 1; i++)
        {
            for ( auto & u : active_users ) {
                u->take_actions(mkt);
                mkt.print_order_book();
            }
        }
        cout << endl;

        cur_time++;
        mkt.advance_time();
    }
}
