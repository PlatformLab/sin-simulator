#include <iostream>
#include <vector>
#include <queue>
#include <cassert>

using namespace std;

struct user {
    string name;
    int flow_size;
    int money;
};

struct slot {
    int cost;
    user* owner;
};

static struct user comcast{"ccast", -1, 0};
static std::vector<slot> order_book(9, {1, &comcast});

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

bool buy_best_slot(struct user &user)
{
    int best_slot_idx = -1;
    int best_slot_utility = -11111;
    int second_best_slot_utility = -11111;

    for (int i = 0; (size_t) i < order_book.size(); i++)
    {
        auto &slot_i = order_book.at(i);
        int flow_completion_time = i;
        if (slot_i.owner->name != user.name) {
            int slot_i_utility = -flow_completion_time - slot_i.cost;
            if (slot_i_utility > best_slot_utility) {
                second_best_slot_utility = best_slot_utility;

                best_slot_utility = slot_i_utility;
                best_slot_idx = i;
            } else if (slot_i_utility > second_best_slot_utility) {
                second_best_slot_utility = slot_i_utility;
            }
        }
    }
    if (best_slot_idx != -1) {
        user.money -= order_book.at(best_slot_idx).cost;
        order_book.at(best_slot_idx).owner->money += order_book.at(best_slot_idx).cost;

        order_book.at(best_slot_idx).owner = &user;

        cout << user.name << " bought slot " << best_slot_idx << " for " << order_book.at(best_slot_idx).cost << endl;
        // now price slot
        if (second_best_slot_utility == -11111) {
            cout << "whyyyy" << endl;
            order_book.at(best_slot_idx).cost += 10;
        } else {
            int utility_delta = best_slot_utility - second_best_slot_utility;
            cout << "got utility delta " << utility_delta << endl;
            assert(utility_delta >= 0);

            cout << "increasing slot price by " << utility_delta + 1<< endl;
            order_book.at(best_slot_idx).cost += utility_delta + 1;
        }
        print_order_book();
        return true;
    } 
    // couldnt buy
    assert(false);
    return false;
}

int main(){
    struct user gregs{"gregs", 3, 100};
    struct user keith{"keith", 4, 100};

    int keith_slots = 0;
    int gregs_slots = 0;
    while (keith_slots != keith.flow_size || gregs_slots != gregs.flow_size) 
    {
        if (gregs.flow_size > gregs_slots)
            buy_best_slot(gregs);
        if (keith.flow_size > keith_slots)
            buy_best_slot(keith);

        // tally slot ownership
        keith_slots = 0;
        gregs_slots = 0;
        for (auto &slot : order_book)
        {
            if (slot.owner->name == "gregs")
                gregs_slots++;
            else if (slot.owner->name == "keith")
                keith_slots++;
        }
    }
    return 1;
}
