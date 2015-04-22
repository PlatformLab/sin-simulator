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
    user* owner;
};

static struct user comcast{"ccast", -1, -1, 0};
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
        int flow_completion_time = max(i, user.cur_completion_time);
        if (slot_i.owner->name != user.name) {
            int slot_i_utility = -flow_completion_time - slot_i.cost;
            cout << "i " << i << " utility " << slot_i_utility << endl;
            if (slot_i_utility > best_slot_utility) {
                best_slot_utility = slot_i_utility;
                best_slot_idx = i;
            }
        } else {
            cout << "couldnt buy slot " << i << ": already owned" << endl;

        }
    }
    if (best_slot_idx != -1) {
        user.money -= order_book.at(best_slot_idx).cost;
        order_book.at(best_slot_idx).owner->money += order_book.at(best_slot_idx).cost;

        order_book.at(best_slot_idx).owner = &user;

        std::cout << "bought slot " << best_slot_idx << " for " << order_book.at(best_slot_idx).cost << endl;
        order_book.at(best_slot_idx).cost++; // charge more for it

        user.cur_completion_time = max(user.cur_completion_time, best_slot_idx);
        std::cout << "completion time now " << user.cur_completion_time << endl;
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

    std::cout << user.name << " finished bidding, spent " << start_money-user.money <<
        " and flow finishes at time " << user.cur_completion_time << endl;
        print_order_book();
}

int main(){
    struct user gregs{"gregs", 3, -11111, 100};
    struct user keith{"keith", 4, -11111, 100};
    make_bids(gregs);
    make_bids(keith);

    return 1;
}
