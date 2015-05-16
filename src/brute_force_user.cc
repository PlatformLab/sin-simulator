
#include "brute_force_user.hh" 

using namespace std;

BruteForceUser::BruteForceUser(const std::string &name, const size_t flow_start_time, const size_t num_packets, std::function<int(std::list<size_t>&)> utility_func)
: AbstractUser(name),
    flow_start_time_(flow_start_time),
    num_packets_(num_packets),
    utility_func_(utility_func)
{
}

list<list<size_t>> BruteForceUser::potential_idxs(const deque<SingleSlot> &order_book, size_t start, size_t len)
{
    if (len == 0)
        return {{}};

    list<list<size_t>> toRet = {};
    for (size_t i = start; i < order_book.size(); i++) {
        if (order_book.at(i).owner != name_) {
            /*
            if (len == 1) {
                list<size_t> v { i };
                toRet.emplace_back( v );
                cout << "this is what we get " << potential_idxs(order_book, i+1, len-1).size() << endl;
            } else {
                */
                for (list<size_t> &vec : potential_idxs(order_book, i+1, len-1)) {
                    vec.emplace_front(i);
                    toRet.emplace_back(vec);
                }
            //}
        }
    }
    return toRet;
}

uint32_t cost_of_slots(const deque<SingleSlot> &order_book, const list<size_t> &idxs)
{
    uint32_t toRet = 0;
    for (size_t i : idxs) {
        toRet += order_book.at(i).best_offer().cost;
    }
    return toRet;
}

template <typename T>
size_t num_owned_in_deque(deque<T> deque, const string &name)
{
    size_t toRet = 0;
    for (T& t : deque) {
        if (t.owner == name) {
            toRet++;
        }
    }
    return toRet;
}

void BruteForceUser::take_actions(Market& mkt)
{
    const deque<SingleSlot> &order_book = mkt.order_book();
    if (order_book.empty())
        return;

    size_t num_packets_to_buy = num_packets_ - num_owned_in_deque(mkt.packets_sent(), name_) - num_owned_in_deque(order_book, name_); 
    list<size_t> best_idxs = {};
    int max_net_utility = INT_MIN;
    for (list<size_t> &idxs : potential_idxs(order_book, flow_start_time_, num_packets_to_buy))
    {
        /*
        cout << "[";
        for (size_t idx : idxs) {
            cout << " " << idx << ",";
        }
        cout << "]" << " costs " << cost_of_slots(order_book, idxs) << " and has utility " << utility_func_(idxs) <<  endl;
        */
        int net_utility = utility_func_(idxs) - cost_of_slots(order_book, idxs);
        if (net_utility > max_net_utility) {
            max_net_utility = net_utility;
            best_idxs = idxs;
        }
    }

    cout << "buying slots ";
    for (size_t i : best_idxs) {
        cout << i << ", ";
        mkt.add_bid_to_slot( i, { order_book.at(i).best_offer().cost, name_ } );
    }
    cout << endl;
}

void BruteForceUser::print_stats(const Market& ) const
{
        cout << "user " << name_ << " started at time " << flow_start_time_ << " and finished at time "
        <<  endl; // need to track money spent
}
