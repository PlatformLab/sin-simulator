/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <unordered_map>
#include "market_emulator.hh"

using namespace std;

static void settle_money_printout(const std::deque<MoneyExchanged> &transactions) 
{
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

void MarketEmulator::run_to_completion()
{ 
    cout << "run to completion" << endl;

    Market oldMkt;

    do {
        do {
            oldMkt = mkt_;
            for ( auto & u : users_ ) {
                u->take_actions(mkt_);
            }
        } while (oldMkt != mkt_);

        mkt_.advance_time();
    } while (oldMkt != mkt_);

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
