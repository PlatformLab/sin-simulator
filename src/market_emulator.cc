/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market_emulator.hh"

using namespace std;

bool MarketEmulator::all_users_finished()
{
    for ( auto & u : users ) {
        if (u.time_finished == (size_t) -1){
            return false;
        }
    }

    cout << "all users finished" << endl;
    return true;
}

MarketEmulator::MarketEmulator( std::vector<EmulatedUser> &&users, const std::string &default_user, uint32_t default_slot_offer, size_t total_num_slots )
: mkt(), users(std::move(users))
{
    for (size_t slot_time = 0; slot_time < total_num_slots; slot_time++) {
        mkt.owner_add_slot(default_user, slot_time);
        mkt.get_order_book().back().add_offer({default_slot_offer, default_user, [](){}});
    }

    for (auto &u : users)
    {
        u.time_finished = (size_t) -1;
    }
}

void MarketEmulator::run_to_completion()
{ 
    cout << "run to completion" << endl;
    size_t cur_time = 0;

    while (not mkt.get_order_book().empty() and not all_users_finished()) {
        cout << "in while" << endl;
        for (int i = 0; i < 1; i++)
        {
            for ( auto & u : users ) {
                if (u.time_to_start <= cur_time && not u.user->done())
                {
                    u.user->take_actions(mkt);
                    mkt.print_order_book();

                    if (u.user->done()){
                        u.time_finished = cur_time;
                    }
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
        << u.time_finished << endl; // need to track money spent
    }
}
