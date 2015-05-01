#ifndef ABSTRACT_USER
#define ABSTRACT_USER

#include <iostream>
#include <cassert>
#include <vector>

#include "common.hh"

class AbstractUser {
    uint32_t money;
    uint32_t flow_size;
    std::string name;

    public:
    std::string &get_name(){
        return name;
    }
    
    virtual struct User_actions get_actions(std::vector<struct Slot_view> &given_view) = 0;
};

#endif /* ABSTRACT_USER */
