/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef NOOP_USER
#define NOOP_USER

#include "abstract_user.hh"
#include "market.hh"

class NoopUser : public AbstractUser
{
    public:
    NoopUser( const std::string &name ) : AbstractUser( name ) { };

    void take_actions( Market& ) override { std::cout << "noop" << std::endl; }

    bool done( const Market& ) const override { return true; };

    void print_stats( const Market& ) const override { std::cout << "noop stats" << std::endl; }
};

#endif /* NOOP_USER */
