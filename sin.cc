#include "sin.hh"
#include <iostream>
#include <cassert>
#include "market.hh"

using namespace std;

int main(){
    cout << "hello world" << endl;
    Market mkt(30, 1);

    time_t last_time = 0;
    time_t cur_time = time(nullptr);
    while (not mkt.get_order_book().empty()) {
        cur_time = time(nullptr);
        // advance time if we havent already for that same time
        if (cur_time != last_time) {
            mkt.advance_time();
            last_time = cur_time;
        }
    }
    return 1;
}
