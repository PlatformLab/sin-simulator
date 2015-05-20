/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <unordered_map>
#include "market_emulator.hh"

using namespace std;

static void settle_money_printout(const std::deque<MoneyExchanged> &transactions) 
{
    cout << "final money owed: " << endl;
    //unordered_map<pair<string, string>, uint32_t> money_owed;
    unordered_map<string, uint32_t> money_owed;
    for ( auto &transaction : transactions ) {
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

MarketEmulator::MarketEmulator( vector<unique_ptr<AbstractUser>> &&users)
: mkt_(), users_(move(users))
{
}

static bool all_users_done(const vector<std::unique_ptr<AbstractUser>> &users, const Market &mkt)
{
    for ( const auto & u : users ) {
        if (not u->done(mkt)) {
            return false;
        }
    }
    return true;
}

static void users_take_actions_until_finished(vector<std::unique_ptr<AbstractUser>> &users, Market &mkt)
{
    while (true) {
        Market oldMkt = mkt;

        for ( auto & u : users ) {
            u->take_actions(mkt);
        }

        if (oldMkt == mkt) {
            break;
        }
    }
}

void MarketEmulator::run_to_completion()
{ 
    cout << "run to completion" << endl;

    while ( not all_users_done( users_, mkt_ ) ) // some comments
    {
        users_take_actions_until_finished( users_, mkt_ );
        mkt_.advance_time();
    }

    // print stats
    for ( auto & u : users_ ) {
        u->print_stats(mkt_);
    }

    settle_money_printout(mkt_.money_exchanged());
    cout << "[ ";
    for (auto & pkt : mkt_.packets_sent()){
        cout << pkt.time << ". " << pkt.owner << "| ";
    }
    cout << "]" << endl;
}
