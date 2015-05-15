
#include "basic_user.hh" 

using namespace std;

static void print_slots(const deque<SingleSlot> &slots)
{
    cout << "[ ";
    bool is_first = true;
    for (const SingleSlot &slot : slots)
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
            cout << slot.best_offer().cost;
        else
            cout << "null";
    }

    cout << " ]" << endl;
}

static size_t num_slots_owned(const deque<SingleSlot> &order_book, const string &name)
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
static size_t get_last_packet_sent_time(const Market &mkt, const std::string &name)
{
    size_t highest_sent_time = 0;
    for ( auto & packet_sent : mkt.packets_sent() ) {
        if ( packet_sent.owner == name ) {
            highest_sent_time = packet_sent.time;
        }
    }
    return highest_sent_time;
}

static size_t current_flow_completion_time(const deque<SingleSlot> &order_book, const string &name)
{
    size_t highest_owned_slot_idx = 0;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        if (order_book.at(i).owner == name) {
            highest_owned_slot_idx = i;
        }
    }
    return order_book.at(highest_owned_slot_idx).time;
}

static size_t flow_completion_time_if_sold(const deque<SingleSlot> &order_book, const string &name, size_t idx_to_sell)
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
    flow_start_time_(flow_start_time),
    num_packets_(num_packets)
{
}

void BasicUser::add_offer_to_slot(Market &mkt, size_t at_idx)
{
    const deque<SingleSlot> &order_book = mkt.order_book();
    assert(order_book.at(at_idx).owner == name_);

    // price and make offer
    vector<size_t> idxs_to_buy;
    size_t fct_if_sold = flow_completion_time_if_sold(order_book, name_, at_idx);
    int utility_delta = recursive_pick_best_slots(order_book, 0, 1, idxs_to_buy, fct_if_sold);
    cout << name_ << " adding offer to idx " << at_idx << " and fct_if_sold " << fct_if_sold
        << " got utility delta " << utility_delta 
        << " and idx to buy instead " << idxs_to_buy.front() << endl;
    assert(utility_delta < 0);

    mkt.add_offer_to_slot(at_idx, { (uint32_t) (-utility_delta) - (uint32_t) fct_if_sold + 1, name_ } );
}

static size_t num_packets_sent(Market &mkt, const string &name)
{
    size_t num_sent = 0;
    for ( auto & packet_sent : mkt.packets_sent() ) {
        if (packet_sent.owner == name) {
            num_sent++;
        }
    }
    return num_sent;
}

void BasicUser::take_actions(Market& mkt)
{
    size_t packets_sent = num_packets_sent(mkt, name_);
    size_t num_packets_left_to_send = num_packets_ - packets_sent;

    const deque<SingleSlot> &order_book = mkt.order_book();

    if (num_packets_left_to_send > 0) {
        size_t slots_owned = num_slots_owned(mkt.order_book(), name_);
        cout << "I'm a basic user named " << name_;
        cout << " I have a flow of size " << num_packets_ << " and have successfully sent " << packets_sent;
        cout << " packets and own " << slots_owned << " tentative future slots in the order book" << endl;

        if (num_packets_left_to_send > slots_owned) {
            get_best_slots(mkt, num_packets_left_to_send-slots_owned);
        }
    }

    for (size_t i = 0; i < order_book.size(); i++)
    {
        const SingleSlot &cur_slot = order_book.at(i);
        if (cur_slot.owner == name_ and not cur_slot.has_offers())
        {
            add_offer_to_slot(mkt, i);
        }
    }
    print_slots(order_book);
}

void BasicUser::print_stats(const Market& mkt) const
{
        cout << "user " << name_ << " started at time " << flow_start_time_ << " and finished at time "
        << get_last_packet_sent_time(mkt, name_) << endl; // need to track money spent
}

void BasicUser::get_best_slots(Market& mkt, size_t num_packets_to_send)
{
    auto & order_book = mkt.order_book();
    vector<size_t> idxs_to_buy;
    size_t cur_fct = order_book.empty() ? 0 : current_flow_completion_time(order_book, name_);
    recursive_pick_best_slots(order_book, 0, num_packets_to_send, idxs_to_buy, cur_fct);

    for (auto i : idxs_to_buy)
    {
        mkt.add_bid_to_slot(i, { order_book.at(i).best_offer().cost, name_ } );

        cout << name_ << " making bid to idx " << i;
        if (order_book.at(i).owner == name_)
        {
            cout << "... got slot!" << endl;
            //add_offer_to_slot(i);
        }
    }
    cout << endl;
}

int BasicUser::recursive_pick_best_slots(const deque<SingleSlot> &order_book, size_t start, size_t n,
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
        const SingleSlot &cur_slot = order_book.at(i);
        if (cur_slot.owner != name_ and cur_slot.has_offers()) {
            vector<size_t> recursive_idxs = idxs; // hopefully copy
            recursive_idxs.emplace_back(i);

            int utility;
            // base case
            if (n == 1) {
//                int flow_length =  - flow_start_time_;
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
