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

    for (int i = 0; (size_t) i < order_book.size(); i++)
    {
        auto &slot_i = order_book.at(i);
        int flow_completion_time = i; //max(i, user.cur_completion_time);
        if (slot_i.owner->name != user.name) {
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

        std::cout << "bought slot " << best_slot_idx << " for " << order_book.at(best_slot_idx).cost << endl;
        order_book.at(best_slot_idx).cost++; // charge more for it
        return true;
    } 
    // couldnt buy
    return false;
}

void make_bids(struct user &user, int num_slots)
{
    int start_money = user.money;
    std::cout << "making " << num_slots << " slot purchases for " << user.name << endl;
    
    for (int i = num_slots; i > 0; i--)
    {
        bool success = buy_best_slot(user);
        assert(success);
    }

    std::cout << user.name << " finished bidding this round, spent " << start_money-user.money << endl;
        print_order_book();
}

int main(){
    struct user gregs{"gregs", 3, 100};
    struct user keith{"keith", 4, 100};

    int keith_slots = 0;
    int gregs_slots = 0;
    while (keith_slots != keith.flow_size || gregs_slots != gregs.flow_size) 
    {
        int gregs_to_buy = gregs.flow_size - gregs_slots; 
        int keith_to_buy = keith.flow_size - keith_slots; 

        if (gregs_to_buy)
            make_bids(gregs, gregs_to_buy);
        if (keith_to_buy)
            make_bids(keith, keith_to_buy);

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
