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

static bool compare_two_bidoffers(struct BidOffer &a, struct BidOffer &b)
{
    return (a.cost < b.cost);
}

void Market::match_bids_and_orders()
{
    for (struct Slot &slot : order_book)
    {
        // need bids and offers for slot to do something
        if (slot.bids.empty() or slot.offers.empty()) {
            continue;
        }

        // get highest bid and award slot to that bidder if more than highest offer price for that slot
        auto highest_bid = std::max_element(slot.bids.begin(), slot.bids.end(), compare_two_bidoffers);
        auto highest_offer = std::max_element(slot.offers.begin(), slot.offers.end(), compare_two_bidoffers);

        if (highest_bid->cost > highest_offer->cost) {
            slot.owner = highest_bid->owner;
            slot.bids.clear();
            slot.offers.clear();
        }
    }
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
