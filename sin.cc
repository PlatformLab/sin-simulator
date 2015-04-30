#include "sin.hh"

bool buy_best_slot(struct user &user)
{
    int best_slot_idx = -1;
    int best_slot_utility = -11111;

    size_t old_flow_completion_time = 0;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        if (user.owns(order_book.at(i))) {
            old_flow_completion_time = i;
        }
    }

    for (size_t i = 0; i < order_book.size(); i++)
    {
        auto &slot_i = order_book.at(i);
        int new_flow_completion_time = max(i, old_flow_completion_time);
        if (not user.owns(order_book.at(i))) {
            int slot_i_utility = -new_flow_completion_time - slot_i.cost;
            if (slot_i_utility > best_slot_utility) {
                best_slot_utility = slot_i_utility;
                best_slot_idx = i;
            }
        }
    }

    // buy best slot
    if (best_slot_idx != -1) {
        user.money -= order_book.at(best_slot_idx).cost;
        order_book.at(best_slot_idx).owner->money += order_book.at(best_slot_idx).cost;

        string &old_owner = order_book.at(best_slot_idx).owner->name;
        order_book.at(best_slot_idx).owner = &user;

        cout << user.name << " bought slot " << best_slot_idx << " for " << order_book.at(best_slot_idx).cost << " from " << old_owner << endl;
        return true;
    } 

    // couldnt buy
    assert(false);
    return false;
}

// slots are priced such that if sold, the user could buy the next best slot
// and their net utility increase would be 1
void price_slot(struct user &user, size_t idx_to_sell)
{
    auto &slot_to_sell = order_book.at(idx_to_sell);
    assert(user.owns(slot_to_sell));
    assert(idx_to_sell < order_book.size());

    size_t flow_completion_time_if_sold = 0;
    size_t old_flow_completion_time = 0;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        if (user.owns(order_book.at(i)))
            old_flow_completion_time = i;

        if (i != idx_to_sell && user.owns(order_book.at(i)))
            flow_completion_time_if_sold = i;
    }

    int best_move_utility_delta = -1111111;
    int best_move_idx = -1;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        auto &slot_i = order_book.at(i);
        if (not user.owns(slot_i)) {
            int old_value = -old_flow_completion_time;
            int new_value = -max(i, flow_completion_time_if_sold);
            int value_delta = new_value - old_value;

            int cost_delta = slot_i.cost;
            int utility_delta = value_delta - cost_delta;

            if (utility_delta > best_move_utility_delta) {
                best_move_utility_delta = utility_delta;
                best_move_idx = i;
            }
        } 
    }
    assert(best_move_idx != -1);
    slot_to_sell.cost = -best_move_utility_delta + 1;
    cout << user.name << " priced slot " << idx_to_sell << " at $" << slot_to_sell.cost;
    cout << " (should move to slot " << best_move_idx << " if bought)" << endl;
    //assert(best_move_utility_delta <= 0);
}

void price_slot_pair(size_t first_idx)
{
    struct slot& slot_a = order_book.at(first_idx);
    struct slot& slot_b = order_book.at(first_idx+1);
    if (slot_a.owner->name != slot_b.owner->name) {
        slot_a.twos_cost = -1;
        slot_b.twos_cost = -1;
    }
    struct user& user = *slot_a.owner;

    size_t flow_completion_time_if_sold = 0;
    size_t old_flow_completion_time = 0;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        if (user.owns(order_book.at(i)))
            old_flow_completion_time = i;

        if (i != first_idx && i != first_idx+1 && user.owns(order_book.at(i)))
            flow_completion_time_if_sold = i;
    }

    int best_move_utility_delta = -1111111;
    int best_move_idx = -1;
    for (size_t i = 0; i < order_book.size()-1; i = i+2)
    {
        auto &slot_i = order_book.at(i);
        if (slot_i.twos_cost > 0 and not user.owns(slot_i)) {
            int old_value = -old_flow_completion_time;
            int new_value = -max(i+1, flow_completion_time_if_sold);
            int value_delta = new_value - old_value;

            int cost_delta = slot_i.twos_cost;
            int utility_delta = value_delta - cost_delta;

            if (utility_delta > best_move_utility_delta) {
                best_move_utility_delta = utility_delta;
                best_move_idx = i;
            }
        } 
    }

    if (best_move_idx != -1)
    {
        slot_a.twos_cost = slot_b.twos_cost = -best_move_utility_delta + 1;
        cout << user.name << " priced slot pair (" << first_idx << ", " << first_idx+1 << ") at $" << slot_a.twos_cost;
        cout << " (should move to slot pair starting at " << best_move_idx << " if bought)" << endl;
        //assert(best_move_utility_delta <= 0);
    } else {
        order_book.at(first_idx).twos_cost = order_book.at(first_idx+1).twos_cost = -1;
    }
}

int main(){
    struct user gregs{"gregs", 3, 100};
    struct user keith{"keith", 4, 100};

    int keith_slots = 0;
    int gregs_slots = 0;

    while (keith_slots != keith.flow_size || gregs_slots != gregs.flow_size) 
    {
        if (gregs.flow_size > gregs_slots) {
            buy_best_slot(gregs);
            price_all_slots();
            print_order_book();
        }
        if (keith.flow_size > keith_slots) {
            buy_best_slot(keith);
            price_all_slots();
            print_order_book();
        }

        // tally slot ownership
        keith_slots = 0;
        gregs_slots = 0;
        for (size_t i = 0; i < order_book.size(); i++)
        {
            auto &slot = order_book.at(i);
            if (slot.owner->name == "gregs")
                gregs_slots++;
            else if (slot.owner->name == "keith")
                keith_slots++;
        }
        cout << "gregs money $" << gregs.money << endl;
        cout << "keith money $" << keith.money << endl;
        cout << "end of round" << endl << endl;
    }
    return 1;
}
