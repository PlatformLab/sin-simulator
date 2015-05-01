#ifndef ABSTRACT_USER
#define ABSTRACT_USER

#include <iostream>
#include <cassert>
#include <vector>

#include "market.hh"

class AbstractUser {
    std::string name;

    public:
    std::string &get_name(){
        return name;
    }
    
    virtual void take_actions(struct Market& mkt) = 0;
};

#endif /* ABSTRACT_USER */
