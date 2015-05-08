#ifndef ABSTRACT_USER
#define ABSTRACT_USER

#include <iostream>
#include <cassert>
#include <vector>

struct Market;

class AbstractUser {
    public:
        const std::string name;
        struct Market& mkt; // dont do this

        AbstractUser(const std::string &name, struct Market& mkt) : name(name), mkt(mkt) {}

        virtual void take_actions() = 0;
};

#endif /* ABSTRACT_USER */
