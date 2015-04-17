#include <iostream>
#include <vector>
#include <queue>

using namespace std;

struct slot {
    int cost;
    string owner;
};

string comcast{"comcast"};

struct flow {
    size_t num_packets;
};

static std::vector<slot> order_book(100, {1, "comcast"});
//buy all until sent all, then move slots back until util optimized


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
            cost += order_book.at(idx).cost;
        }
        idx++;
    }
    value = idx; // time of last send
    // take most expensive packet and try to move it to maximize value-cost

    int cur_utility = (int) value - cost;
    while (idx < order_book.size())
    {
        const pair<struct slot, size_t> & top = tentative_order.top();
        int move_utility = idx - (cost - top.first.cost + order_book.at(idx).cost);
        if (move_utility > cur_utility)
        {
            cost -= top.first.cost + order_book.at(idx).cost;
            tentative_order.pop();
            tentative_order.push( std::make_pair( order_book.at(idx), idx) );

            cur_utility = move_utility;
        }
    }
}

int main(){
    struct flow a{3};
    struct flow b{4};
    make_bids("greg", a);
    make_bids("keith", b);

    std::cout << "hello world" << std::endl;
    return 1;
}
