#include "sin.hh"
#include <iostream>
#include <cassert>
#include "abstract_user.hh"
#include "noop_user.hh"
#include "market.hh"

using namespace std;

int main(){
    cout << "hello world" << endl;
    Market mkt;
    std::vector<AbstractUser*> users;
    users.emplace_back(new NoopUser());
    const time_t market_time_window = 10;

    time_t last_time = 0;
    time_t cur_time = time(nullptr);

    for (time_t time_to_add = cur_time; time_to_add < cur_time+market_time_window; time_to_add++)
        mkt.owner_add_slot(time_to_add);

    while (not mkt.get_order_book().empty()) {
        cur_time = time(nullptr);
        // advance time if we havent already for that same time
        if (cur_time != last_time) {
            mkt.match_bids_and_orders();

            mkt.advance_time();
            mkt.owner_add_slot(cur_time + market_time_window);

            last_time = cur_time;
            mkt.print_order_book();

            for (AbstractUser *u : users) {
                u->take_actions(mkt);
            }
        }
    }
    return 1;
}
