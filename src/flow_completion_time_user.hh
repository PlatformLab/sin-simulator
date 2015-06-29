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
    size_t previous_market_version_ = 0;

    double get_benefit( std::priority_queue<std::pair<double, size_t>> costs_and_indices_to_buy ) const;
    bool can_use( const SingleSlot &slot ) const;

    std::priority_queue<std::pair<double, size_t>> pick_n_slots_to_buy( const std::deque<SingleSlot> &order_book,
            const size_t num_packets_to_buy, const size_t time_to_ignore ) const;

    public:
    FlowCompletionTimeUser( const size_t &uid, const size_t flow_start_time, const size_t num_packets );

    void take_actions( Market& mkt ) override;

    bool done( const Market& mkt ) override;

    void print_stats( const Market& mkt ) const override;

    double cost( const Market& ) const;
    double utility( const Market& ) const override;
    double best_expected_utility() const override;
    double benefit( const Market& ) const override;
};

#endif /* FLOW_COMPLETION_TIME_USER */
