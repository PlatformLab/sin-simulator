/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <unordered_map>
#include "market_emulator.hh"
#include "pretty_print.hh"

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
            Market beforeMkt = mkt;
            u->take_actions(mkt);
            if (beforeMkt != mkt) { // they actually did something
                print_slots();
            }
        }

        if (oldMkt == mkt) {
            //cout << "advancing time" << endl << endl;
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
        if ( mkt_.order_book().empty() ){
            return;
        }
    }
}

void MarketEmulator::print_slots()
{
    printOrderBook(mkt_.order_book());
}

void MarketEmulator::print_packets_sent()
{
    cout << "Final packets sent are:" << endl;
    printPacketsSent(mkt_.packets_sent());
}

void MarketEmulator::print_money_exchanged()
{
    unordered_map<string, double> money_owed;
    unordered_map<string, double> money_total;
    // track money owed in both directions (a to b $1 and b to a $-1 i.e.)
    // print one that is net positive at end
    for ( auto &transaction : mkt_.money_exchanged() ) {
        string pair1 = transaction.from + " to " + transaction.to;
        string pair2 = transaction.to + " to " + transaction.from;

        if (money_owed.count(pair1) == 0){
            money_owed[pair1] = 0;
        }
        if (money_owed.count(pair2) == 0){
            money_owed[pair2] = 0;
        }
        money_owed[pair1] += transaction.amount;
        money_owed[pair2] -= transaction.amount;

        if (money_total.count(transaction.to) == 0){
            money_total[transaction.to] = 10;
        }
        if (money_total.count(transaction.from) == 0){
            money_total[transaction.from] = 10;
        }
        money_total[transaction.to] += transaction.amount;
        money_total[transaction.from] -= transaction.amount;
    }
    cout << "final money owed: " << endl;
    for ( auto &pair : money_owed ) {
        if (pair.second > 0) {
            cout << pair.first << " $" << pair.second << endl;
        }
    }

    cout << "balance after (started with 10 each): " << endl;
    for ( auto &pair : money_total ) {
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
