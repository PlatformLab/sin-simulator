/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OWNER_USER
#define OWNER_USER

#include <iostream>
#include <deque>
#include "abstract_user.hh"
#include "market.hh"


class OwnerUser : public AbstractUser
{
    const uint32_t default_slot_offer;
    const size_t total_num_slots;

    public:
    OwnerUser(const std::string &name, const uint32_t default_slot_offer, const size_t total_num_slots);

    void take_actions(Market& mkt);

    void print_stats(Market& mkt) const;
};

#endif /* OWNER_USER */
