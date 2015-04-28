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

    bool owns(struct slot & slot) { return (slot.owner->name == name); }
};

static struct user comcast{"ccast", -1, 0};
static vector<slot> order_book(9, {1, &comcast});

bool buy_best_slot(struct user &user);
void price_slot(struct user &user, size_t idx_to_sell);
void price_all_slots()
{
    for (int i = (order_book.size()-1); i >= 0; i--)
    {
        auto &slot = order_book.at(i);
        if (slot.owner->name != "ccast")
            price_slot(*slot.owner, i);
    }
}

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
