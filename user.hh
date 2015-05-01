#ifndef USER
#define USER

#include <iostream>
#include <cassert>
#include <vector>

#include "common.hh"

class User {
    uint32_t money;
    uint32_t flow_size;
    std::string name;

    public:
    std::string &get_name(){
        return name;
    }
    
    struct User_actions get_actions(std::vector<struct Slot_view> &given_view)
    {
        return {};
    }
};

#endif /* USER */
