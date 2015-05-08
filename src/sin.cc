#include <iostream>
#include <cassert>
#include "abstract_user.hh"
#include "basic_user.hh"
#include "market.hh"

using namespace std;

int main(){
    cout << "hello world" << endl;
    Market mkt;
    vector<unique_ptr<AbstractUser>> users; // unique_ptrs
    users.push_back( make_unique<BasicUser>( "gregs", mkt, 4 ) );
    users.push_back( make_unique<BasicUser>( "keith", mkt, 2 ) );

    const time_t market_time_window = 10;
    const time_t base_time = time(nullptr);


    for (time_t slot_time = 0; slot_time < market_time_window; slot_time++) {
        mkt.owner_add_slot("comcast", slot_time);
        //mkt.get_order_book().back().add_offer({10-(uint32_t) slot_time, "comcast"});
        mkt.get_order_book().back().add_offer({1, "comcast"});
    }

    std::cout << "initial order book:" << std::endl;
    mkt.print_order_book();
    std::cout << std::endl;

    time_t last_time = 0;
    time_t cur_time;
    while (not mkt.get_order_book().empty()) {
        cur_time = time(nullptr) - base_time;
        // advance time if we havent already for that same time
        if (cur_time != last_time) {
            for (int i = 0; i < 1; i++)
            {
                for ( auto & u : users ) {
                    u->take_actions();
                    mkt.print_order_book();
                }
            }
            cout << endl;

            mkt.advance_time();
            mkt.owner_add_slot("ccast", cur_time + market_time_window);
            mkt.get_order_book().back().add_offer({1, "ccast"});

            last_time = cur_time;
        }
    }
    return 1;
}
