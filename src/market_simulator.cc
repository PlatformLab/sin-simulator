/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market_simulator.hh"

using namespace std;

static bool all_users_done(const vector<unique_ptr<AbstractUser>> &users, const Market &mkt)
{
    for ( const auto & u : users ) {
        if (not u->done(mkt)) {
            return false;
        }
    }
    return true;
}

class MarketSimulator {
    Market mkt_;
    std::vector<std::unique_ptr<AbstractBCP>> users_;

    void users_take_actions_until_finished()
    {
        bool all_done = true;
        do
        {
            all_done = true;
            for ( auto &u : users_ ) {
                    size_t before_version = mkt_.version();
                    u->take_actions( mkt_ );
                    bool market_unchanged = before_version == mkt_.version();
                    all_done &= market_unchanged;
                }
        } while ( not all_done )
    }

    public:
    MarketSimulator( std::vector<std::unique_ptr<AbstractUser>> &&users )
        : users_(move(users))
    {

    }
    void run_to_completion()
    {
        do
        {
            users_take_actions_until_finished();
            mkt_.advance_time();
        }
        while ( not market.empty() );
    }

    void print_outcome()
    {
        cout << "hello world" << endl;
    }
};
