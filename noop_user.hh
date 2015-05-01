#ifndef NOOP_USER
#define NOOP_USER

#include <iostream>
#include "abstract_user.hh"

class NoopUser : public AbstractUser
{
    public:
    struct User_actions get_actions(std::vector<struct Slot_view> &given_views)
    {
        std::cout << std::endl << "I'm a noop user!" << std::endl;
        print_slot_views(given_views);
        std::cout << "end noop user!" << std::endl << std::endl;
        return {};
    }
};

#endif /* NOOP_USER */
