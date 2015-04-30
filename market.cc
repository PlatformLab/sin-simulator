#include "market.hh"

using namespace std;

struct Slot* get_slot_at_time(deque<struct Slot> order_book, uint32_t slot_time)
{

    for (struct Slot &slot : order_book)
    {
        if (slot.time == slot_time) {
            return &slot;
        }
    }
    return nullptr;
}

Market::Market(uint32_t num_future_slots, uint32_t default_slot_price)
    : order_book_size(num_future_slots),
     cur_time(0),
     default_slot_price(default_slot_price)
{
    for (uint32_t time = 0; time < num_future_slots; time++) {
        order_book.emplace_back(time, default_slot_price);
    }
}

bool Market::add_bid(struct User* user, uint64_t slot_time, uint32_t price)
{
    struct Slot *slot = get_slot_at_time(order_book, slot_time);

    if (slot) {
        struct Bid b = {price, user};
        slot->bids.emplace_back(b);
        return true;
    } else {
        return false;
    }
}

bool Market::delete_bids(struct User* user, uint64_t slot_time)
{
    bool toRet = false;

    struct Slot *slot = get_slot_at_time(order_book, slot_time);

    if (slot) {
        // clear all bids made by user in this slot
        for(auto it = slot->bids.begin(); it != slot->bids.end(); )
            if (it->owner == user) {
                it = slot->bids.erase(it);
                toRet = true;
            } else {
                ++it;
            }
    }

    return toRet;
}

bool Market::add_offer(struct User* user, uint64_t slot_time, uint32_t price)
{
    struct Slot *slot = get_slot_at_time(order_book, slot_time);

    if (slot && slot->owner == user) {
        slot->current_offer = price;
        return true;
    } else {
        return false;
    }
}

bool Market::add_packet(struct User* user, uint64_t slot_time, string packet)
{
    struct Slot *slot = get_slot_at_time(order_book, slot_time);

    if (slot && slot->owner == user) {
        slot->packet = packet;
        return true;
    } else {
        return false;
    }
}

void Market::advance_time()
{
    assert(order_book.front().time == cur_time);  

    if (order_book.front().packet == "") {
        cout << "no packet to send at time " << cur_time << endl;
    } else {
        cout << "sending packet at time " << cur_time << " with contents: " << order_book.front().packet << endl;
    }

    // delete front slot
    order_book.pop_front();

    // add new slot to back
    order_book.emplace_back(cur_time+order_book_size, default_slot_price);

    cur_time++;
}

std::deque<struct Slot> Market::get_order_book()
{
    return order_book;
}
