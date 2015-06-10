/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <unordered_map>
#include <unordered_set>
#include "market_emulator.hh"
#include "pretty_print.hh"

using namespace std;

MarketEmulator::MarketEmulator( vector<unique_ptr<AbstractUser>> &&users, const bool verbose, const bool random_user_order )
: mkt_(),
users_(move(users)),
verbose_(verbose),
random_user_order_(random_user_order)
{
}

static bool all_users_done(const vector<unique_ptr<AbstractUser>> &users, const Market &mkt)
{
    for ( const auto & u : users ) {
        if (not u->done(mkt)) {
            return false;
        }
    }
    return true;
}

size_t MarketEmulator::next_idx(size_t last_idx)
{
    return ( random_user_order_ ? rand() : last_idx + 1 ) % users_.size();
}

void MarketEmulator::users_take_actions_until_finished()
{
    unordered_set<size_t> idxs_finished;
    size_t idx_to_run = next_idx( -1 );

    while ( idxs_finished.size() < users_.size() ) {

        while ( idxs_finished.count(idx_to_run) > 0 ) { // if idx already done, skip it
            idx_to_run = next_idx( idx_to_run );
        }

        const Market oldMkt = mkt_;

        users_.at(idx_to_run)->take_actions(mkt_);
        if (oldMkt == mkt_) { // user didn't do anything
            idxs_finished.insert(idx_to_run);
        } else {
            idxs_finished.clear(); // let everyone run again if someone changed market
            if (verbose_) {
                cout << users_.at(idx_to_run)->name_ << " took actions:" << endl;
                print_slots();
                cout << "_____________________" << endl;
            }
        }

        idx_to_run = next_idx( idx_to_run );
    }
}

void MarketEmulator::run_to_completion()
{ 
    while ( not all_users_done( users_, mkt_ ) ) // some comments
    {
        users_take_actions_until_finished();
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
    printPacketsSent(mkt_.packets_sent());
}

void MarketEmulator::print_money_exchanged()
{
    unordered_map<string, double> money_owed;
    unordered_map<string, double> money_total;

    // track money owed in both directions symmetrically (a to b $1 and b to a $-1 i.e.)
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
            money_total[transaction.to] = 0;
        }
        if (money_total.count(transaction.from) == 0){
            money_total[transaction.from] = 0;
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

    cout << "net balance: " << endl;
    for ( auto &pair : money_total ) {
            cout << pair.first << " $" << pair.second << endl;
    }
}

void MarketEmulator::print_user_stats()
{
    for ( auto & u : users_ ) {
        u->print_stats(mkt_);
    }
}
