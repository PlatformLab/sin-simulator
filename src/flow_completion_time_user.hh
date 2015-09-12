/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef FLOW_COMPLETION_TIME_USER
#define FLOW_COMPLETION_TIME_USER

#include <iostream>
#include "abstract_user.hh"
#include "market.hh"
#include "flow.hh"


class FlowCompletionTimeUser : public AbstractUser
{
    const size_t num_packets_;
    size_t done_ = 0;

    public:
    FlowCompletionTimeUser( Flow flow )
        : AbstractUser( flow.uid, flow.start ),
        num_packets_( flow.num_packets )
    {
    }

    void take_actions( Market& ) override
    {
    }
};

#endif /* FLOW_COMPLETION_TIME_USER */
