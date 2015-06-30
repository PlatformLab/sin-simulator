/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef FLOW_COMPLETION_TIME_USER
#define FLOW_COMPLETION_TIME_USER

#include <iostream>
#include <functional>
#include <deque>
#include <limits>
#include <list>
#include "abstract_user.hh"
#include "pretty_print.hh"
#include "market.hh"

class FlowCompletionTimeUser : public AbstractUser
{
    private:
    const size_t flow_start_time_;
    const size_t flow_num_packets_;
    bool done_ = false;
    double money_spent_ = 0;
    double expected_utility_ = std::numeric_limits<double>::lowest();
    double best_expected_utility_ = std::numeric_limits<double>::lowest();

    double get_benefit( size_t finish_time ) const;
    bool can_buy( const SingleSlot &slot ) const;

    double get_sell_price( const std::deque<SingleSlot> &order_book, const size_t last_slot_time, const double current_benefit ) const;
    void price_owned_slots( Market &mkt );

    std::vector<size_t> pick_n_slots_to_buy( const std::deque<SingleSlot> &order_book,
            const size_t num_packets_to_buy, const size_t latest_time_already_owned ) const;

    public:
    FlowCompletionTimeUser( const std::string &name, const size_t flow_start_time, const size_t num_packets );

    void take_actions( Market& mkt ) override;

    bool done( const Market& mkt ) override;

    void print_stats( const Market& mkt ) const override;

    double cost( const Market& ) const;
    double utility( const Market& ) const override;
    double best_expected_utility() const override;
    double benefit( const Market& ) const override;
};

#endif /* FLOW_COMPLETION_TIME_USER */
