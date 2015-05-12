/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"
#include <algorithm>

using namespace std;

void Market::advance_time()
{
    assert(not order_book_.empty());
    struct Slot &front = order_book_.front();

    cout << "slot at time " << front.time << " awarded to " << front.owner << endl;

    // add slot to sent_slots and delete from order book
    sent_slots_.push_back(move(front));
    order_book_.pop_front();
}

static void print_slots(const deque<Slot> &slots)
{
    cout << "[ ";
    bool is_first = true;
    for (const Slot &slot : slots)
    {
        if (is_first) {
            is_first = false;
        } else {
            cout << " | ";
        }
        cout << slot.time << ". ";

        if (slot.owner != "")
            cout << slot.owner;
        else
            cout << "null";

        cout << " $";
        if (slot.has_offers())
            cout << slot.lowest_offer().cost;
        else
            cout << "null";
    }

    cout << " ]" << endl;
}
