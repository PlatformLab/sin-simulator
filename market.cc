#include "market.hh"
#include <algorithm>

using namespace std;

void Market::advance_time()
{
    assert(not order_book.empty());
    struct Slot &front = order_book.front();

    cout << "slot at time " << front.time << " awarded to " << front.owner << endl;
    front.if_packet_sent();

    // delete front slot
    order_book.pop_front();

    // maybe add new slot to back
}

void Market::match_bids_and_orders()
{
    for (size_t i = 0; i < order_book.size(); i++)
    {
        struct Slot &slot = order_book.at(i);

        // need bids and offers for slot to do something
        if (slot.bids.empty() or slot.offers.empty()) {
            continue;
        }

        // get highest bid and award slot to that bidder if more than lowest offer price for that slot
        if (slot.highest_bid().cost > slot.lowest_offer().cost) {
            struct BidOffer winner = slot.highest_bid();
            slot.owner = winner.owner;
            slot.if_packet_sent = winner.if_packet_sent;
            slot.offers.clear();
            slot.bids.clear();

            slot.if_offer_taken(i);

            winner.if_bid_wins(i);
            slot.if_offer_taken = winner.if_offer_taken;
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
        cout << slot.time << ". ";

        if (slot.owner != "")
            cout << slot.owner;
        else
            cout << "null";

        auto lowest_offer = std::min_element(slot.offers.begin(), slot.offers.end(), compare_two_bidoffers);
        cout << " $";
        if (lowest_offer != slot.offers.end())
            cout << lowest_offer->cost;
        else
            cout << "null";
    }

    cout << " ]" << endl;
}
