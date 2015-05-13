/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "basic_user.hh" 

using namespace std;

static size_t num_slots_owned(deque<SingleSlot> &order_book, const string &name)
{
    size_t slots_owned = 0;
    for (auto &slot : order_book)
    {
        if (slot.owner == name)
            slots_owned++;
    }
    return slots_owned;
}

// used to figure out flow completion time at end
static size_t get_last_packet_sent_time(const std::deque<Slot> &sent_slots, const std::string &name)
{
    size_t highest_sent_time = 0;
    for (auto & slot : sent_slots) {
        if (slot.owner == name) {
            highest_sent_time = slot.time;
        }
    }
    return highest_sent_time;
}

static size_t current_flow_completion_time(deque<SingleSlot> &order_book, const string &name)
{
    size_t highest_owned_slot_idx = 0;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        if (order_book.at(i).owner == name) {
            highest_owned_slot_idx = i;
        }
    }
    return highest_owned_slot_idx;
}

static size_t flow_completion_time_if_sold(deque<SingleSlot> &order_book, const string &name, size_t idx_to_sell)
{
    size_t fct_if_sold = 0;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        if (order_book.at(i).owner == name && i != idx_to_sell) {
            fct_if_sold = i;
        }
    }
    return fct_if_sold;
}

BasicUser::BasicUser(const std::string &name, const size_t flow_start_time, const size_t num_packets)
: AbstractUser(name),
    flow_start_time(flow_start_time),
    num_packets(num_packets)
{
}

void BasicUser::add_offer_to_slot(Market &mkt, size_t at_idx)
{
    deque<SingleSlot> &order_book = mkt.mutable_order_book();
    SingleSlot &slot = order_book.at(at_idx);
    assert(slot.owner == name);

    // price and make offer
    vector<size_t> idxs_to_buy;
    size_t fct_if_sold = flow_completion_time_if_sold(order_book, name, at_idx);
    int utility_delta = recursive_pick_best_slots(order_book, 0, 1, idxs_to_buy, fct_if_sold);
    cout << name << " adding offer to idx " << at_idx << " and fct_if_sold " << fct_if_sold
        << " got utility delta " << utility_delta 
        << " and idx to buy instead " << idxs_to_buy.front() << endl;
    assert(utility_delta < 0);
    BidOffer toAdd = { (uint32_t) (-utility_delta) - (uint32_t) fct_if_sold + 1, name };
    slot.add_offer( toAdd );
}

static size_t num_packets_i_sent(const deque<Slot> &sent_slots, const string &my_name)
{
    size_t num_sent = 0;
    for (auto & slot : sent_slots) {
        if (slot.owner == my_name) {
            num_sent++;
        }
    }
    return num_sent;
}

void BasicUser::take_actions(Market& mkt)
{
    size_t num_packets_sent = num_packets_i_sent(mkt.sent_slots(), name);
    size_t num_packets_left_to_send = num_packets - num_packets_sent;

    deque<SingleSlot> &order_book = mkt.mutable_order_book();

    if (num_packets_left_to_send > 0) {
        size_t slots_owned = num_slots_owned(mkt.mutable_order_book(), name);
        cout << "I'm a basic user named " << name;
        cout << " I have a flow of size " << num_packets << " and have successfully sent " << num_packets_sent;
        cout << " packets and own " << slots_owned << " tentative future slots in the order book" << endl;

        if (num_packets_left_to_send > slots_owned) {
            get_best_slots(order_book, num_packets_left_to_send-slots_owned);
        }
    }

    for (size_t i = 0; i < order_book.size(); i++)
    {
        SingleSlot &cur_slot = order_book.at(i);
        if (cur_slot.owner == name and not cur_slot.has_offers())
        {
            add_offer_to_slot(mkt, i);
        }
    }
}

void BasicUser::print_stats(Market& mkt) const
{
        cout << "user " << name << " started at time " << flow_start_time << " and finished at time "
        << get_last_packet_sent_time(mkt.sent_slots(), name) << endl; // need to track money spent
}

void BasicUser::get_best_slots(deque<SingleSlot> &order_book, size_t num_packets_to_send)
{
    vector<size_t> idxs_to_buy;
    size_t cur_fct = current_flow_completion_time(order_book, name);
    recursive_pick_best_slots(order_book, 0, num_packets_to_send, idxs_to_buy, cur_fct);

    for (auto i : idxs_to_buy)
    {
        auto &slot = order_book.at(i);
        BidOffer toAdd = { slot.best_offer().cost, name };
        slot.add_bid( toAdd );
        cout << name << " making bid of $" << toAdd.cost << " to idx " << i;
        if (slot.owner == name)
        {
            cout << "... got slot!" << endl;
            //add_offer_to_slot(i);
        }
    }
    cout << endl;
}

int BasicUser::recursive_pick_best_slots(deque<SingleSlot> &order_book, size_t start, size_t n,
        vector<size_t> &idxs, size_t cur_fct)
{
    int best_utility = -11111;
    vector<size_t> best_idxs;
    if (order_book.size() < n) // make this cleaner later
    {
        return best_utility;
    }
    for (size_t i = start; i < order_book.size()-n; i++)
    {
        SingleSlot &cur_slot = order_book.at(i);
        if (cur_slot.owner != name and cur_slot.has_offers()) {
            vector<size_t> recursive_idxs = idxs; // hopefully copy
            recursive_idxs.emplace_back(i);

            int utility;
            // base case
            if (n == 1) {
                utility = -cur_slot.best_offer().cost - max(i, cur_fct);
            } else {
                utility = -cur_slot.best_offer().cost 
                    + recursive_pick_best_slots(order_book, i+1, n-1, recursive_idxs, cur_fct);
            }
            if (utility > best_utility) {
                best_utility = utility;
                best_idxs = recursive_idxs; //hopefully copy as well
            }
        }
    }
    idxs = best_idxs;
    //assert(best_utility != -11111)
    return best_utility;
}
