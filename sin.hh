#include <iostream>
#include <vector>
#include <queue>
#include <cassert>

using namespace std;

struct user;

struct slot {
    int cost;
    user* owner;
};

struct user {
    string name;
    int flow_size;
    int money;

    bool owns(struct slot & slot)
    {
        return (slot.owner->name == name);
    }
};

static struct user comcast{"ccast", -1, 0};
static vector<slot> order_book(9, {1, &comcast});

void print_order_book()
{
    int i = 0;
    cout << "[";
    for (auto &slot : order_book)
    {
        cout << i++ << ":" << slot.owner->name << "$" << slot.cost << " | "; 
    }
    cout << "]" << endl;
}

void price_slot(struct user &user, size_t idx_to_sell)
{
    auto &slot_to_sell = order_book.at(idx_to_sell);
    assert(user.owns(slot_to_sell));
    assert(idx_to_sell < order_book.size());

    size_t flow_completion_time_if_sold = 0;
    size_t flow_completion_time = 0;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        if (user.owns(order_book.at(i)))
            flow_completion_time = i;

        if (i != idx_to_sell && user.owns(order_book.at(i)))
            flow_completion_time_if_sold = i;
    }

    int best_move_utility_delta = -1111111;
    int best_move_idx = -1;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        auto &slot_i = order_book.at(i);
        if (not user.owns(slot_i)) {
            int old_value = -flow_completion_time;
            int new_value = - max(i, flow_completion_time_if_sold);
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
    assert(best_move_utility_delta <= 0);
    slot_to_sell.cost = -best_move_utility_delta + 1;
    cout << user.name << " priced slot " << idx_to_sell << " at $" << slot_to_sell.cost << endl;
}

bool buy_best_slot(struct user &user)
{
    int best_slot_idx = -1;
    int best_slot_utility = -11111;

    size_t max_flow_completion_time = 0;
    for (size_t i = 0; i < order_book.size(); i++)
    {
        if (user.owns(order_book.at(i))) {
            max_flow_completion_time = i;
        }
    }

    for (size_t i = 0; i < order_book.size(); i++)
    {
        auto &slot_i = order_book.at(i);
        int flow_completion_time = max(i, max_flow_completion_time);
        if (not user.owns(order_book.at(i))) {
            int slot_i_utility = -flow_completion_time - slot_i.cost;
            if (slot_i_utility > best_slot_utility) {
                best_slot_utility = slot_i_utility;
                best_slot_idx = i;
            }
        }
    }
    if (best_slot_idx != -1) {
        user.money -= order_book.at(best_slot_idx).cost;
        order_book.at(best_slot_idx).owner->money += order_book.at(best_slot_idx).cost;

        order_book.at(best_slot_idx).owner = &user;

        cout << user.name << " bought slot " << best_slot_idx << " for " << order_book.at(best_slot_idx).cost << endl;
        return true;
    } 
    // couldnt buy
    assert(false);
    return false;
}

void price_all_slots()
{
    for (size_t i = 0; i < order_book.size(); i++)
    {
        auto &slot = order_book.at(i);
        if (slot.owner->name != "ccast")
            price_slot(*slot.owner, i);
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
        // tally slot ownership and price slots
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
    }
    return 1;
}
