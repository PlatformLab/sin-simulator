#ifndef NOOP_USER
#define NOOP_USER

#include <iostream>
#include "abstract_user.hh"

class NoopUser : public AbstractUser
{
    public:
//        NoopUser(const std::string &name) = default;
        void take_actions(struct Market& mkt)
        {
            std::cout << "I'm a noop user!" << std::endl;
        }

        void packet_sent() { }
};

#endif /* NOOP_USER */
