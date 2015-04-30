#include "sin.hh"
#include <iostream>
#include <cassert>
#include "market.hh"

using namespace std;

int main(){
    cout << "hello world" << endl;
    Market mkt(30, 1);
    while (not mkt.get_order_book().empty()) {
        mkt.advance_time();
    }
    return 1;
}
