#include "sin.hh"
#include <iostream>
#include <cassert>
#include "abstract_user.hh"
#include "noop_user.hh"
#include "market.hh"

using namespace std;

int main(){
    cout << "hello world" << endl;
    Market mkt(10, 1);
    std::vector<AbstractUser*> users;
    users.emplace_back(new NoopUser());

    time_t last_time = 0;
    time_t cur_time = time(nullptr);
    while (not mkt.empty()) {
        cur_time = time(nullptr);
        // advance time if we havent already for that same time
        if (cur_time != last_time) {
            mkt.match_bids_and_orders();
            mkt.advance_time();
            last_time = cur_time;
            mkt.print_order_book();
        //} else {
            for (AbstractUser *u : users) {
                string &user_name = u->get_name();
                auto market_view = mkt.give_order_book(user_name);

                struct User_actions actions = u->get_actions(market_view);

                bool success;
                // let user add bids
                for (auto &buy_bid : actions.buy_bids) {
                    success = mkt.add_bid(user_name, buy_bid.slot_time, buy_bid.price);
                    assert(success && "failure to add bid");
                }
                // let user delete bids
                for (auto &delete_bid : actions.delete_bids) {
                    success = mkt.delete_bids(user_name, delete_bid.slot_time);
                    assert(success && "failure to del bid");
                }
                // let user add offers for slots they own 
                for (auto &sell_offer : actions.sell_offers) {
                    success = mkt.add_offer(user_name, sell_offer.slot_time, sell_offer.price);
                    assert(success && "failure to add offer");
                }
                // let user give packet to send to slot they own
                for (auto &packet_to_send : actions.packets_to_send) {
                    success = mkt.add_packet(user_name, packet_to_send.slot_time, packet_to_send.packet);
                    assert(success && "failure to add packet");
                }
            }
        }
    }
    return 1;
}
