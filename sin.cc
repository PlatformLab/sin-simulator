#include "sin.hh"
#include <iostream>
#include <cassert>
#include "abstract_user.hh"
#include "noop_user.hh"
#include "basic_user.hh"
#include "market.hh"

using namespace std;

int main(){
    cout << "hello world" << endl;
    Market mkt;
    std::vector<AbstractUser*> users;
    users.emplace_back(new NoopUser());
    users.emplace_back(new BasicUser("gregs", 4));

    const time_t market_time_window = 10;
    const time_t base_time = time(nullptr);


    for (time_t slot_time = 1; slot_time <= market_time_window; slot_time++) {
        mkt.owner_add_slot("comcast", slot_time);
        mkt.get_order_book().back().add_offer({1, "comcast"});
    }

    time_t last_time = 0;
    time_t cur_time;
    while (not mkt.get_order_book().empty()) {
        cur_time = time(nullptr) - base_time;
        // advance time if we havent already for that same time
        if (cur_time != last_time) {
            mkt.match_bids_and_orders();

            mkt.advance_time();
            mkt.owner_add_slot("comcast", cur_time + market_time_window);
            mkt.get_order_book().back().add_offer({1, "comcast"});

            last_time = cur_time;
            mkt.print_order_book();

            for (AbstractUser *u : users) {
                u->take_actions(mkt);
            }
        }
    }
    return 1;
}
