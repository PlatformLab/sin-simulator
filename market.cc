#include "market.hh"
#include <algorithm>

using namespace std;

struct Slot* get_slot_at_time(deque<struct Slot> &order_book, uint32_t slot_time)
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

bool Market::add_bid(struct Buyer* buyer, uint64_t slot_time, uint32_t price)
{
    struct Slot *slot = get_slot_at_time(order_book, slot_time);

    if (slot) {
        struct Bid b = {price, buyer};
        slot->bids.emplace_back(b);
        return true;
    } else {
        return false;
    }
}

bool Market::delete_bids(struct Buyer* buyer, uint64_t slot_time)
{
    bool toRet = false;

    struct Slot *slot = get_slot_at_time(order_book, slot_time);

    if (slot) {
        // clear all bids made by buyer in this slot
        for(auto it = slot->bids.begin(); it != slot->bids.end(); )
            if (it->owner == buyer) {
                it = slot->bids.erase(it);
                toRet = true;
            } else {
                ++it;
            }
    }

    return toRet;
}

bool Market::add_offer(struct Buyer* buyer, uint64_t slot_time, uint32_t price)
{
    struct Slot *slot = get_slot_at_time(order_book, slot_time);

    if (slot && slot->owner == buyer) {
        slot->current_offer = price;
        return true;
    } else {
        return false;
    }
}

bool Market::add_packet(struct Buyer* buyer, uint64_t slot_time, string packet)
{
    struct Slot *slot = get_slot_at_time(order_book, slot_time);

    if (slot && slot->owner == buyer) {
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

// helper used for matching bids
static bool compare_two_bids(struct Bid &a, struct Bid &b)
{
    return (a.cost < b.cost);
}

void Market::match_bids_and_orders()
{
    for (struct Slot &slot : order_book)
    {
        // no bids for this slot
        if (slot.bids.empty()) {
            continue;
        }

        // get highest bid and award slot to that bidder if higher than current offer price for slot
        auto highest_bid = std::max_element(slot.bids.begin(), slot.bids.end(), compare_two_bids);
        if (highest_bid->cost > slot.current_offer) {
            slot.owner = highest_bid->owner;

            // not for sale at start
            slot.current_offer = uint32_t(-1);

            if (slot.packet != "") {
                cout << "dropping packet at slot " << slot.time << " with contents: " << slot.packet << endl;
            }

            slot.packet = "";
        }
    }
}

bool Market::empty()
{
    return order_book.empty();
}

void Market::print_order_book()
{
    cout << "[ ";
    for (struct Slot &slot : order_book)
    {
        cout << "Time:" << slot.time;

        cout << ", owner:";
        if (slot.owner)
            cout << slot.owner->name;
        else
            cout << "null";

        cout << ", cost: " << slot.current_offer;
        cout << " | ";
    }

    cout << "]" << endl;
}

std::vector<struct Slot_view> Market::give_order_book(struct Buyer &recipient)
{
    std::vector<struct Slot_view> toRet;

    for (struct Slot &slot : order_book)
    {
        toRet.emplace_back(slot.to_slot_view(recipient));
    }
    return toRet;
}
