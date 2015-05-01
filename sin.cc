#include "sin.hh"
#include <iostream>
#include <cassert>
#include "user.hh"
#include "market.hh"

using namespace std;

int main(){
    cout << "hello world" << endl;
    Market mkt(10, 1);
    std::vector<struct User> users;

    time_t last_time = 0;
    time_t cur_time = time(nullptr);
    while (not mkt.empty()) {
        cur_time = time(nullptr);
        // advance time if we havent already for that same time
        if (cur_time != last_time) {
            mkt.advance_time();
            last_time = cur_time;
            mkt.print_order_book();
        } else {
            for (struct User &u : users) {
                auto buyer = u.get_buyer_identity();
                auto market_view = mkt.give_order_book(buyer);

                for (auto &slot_action : u.take_actions(market_view))
                {
                    cout << slot_action.slot_time << endl;

                // let user add bids
                // let user delete bids
                // let user add offers for slots they own 
                // let user give packet to send to slot they own
                }
            }
        }
    }
    return 1;
}
