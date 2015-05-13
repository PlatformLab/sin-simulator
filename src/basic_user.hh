/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef BASIC_USER
#define BASIC_USER

#include <iostream>
#include <deque>
#include "abstract_user.hh"
#include "market.hh"

class BasicUser : public AbstractUser
{
    public:
    const size_t flow_start_time;
    const size_t num_packets;

    BasicUser(const std::string &name, const size_t flow_start_time, const size_t num_packets);

    void take_actions(Market& mkt);

    void print_stats(Market& mkt) const;

    private:
    void add_offer_to_slot(Market &mkt, size_t at_idx);
    int recursive_pick_best_slots(std::deque<SingleSlot> &order_book, size_t start, size_t n,
            std::vector<size_t> &idxs, size_t cur_fct);

    void get_best_slots(std::deque<SingleSlot> &order_book, size_t flow_size);
};

#endif /* BASIC_USER */
