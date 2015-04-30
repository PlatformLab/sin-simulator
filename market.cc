#include "market.hh"

using namespace std;

void Market::add_bid(struct User* user, uint64_t slot_time, uint32_t price)
{

}

bool Market::delete_bid(struct User* user, uint64_t slot_time)
{
return false;
}

void Market::add_offer(struct User* user, uint64_t slot_time, uint32_t price)
{
}

void Market::add_packet(struct User* user, uint64_t slot_time, string* packet)
{
}

void Market::advance_time()
{
    cur_time++;
    assert(order_book.front().time == cur_time);  

    if (order_book.front().packet) {
        cout << "sending packet at time " << cur_time << " with contents: " << *order_book.front().packet << endl;
    } else {
        cout << "no packet to send at time " << cur_time << endl;
    }
    order_book.pop();
    //struct slot to_add{
    //order_book.emplace_back();
}

std::queue<struct Slot> Market::get_order_book()
{
    return order_book;
}
