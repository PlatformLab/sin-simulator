#include "market.hh"
#include <algorithm>

using namespace std;

Market::Market(uint32_t num_future_slots)
    : order_book(num_future_slots)
{ }

void Market::advance_time()
{
    assert(not order_book.empty());
    struct Slot &front = order_book.front();

    cout << "slot at time " << front.time << " awarded to " << front.owner << endl;

    // delete front slot
    order_book.pop_front();

    // maybe add new slot to back
}

void Market::match_bids_and_orders()
{
    /*
    for (struct Slot &slot : order_book)
    {
        // no bids for this slot
        if (slot.bids.empty()) {
            continue;
        }

        // get highest bid and award slot to that bidder if higher than current offer price for slot
        auto highest_bid = std::max_element(slot.bids.begin(), slot.bids.end(), compare_two_bids);
        if (highest_bid->cost > slot.current_offer) {
            slot.owner.reset(new string(highest_bid->owner));

            // not for sale at start
            slot.current_offer = uint32_t(-1);

            if (slot.packet != "") {
                cout << "dropping packet at slot " << slot.time << " with contents: " << slot.packet << endl;
            }

            slot.packet = "";
        }
    }
    */
}

void Market::print_order_book()
{
    cout << "[ ";
    bool is_first = true;
    for (struct Slot &slot : order_book)
    {
        if (is_first) {
            is_first = false;
        } else {
            cout << " | ";
        }
        cout << "Time:" << slot.time;

        cout << ", owner:";
        if (slot.owner != "")
            cout << slot.owner;
        else
            cout << "null";

        //TODO cost
        //cout << ", cost: " << slot.current_offer;
    }

    cout << " ]" << endl;
}
