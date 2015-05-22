/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <unordered_map>
#include "market_emulator.hh"

using namespace std;

MarketEmulator::MarketEmulator( vector<unique_ptr<AbstractUser>> &&users)
: mkt_(), users_(move(users))
{
}

static bool all_users_done(const vector<unique_ptr<AbstractUser>> &users, const Market &mkt)
{
    for ( const auto & u : users ) {
        if (not u->done(mkt)) {
//            cout << "not all users done" << endl;
            return false;
        }
    }
    return true;
}

void MarketEmulator::users_take_actions_until_finished(vector<unique_ptr<AbstractUser>> &users, Market &mkt)
{
    while (true) {
        Market oldMkt = mkt;

        for ( auto & u : users ) {
            u->take_actions(mkt);
            print_slots();
        }

        if (oldMkt == mkt) {
            cout << "advancing time" << endl << endl;
            break;
        }
 //       cout << "market changed, taking user actions again" << endl;
    }
}

void MarketEmulator::run_to_completion()
{ 
    while ( not all_users_done( users_, mkt_ ) ) // some comments
    {
        users_take_actions_until_finished( users_, mkt_ );
        mkt_.advance_time();
    }
}

void MarketEmulator::print_slots()
{
    cout << "[ ";
    bool is_first = true;
    for (const SingleSlot &slot : mkt_.order_book())
    {
        if (is_first) {
            is_first = false;
        } else {
            cout << " | ";
        }
        cout << slot.time << ". ";

        if (slot.owner != "")
            cout << slot.owner;
        else
            cout << "null";

        cout << " $";
        if (slot.has_offers())
            cout << slot.best_offer().cost;
        else
            cout << "null";
    }

    cout << " ]" << endl;
}

void MarketEmulator::print_packets_sent()
{
    cout << "[ ";
    for (auto & pkt : mkt_.packets_sent()){
        cout << pkt.time << ". " << pkt.owner << "| ";
    }
    cout << "]" << endl;
}

void MarketEmulator::print_money_exchanged()
{
    cout << "final money owed: " << endl;
    unordered_map<string, double> money_owed;
    for ( auto &transaction : mkt_.money_exchanged() ) {
        auto pair = transaction.from + " to " + transaction.to;
        if (money_owed.count(pair) == 0){
            money_owed[pair] = 0;
        }
        money_owed[pair] += transaction.amount;
    }
    for ( auto &pair : money_owed ) {
        cout << pair.first << " $" << pair.second << endl;
    }
}

void MarketEmulator::print_user_stats()
{
    // print stats
    for ( auto & u : users_ ) {
        u->print_stats(mkt_);
    }
}
