/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef FLOW_COMPLETION_TIME_USER
#define FLOW_COMPLETION_TIME_USER

#include <iostream>
#include <functional>
#include <deque>
#include <queue>
#include <list>
#include "abstract_user.hh"
#include "pretty_print.hh"
#include "market.hh"

class FlowCompletionTimeUser : public AbstractUser
{
    private:
    const size_t flow_start_time_;
    const size_t num_packets_;
    bool done_;

    public:
    FlowCompletionTimeUser( const std::string &name, const size_t flow_start_time, const size_t num_packets );

    void take_actions( Market& mkt ) override;

    bool done( const Market& mkt ) override;

    void print_stats( const Market& mkt ) const override;
};

#endif /* FLOW_COMPLETION_TIME_USER */
