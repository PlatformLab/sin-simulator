/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OWNER_USER
#define OWNER_USER

#include <iostream>
#include <deque>
#include "abstract_user.hh"
#include "market.hh"


class OwnerUser : public AbstractUser
{
    public:
    OwnerUser(const std::string &name) : AbstractUser(name) {}

    void take_actions(Market& ) {}

    void print_stats(Market& ) const {}
};

#endif /* OWNER_USER */
