/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market_emulator.hh"

using namespace std;

void MarketEmulator::run_to_completion()
{ 
    size_t cur_time = 0;

    size_t market_time_window = 10;

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

        mkt.advance_time();
        mkt.owner_add_slot("ccast", cur_time + market_time_window);
        mkt.get_order_book().back().add_offer({1, "ccast", [](){}});
    }
}
