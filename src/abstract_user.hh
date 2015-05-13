/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ABSTRACT_USER
#define ABSTRACT_USER

#include <iostream>
#include <cassert>
#include <vector>

struct Market;

class AbstractUser {
    public:
        const std::string name;

        AbstractUser(const std::string &name) : name(name) { }

        virtual void take_actions( Market& mkt ) = 0;

        virtual void print_stats( Market& mkt ) const = 0;

        virtual ~AbstractUser() = default;
};

#endif /* ABSTRACT_USER */
