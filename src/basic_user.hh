/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef BASIC_USER
#define BASIC_USER

#include <iostream>
#include <deque>
#include "abstract_user.hh"
#include "market.hh"

// make class owner user

// get a flow at time x of length n

class BasicUser : public AbstractUser
{
    const size_t num_packets;

    int recursive_pick_best_slots(std::deque<SingleSlot> &order_book, size_t start, size_t n,
            std::vector<size_t> &idxs, size_t cur_fct);

    public:
    BasicUser(const std::string &name, size_t flow_start_time, size_t num_packets);

    void add_offer_to_slot(Market &mkt, size_t at_idx);

    void take_actions(Market& mkt);

    void packet_sent();


    void get_best_slots(std::deque<SingleSlot> &order_book, size_t flow_size);

    private:
};

#endif /* BASIC_USER */
