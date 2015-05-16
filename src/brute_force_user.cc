
#include "brute_force_user.hh" 

using namespace std;

BruteForceUser::BruteForceUser(const std::string &name, const size_t flow_start_time, const size_t num_packets)
: AbstractUser(name),
    flow_start_time_(flow_start_time),
    num_packets_(num_packets)
{
}

list<list<size_t>> BruteForceUser::potential_idxs(const deque<SingleSlot> &order_book, size_t start, size_t len)
{
    list<list<size_t>> toRet = {};
    for (size_t i = start; i < order_book.size(); i++) {
        if (order_book.at(i).owner != name_) {
            if (len == 1) {
                list<size_t> v { i };
                toRet.emplace_back( v );
            } else {
                for (list<size_t> &vec : potential_idxs(order_book, i+1, len-1)) {
                    vec.emplace_front(i);
                    toRet.emplace_back(vec);
                }
            }
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

void BruteForceUser::take_actions(Market& mkt)
{
    const deque<SingleSlot> &order_book = mkt.order_book();
    if (order_book.empty())
        return;

    for (list<size_t> &idxs : potential_idxs(order_book, flow_start_time_, num_packets_))
    {
        cout << "[";
        for (size_t idx : idxs) {
            cout << " " << idx << ",";
        }
        cout << "]" << endl;
    }
}

void BruteForceUser::print_stats(const Market& ) const
{
        cout << "user " << name_ << " started at time " << flow_start_time_ << " and finished at time "
        <<  endl; // need to track money spent
}
