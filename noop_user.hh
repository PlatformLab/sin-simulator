#ifndef NOOP_USER
#define NOOP_USER

#include <iostream>
#include "abstract_user.hh"

class NoopUser : public AbstractUser
{
    public:
        void take_actions(struct Market& mkt)
        {
            std::cout << "I'm a noop user!" << std::endl;
        }
};

#endif /* NOOP_USER */
