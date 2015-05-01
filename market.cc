#include "market.hh"
#include <algorithm>

using namespace std;

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

        // get highest bid and award slot to that bidder if more than lowest offer price for that slot
        auto highest_bid = std::max_element(slot.bids.begin(), slot.bids.end(), compare_two_bidoffers);
        auto lowest_offer = std::min_element(slot.offers.begin(), slot.offers.end(), compare_two_bidoffers);

        if (highest_bid->cost > lowest_offer->cost) {
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

        auto lowest_offer = std::min_element(slot.offers.begin(), slot.offers.end(), compare_two_bidoffers);
        if (lowest_offer != slot.offers.end())
            cout << ", lowest offer: " << lowest_offer->cost;
        else
            cout << ", no offers: ";
    }

    cout << " ]" << endl;
}
