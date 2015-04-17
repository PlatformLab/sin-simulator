#include <iostream>
#include <vector>
#include <queue>
#include <cassert>


using namespace std;

struct slot {
    int cost;
    string owner;
};

string comcast{"comcast"};

struct flow {
    size_t num_packets;
};

static std::vector<slot> order_book(10, {1, "comcast"});

class Compare
{
public:
    bool operator() (const pair<struct slot, size_t> &a, pair<struct slot, size_t> &b)
    {
        return std::less<int>()(a.first.cost, b.first.cost);
    }
};

void make_bids(string user, struct flow flow)
{
    priority_queue<pair<struct slot, size_t>, vector<pair<struct slot, size_t>>, Compare> tentative_order;
    size_t value = 0;
    int cost = 0;

    size_t idx = 0;
    while (tentative_order.size() < flow.num_packets)
    {
        if (order_book.at(idx).owner != user)
        {
            tentative_order.push(std::make_pair( order_book.at(idx), idx));
            cout << "tentatively bought " << idx << " for " <<order_book.at(idx).cost<< endl;
            cost += order_book.at(idx).cost;
        }
        idx++;
    }
    value = -(idx - 1); // time of last send
    // take most expensive packet and try to move it to maximize value-cost

    int cur_utility = (int) value - cost;
    cout << "cur_utility is " << cur_utility << endl;
    while (idx < order_book.size())
    {
        const pair<struct slot, size_t> & top = tentative_order.top();
        int move_utility = -idx - (cost - top.first.cost + order_book.at(idx).cost);
        cout << "utility diff of swapping most expensive slot to slot idx "<< idx << " is " << move_utility - cur_utility << endl;
        if (move_utility > cur_utility)
        {
            cout << "swapping" << endl;
            cost = cost - top.first.cost + order_book.at(idx).cost;
            tentative_order.pop();
            tentative_order.push( std::make_pair( order_book.at(idx), idx) );

            cur_utility = move_utility;
        }
        idx++;
    }

    int total_cost = 0;
    // make bids
    while (not tentative_order.empty())
    {
        const pair<struct slot, size_t> & top = tentative_order.top();
        assert(order_book.at(top.second).cost == top.first.cost);
        assert(order_book.at(top.second).owner == top.first.owner);
        total_cost += order_book.at(top.second).cost;
        order_book.at(top.second) = { 5, user };
        tentative_order.pop();
    }
    cout << total_cost << " " << cost << endl;
    assert( total_cost == cost);
}

int main(){
    struct flow a{3};
    struct flow b{4};
    make_bids("greg", a);
    make_bids("keith", b);

    std::cout << "hello world" << std::endl;
    return 1;
}
