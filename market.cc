#include "market.hh"

using namespace std;

Market::Market(uint32_t num_future_slots, uint32_t default_slot_price)
    : order_book_size(num_future_slots),
     cur_time(0),
     default_slot_price(default_slot_price)
{
    for (uint32_t time = 0; time < num_future_slots; time++) {
        order_book.emplace(time, default_slot_price);
    }
}

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
    assert(order_book.front().time == cur_time);  

    if (order_book.front().packet) {
        cout << "sending packet at time " << cur_time << " with contents: " << *order_book.front().packet << endl;
    } else {
        cout << "no packet to send at time " << cur_time << endl;
    }
    order_book.pop();
    //struct slot to_add{
    //order_book.emplace_back();
    order_book.emplace(cur_time+order_book_size, default_slot_price);

    cur_time++;
}

std::queue<struct Slot> Market::get_order_book()
{
    return order_book;
}
