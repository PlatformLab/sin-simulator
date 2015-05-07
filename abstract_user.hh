#ifndef ABSTRACT_USER
#define ABSTRACT_USER

#include <iostream>
#include <cassert>
#include <vector>

struct Market;

class AbstractUser {
    public:
        const std::string name;

        AbstractUser(const std::string &name) : name(name) {}

        virtual void take_actions(struct Market& mkt) = 0;
};

#endif /* ABSTRACT_USER */
