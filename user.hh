#ifndef USER
#define USER

#include <iostream>
#include <cassert>
#include <vector>

#include "common.hh"

struct User {
    uint32_t money;
    uint32_t flow_size;
    std::string name;

    struct Buyer get_buyer_identity(){
        return {name};
    }
    
    std::vector<struct Slot_action> take_actions(std::vector<struct Slot_view> &given_view)
    {
        return {};
    }
};

#endif /* USER */
