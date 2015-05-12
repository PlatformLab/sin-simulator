/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"
#include <algorithm>

using namespace std;

void Market::advance_time()
{
    assert(not _order_book.empty());
    struct Slot &front = _order_book.front();

    cout << "slot at time " << front.time << " awarded to " << front.owner << endl;

    // add slot to sent_slots and delete from order book
    _sent_slots.push_back(move(front));
    _order_book.pop_front();
}

static void Market::print_slots(const deque<Slot> &slots)
{
    cout << "[ ";
    bool is_first = true;
    for (Slot &slot : slots)
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
