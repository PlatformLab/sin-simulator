/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef BASIC_USER
#define BASIC_USER

#include <iostream>
#include <deque>
#include "abstract_user.hh"
#include "market.hh"

class BasicUser : public AbstractUser
{
    size_t flow_size;

    public:
    BasicUser(const std::string &name, size_t flow_size) : AbstractUser(name), flow_size(flow_size) {}

    void add_offer_to_slot(Market &mkt, size_t at_idx);

    void take_actions(struct Market& mkt);

    void packet_sent();


    void get_best_slots(std::deque<struct Slot> &order_book, size_t flow_size);

    private:
    int recursive_pick_best_slots(std::deque<struct Slot> &order_book, size_t start, size_t n,
            std::vector<size_t> &idxs, size_t cur_fct);

    bool done();
};

#endif /* BASIC_USER */
