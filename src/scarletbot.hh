/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SCARLETBOT
#define SCARLETBOT

#include "abstract_user.hh"
#include "market.hh"

class ScarletBot: public AbstractUser{
    private:
    const size_t budget_;
    const size_t flow_start_time_;
    const size_t num_packets_;

    public:
    ScarletBot(const std::string &name, const size_t flow_start_time, const size_t num_packets, const size_t budget);

    void take_actions(Market& mkt) ;

    void print_stats(const Market& ) const;

};

#endif /* SCARLETBOT */
