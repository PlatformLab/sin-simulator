/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef BRUTE_FORCE_USER
#define BRUTE_FORCE_USER

#include <iostream>
#include <deque>
#include <list>
#include "abstract_user.hh"
#include "market.hh"

class BruteForceUser : public AbstractUser
{
    private:
    const size_t flow_start_time_;
    const size_t num_packets_;
    std::function<int(std::list<size_t>&, size_t)> utility_func_;

    public:
    BruteForceUser(const std::string &name, const size_t flow_start_time, const size_t num_packets, std::function<int(std::list<size_t>&, size_t)> utility_func);

    void take_actions(Market& mkt);

    bool done(const Market& mkt) const;

    void print_stats(const Market& mkt) const;
};

#endif /* BRUTE_FORCE_USER */
