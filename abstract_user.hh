#ifndef ABSTRACT_USER
#define ABSTRACT_USER

#include <iostream>
#include <cassert>
#include <vector>

#include "common.hh"

class AbstractUser {
    std::string name;

    public:
    std::string &get_name(){
        return name;
    }
    
    virtual struct User_actions get_actions(std::vector<struct Slot_view> &given_view) = 0;

    void print_slot_views(std::vector<struct Slot_view> &given_views)
    {
        std::cout << "{ ";
        bool first = true;
        for(auto &slot : given_views)
        {
            if (first)
                first = false;
            else 
                std::cout << " | ";

            std::cout << "time: " << slot.time;
            std::cout << " current_offer: " << slot.current_offer;
            if (slot.you_own)
                std::cout << " owned by me";
            else
                std::cout << " not owned";
        }
        std::cout << " }" << std::endl;
    }
};

#endif /* ABSTRACT_USER */
