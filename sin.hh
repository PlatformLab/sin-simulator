#include <iostream>
#include <vector>
#include <queue>
#include <cassert>


using namespace std;

struct user {
    string name;
    int flow_size;
    int cur_completion_time;
    int money;
};

struct slot {
    int cost;
    string owner;
};

static std::vector<slot> order_book(10, {1, "comcast"});

bool buy_best_slot(struct user &user)
{
    int best_slot_idx = -1;
    int best_slot_utility = -11111;

    for (int i = 0; i < order_book.size(); i++)
    {
        auto &slot_i = order_book.at(i);
        if (slot_i.owner != user.name) {
            int slot_i_utility = -i - slot_i.cost;
            if (slot_i_utility > best_slot_utility) {
                best_slot_utility = slot_i_utility;
                best_slot_idx = i;
            }
        }
    }
    if (best_slot_idx != -1) {
        order_book.at(best_slot_idx).owner = user.name;
        std::cout << "bought slot " << best_slot_idx << " for " << order_book.at(best_slot_idx).cost << endl;
        user.money -= order_book.at(best_slot_idx).cost;
        order_book.at(best_slot_idx).cost++; // charge more for it
        return true;
    } 
    // couldnt buy
    return false;
}

void make_bids(struct user &user)
{
    int start_money = user.money;
    std::cout << "making bids for " << user.name << endl;
    for (int i = user.flow_size; i > 0; i--)
    {
        bool success = buy_best_slot(user);
        assert(success);
    }
    std::cout << user.name << " finished bidding, spent " << start_money-user.money << endl;
}

int main(){
    struct user gregs{"gregs", 3, -11111, 100};
    struct user keith{"keith", 4, -11111, 100};
    make_bids(gregs);
    make_bids(keith);

    std::cout << "hello world" << std::endl;
    return 1;
}
